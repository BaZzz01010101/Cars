#include "core.h"
#include "ServerApp.h"
#include "Helpers.h"
#include "PlayerJoin.h"
#include "PlayerLeave.h"

namespace game
{
  ServerApp::ServerApp(const Config& config, const ServerConfig& serverConfig) :
    exit(false),
    config(config),
    network(serverConfig, *this),
    scene(config),
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
    else if (elapsed >= fixedDt)
    {
      lastUpdateTime += fixedDt;

      scene.update(config.physics.fixedDt);
      sendPlayerStates();
      network.update();
    }

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
        network.broadcast(stream, false);
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

    int playerIndex = scene.cars.tryAdd(guid, config, scene);

    if (playerIndex < 0)
    {
      printf("SERVER_APP: OnClientConnected: %" PRIu64 ". Disconnecting! Players pool overflow.\n", guid);
      network.disconnectClient(guid, true);

      return;
    }

    printf("SERVER_APP: OnClientConnected: %" PRIu64 ". Players: %i\n", guid, scene.cars.count());
    scene.localPlayerIndex = playerIndex;
    Car& player = scene.getLocalPlayer();
    float x = randf(-10, 10);
    float z = randf(-10, 10);
    vec3 normal;
    float y = scene.terrain.getHeight(x, z, &normal);
    player.position = { x, y + 3, z };
    player.rotation = quat::fromAxisAngle(normal, randf(2.0f * PI));

    PlayerJoin playerJoin = {
      .physicalFrame = scene.localPhysicalFrame,
      .guid = guid,
      .position = player.position,
      .rotation = player.rotation,
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
  }

}
