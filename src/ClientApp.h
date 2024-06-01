#pragma once
#include "Config.h"
#include "ServerConfig.h"
#include "Scene.h"
#include "Hud.h"
#include "Renderer.h"
#include "IClientMessageHandler.h"
#include "RakNetClient.h"
#include "MatchStats.h"

namespace game
{
  using namespace network;
  using namespace dto;

  struct ClientApp : public IClientMessageHandler
  {
    static constexpr float SYNC_FACTOR = 0.5f;
    volatile bool exit;

    const Config& config;
    const WindowConfig& windowConfig;
    CustomCamera camera;
    Scene scene;
    Hud hud;
    Renderer renderer;
    RakNetClient network;
    MatchStats matchStats {};

    float dtAccumulator = 0;
    bool paused = false;
    bool drawDebugInfo = false;

    ClientApp(const Config& config, const WindowConfig& windowConfig, const ServerConfig& serverConfig);

    void initialize();
    bool pulse();
    void shutdown();
    void togglePaused();
    void update(float dt);
    void updateShortcuts();
    void sendPlayerControl(const PlayerControl& playerControl);
    void updateCamera(float dt, float lerpFactor);
    PlayerControl getLocalPlayerControl();

    virtual void onConnected(uint64_t guid) override;
    virtual void onDisconnected(uint64_t guid) override;
    virtual void onPlayerJoin(const PlayerJoin& playerJoin) override;
    virtual void onPlayerLeave(const PlayerLeave& playerLeave) override;
    virtual void onPlayerControl(const PlayerControl& playerControl) override;
    virtual void onPlayerState(const PlayerState& playerState) override;
    virtual void onPlayerHit(const PlayerHit& playerHit) override;
    virtual void onPlayerKill(const PlayerKill& playerKill) override;
    virtual void onMatchState(const MatchState& matchState) override;
  };

}