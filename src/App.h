#pragma once
#include "Config.h"
#include "Scene.h"
#include "Hud.h"
#include "Renderer.h"
#include "IClientMessageHandler.h"
#include "RakNetClient.h"

namespace game
{
  using namespace network;
  using namespace dto;

  struct App : public IClientMessageHandler
  {
    static constexpr float SYNC_FACTOR = 0.5f;
    volatile bool exit;

    Config config {};
    CustomCamera camera;
    Scene scene;
    Hud hud;
    Renderer renderer;
    RakNetClient network;

    bool paused = false;

    App();

    void initialize();
    void run();
    void shutdown();
    void togglePaused();
    void update(float dt);
    void updateLocalPlayerControl();
    void updateShortcuts();
    void sendLocalPlayerState();
    void updateCamera(float dt, float lerpFactor);
    PlayerControl getLocalPlayerControl();

    virtual void onConnected(uint64_t guid) override;
    virtual void onDisconnected(uint64_t guid) override;
    virtual void onPlayerJoin(const PlayerJoin& playerJoin) override;
    virtual void onPlayerLeave(const PlayerLeave& playerLeave) override;
    virtual void onPlayerControl(const PlayerControl& playerControl) override;
    virtual void onPlayerState(const PlayerState& playerState) override;
  };

}