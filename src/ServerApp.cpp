#include "core.h"
#include "ServerApp.h"
#include "Helpers.h"
#include "PlayerJoin.h"
#include "PlayerLeave.h"
#include "PlayerHit.h"
#include "PlayerKill.h"
#include "MatchState.h"

namespace game
{
  ServerApp::ServerApp(const Config& config, const ServerConfig& serverConfig) :
    exit(false),
    config(config),
    network(serverConfig, *this),
    scene(config, true),
    maxPlayers(serverConfig.maxPlayers)
  {
  }

  void ServerApp::initialize()
  {
    printf("Server started:\n");
    scene.init();
    network.start();
    maxSleep = nanoseconds(0);

    for (int i = 0; i < 100; i++)
    {
      time_point tp = clock.now();
      std::this_thread::sleep_for(milliseconds(0));
      maxSleep = std::max(maxSleep, clock.now() - tp);
    }

    maxSleep *= 2;
    lastUpdateTime = clock.now();
  }

  bool ServerApp::pulse()
  {
    static const nanoseconds fixedDt(uint64_t(nanoseconds::period::den * config.physics.fixedDt));

    time_point now = clock.now();
    nanoseconds elapsed = now - lastUpdateTime;

    if (elapsed < fixedDt - maxSleep)
      std::this_thread::sleep_for(milliseconds(1));
    else while (elapsed >= fixedDt)
    {
      elapsed -= fixedDt;

      if (scene.cars.count() > 0)
      {
        scene.update(config.physics.fixedDt);
        updateMatchRestart();
        sendPlayerStates();
        sendPlayerHits();
        sendPlayerKills();
      }

      network.update();
    }

    lastUpdateTime = now - elapsed;

    return !exit;
  }

  void ServerApp::shutdown()
  {
    network.stop();
  }

  void ServerApp::sendPlayerStates()
  {
    for (int i = 0; i < scene.cars.capacity(); i++)
      if (scene.cars.isAlive(i))
      {
        BitStream stream;
        PlayerState playerState = scene.getPlayerState(i);
        playerState.ping = network.getPlayerPing(scene.cars[i].guid);
        playerState.writeTo(stream);
        const Car& car = scene.cars[i];

        // When player is still respawning we send state only to him to avoid potential abuse the new position of the player by cheat software
        if (car.aliveState == Car::AliveState::Countdown)
          network.send(stream, car.guid, false);
        else
          network.broadcast(stream, false);
      }
  }

  void ServerApp::sendPlayerHits()
  {
    for (int i = 0; i < scene.cars.capacity(); i++)
      if (scene.cars.isAlive(i))
        for (const PlayerHit& playerHit : scene.getPlayerHits(i))
        {
          BitStream stream;
          playerHit.writeTo(stream);
          network.broadcast(stream, false);
        }
  }

  void ServerApp::sendPlayerKills()
  {
    for (const PlayerKill& playerKill : scene.getPlayerKills())
    {
      matchStats.addDeath(playerKill.guid);
      matchStats.addKill(playerKill.killerGuid);

      BitStream stream;
      playerKill.writeTo(stream);
      network.broadcast(stream, true);
    }
  }

  void ServerApp::updateMatchRestart()
  {
    if (scene.matchState == Scene::Scoreboard && scene.getMatchStateTimeout() <= 0)
    {
      matchStats.reset();
      scene.reset();

      if (scene.cars.count() == 1)
        scene.switchToMatchState(Scene::Running);
      else
      {
        scene.switchToMatchState(Scene::Countdown);
        scene.respawnAllPlayers();
      }

      broadcastMatchState(true);
    }
  }

  void ServerApp::sendMatchState(uint64_t guid, bool resetMatchStats)
  {
    BitStream stream;

    MatchState {
      .matchTimeout = scene.matchTimeout,
      .shouldResetMatchStats = resetMatchStats,
    }.writeTo(stream);

    network.send(stream, guid, true);
  }

  void ServerApp::broadcastMatchState(bool resetMatchStats)
  {
    BitStream stream;

    MatchState {
      .matchTimeout = scene.matchTimeout,
      .shouldResetMatchStats = resetMatchStats,
    }.writeTo(stream);

    network.broadcast(stream, true);
  }

  PlayerName ServerApp::getRandomPlayerName()
  {
    static constexpr int PLAYER_NAMES_COUNT = sizeof(PLAYER_NAMES) / sizeof(PLAYER_NAMES[0]);
    int freeNamesCount = PLAYER_NAMES_COUNT - scene.cars.count();
    int skipCount = randi(freeNamesCount - 1);

    PlayerName name {};

    for (int i = 0; i < PLAYER_NAMES_COUNT; i++)
    {
      name = PLAYER_NAMES[i];

      for (int j = 0; j < scene.cars.capacity(); j++)
        if (scene.cars.isAlive(j) && scene.cars[j].name == name)
          goto name_exists;

      if (skipCount-- == 0)
        break;

    name_exists:;
    }

    if (name[0] == '\0')
      snprintf(name.data(), PLAYER_NAME_BUF_SIZE, "Player: %i", scene.cars.count());

    return name;
  }

  void ServerApp::onClientConnected(uint64_t guid)
  {
    if (scene.cars.count() >= maxPlayers)
    {
      printf("SERVER_APP: OnClientConnected: %" PRIu64 ". Disconnecting! Players limit reached.\n", guid);
      network.disconnectClient(guid, true);

      return;
    }

    int newPlayerIndex = scene.cars.tryAdd(guid, config, scene);

    if (newPlayerIndex < 0)
    {
      printf("SERVER_APP: OnClientConnected: %" PRIu64 ". Disconnecting! Players pool overflow.\n", guid);
      network.disconnectClient(guid, true);

      return;
    }

    printf("SERVER_APP: OnClientConnected: %" PRIu64 ". Players: %i\n", guid, scene.cars.count());
    Car& newPlayer = scene.cars[newPlayerIndex];
    newPlayer.name = getRandomPlayerName();

    int playersCount = scene.cars.count();

    if (playersCount == 1)
    {
      scene.reset();
      scene.respawnPlayer(newPlayer, false);
      scene.switchToMatchState(Scene::Running);
      sendMatchState(guid, true);
    }
    else if (playersCount == 2)
    {
      if (scene.matchState == Scene::Scoreboard)
      {
        scene.respawnPlayer(newPlayer, false);
        sendMatchState(guid, false);
      }
      else
      {
        scene.reset();
        scene.respawnAllPlayers();
        scene.switchToMatchState(Scene::Countdown);
        broadcastMatchState(true);
      }
    }
    else
    {
      scene.respawnPlayer(newPlayer, true);
      sendMatchState(guid, false);
    }

    // TODO: Need to find a way to hide the car on client who is currently joining with countdown
    PlayerJoin playerJoin = {
      .physicalFrame = scene.localPhysicalFrame,
      .guid = guid,
      .name = newPlayer.name,
      .position = newPlayer.position,
      .rotation = newPlayer.rotation,
      .kills = 0,
      .deaths = 0,
    };

    BitStream stream;
    playerJoin.writeTo(stream);
    network.broadcastExcept(stream, guid, true);

    for (int i = 0; i < scene.cars.capacity(); i++)
      if (scene.cars.isAlive(i))
      {
        Car& player = scene.cars[i];
        PlayerStats* playerStats = matchStats.tryGetStats(player.guid);
        int playerKills = playerStats ? playerStats->kills : 0;
        int playerDeaths = playerStats ? playerStats->deaths : 0;

        PlayerJoin playerJoin = {
          .physicalFrame = scene.localPhysicalFrame,
          .guid = player.guid,
          .name = player.name,
          .position = player.position,
          .rotation = player.rotation,
          .kills = playerKills,
          .deaths = playerDeaths,
        };

        BitStream stream;
        playerJoin.writeTo(stream);
        network.send(stream, guid, true);
      }

    matchStats.addPlayer(newPlayer.guid, 0, 0);
    sendPlayerStates();
  }

  void ServerApp::onClientDisconnected(uint64_t guid)
  {
    printf("SERVER_APP: OnClientDisconnected: %" PRIu64 "\n", guid);

    for (int i = 0; i < scene.cars.capacity(); i++)
      if (scene.cars.isAlive(i) && scene.cars[i].guid == guid)
      {
        scene.cars.remove(i);
        break;
      }

    printf("SERVER_APP: Players left: %i\n", scene.cars.count());

    PlayerLeave playerLeave = {
      .guid = guid,
    };

    BitStream stream;
    playerLeave.writeTo(stream);
    network.broadcast(stream, true);

    int playersCount = scene.cars.count();

    if (playersCount == 0)
      scene.reset();
    else if (playersCount == 1)
    {
      if (scene.matchState == Scene::Running)
      {
        scene.switchToMatchState(Scene::Scoreboard);
        broadcastMatchState(false);
      }
      else if(scene.matchState == Scene::Countdown)
      {
        scene.reset();
        scene.switchToMatchState(Scene::Running);
        broadcastMatchState(true);
      }
    }
  }

  void ServerApp::onPlayerControl(const PlayerControl& playerControl)
  {
    scene.updatePlayerControl(playerControl);

    BitStream stream;
    playerControl.writeTo(stream);
    network.broadcast(stream, false);
  }

}
