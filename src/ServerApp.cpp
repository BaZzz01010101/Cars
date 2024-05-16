#include "core.h"
#include "ServerApp.h"
#include "Helpers.h"
#include "PlayerJoin.h"
#include "PlayerLeave.h"
#include "PlayerHit.h"
#include "PlayerKill.h"

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

      scene.update(config.physics.fixedDt);
      sendPlayerStates();
      sendPlayerHits();
      sendPlayerKills();
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
        scene.getPlayerState(i).writeTo(stream);
        const Car& car = scene.cars[i];

        if(car.isRespawning())
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
      BitStream stream;
      playerKill.writeTo(stream);
      network.broadcast(stream, true);
    }
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
    scene.respawnCar(newPlayer);

    PlayerJoin playerJoin = {
      .physicalFrame = scene.localPhysicalFrame,
      .guid = guid,
      .position = newPlayer.position,
      .rotation = newPlayer.rotation,
    };

    BitStream stream;
    playerJoin.writeTo(stream);
    network.broadcastExcept(stream, guid, true);

    for (int i = 0; i < scene.cars.capacity(); i++)
      if (scene.cars.isAlive(i))
      {
        Car& player = scene.cars[i];

        PlayerJoin playerJoin = {
          .physicalFrame = scene.localPhysicalFrame,
          .guid = player.guid,
          .position = player.position,
          .rotation = player.rotation,
        };

        BitStream stream;
        playerJoin.writeTo(stream);
        network.send(stream, guid, true);
      }
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
  }

  void ServerApp::onPlayerControl(const PlayerControl& playerControl)
  {
    scene.updatePlayerControl(playerControl);

    BitStream stream;
    playerControl.writeTo(stream);
    network.broadcast(stream, false);
  }

}
