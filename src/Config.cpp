#include "pch.h"
#include "Config.h"

namespace game
{

  Config Config::DEFAULT =
  {
    .physics = {
      .maxDt = 0.02f,
      .gravity = 9.8f,
      .car = {
        .mass = 1500,
        .enginePower = 100,
        .brakePower = 10000,
        .handBrakePower = 100000,
        .maxSpeed = 50,
        .minSteeringAngle = PI / 8,
        .maxSteeringAngle = PI / 4,
        .maxSteeringSpeed = PI / 2,
        .carAligningForce = 15,
        .bodyFriction = 0.05f,
        .aerodynamicKoef = 5.7f,
        .connectionPoints = {
          .wheels = {
            .frontLeft = { 0.97f, -0.36f, 1.34f },
            .frontRight = { -0.97f, -0.36f, 1.34f },
            .rearLeft = { 0.97f, -0.536f, -1.20f },
            .rearRight = { -0.97f, -0.536f, -1.20f },
          },
          .weapon = {
            .gun = { 0, 0.45f, -0.2f },
            .cannon = { 0, 0.55f, -1.6f }
          }
        }
      },
      .frontWheels = {
        .mass = 20,
        .radius = 0.5f,
        .suspensionStiffness = 100000,
        .suspensionDamping = 0.2f,
        .maxSuspensionOffset = 0.5f,
        .tireFriction = 0.9f,
        .rollingFriction = 0.01f,
      },
      .rearWheels = {
        .mass = 20,
        .radius = 0.5f,
        .suspensionStiffness = 200000,
        .suspensionDamping = 0.2f,
        .maxSuspensionOffset = 0.25f,
        .tireFriction = 0.9f,
        .rollingFriction = 0.01f,
      },
      .gun = {
        .minPitch = -PI / 4 ,
        .maxPitch = PI / 16,
        .minYaw = -PI * 3 / 4,
        .maxYaw = PI * 3 / 4,
        .rotationSpeed = PI / 2,
        .bodyFriction = 0.1f,
        .fireInterval = 0.1f,
        .projectileSpeed = 300,
        .baseDamage = 10,
        .projectileLifeTime = 2.0f,
        .barrelPosition = { 0, 0.35f, 1.5f},
      },
      .cannon = {
        .minPitch = -PI / 8 ,
        .maxPitch = PI / 32,
        .minYaw = -PI / 4,
        .maxYaw = PI / 4,
        .rotationSpeed = PI / 8,
        .bodyFriction = 0.1f,
        .fireInterval = 1.0f,
        .projectileSpeed = 100,
        .baseDamage = 100,
        .projectileLifeTime = 3.0f,
        .barrelPosition = { 0, 0.75f, 3.0f},
      },
      .cannonExplosion = {
        .radius = 3,
        .duration = 0.5f,
        .force = 100000,
      },
    },
    .graphics = {
      .screen = {
        .title = "Mad Max",
        .width = 1280,
        .height = 720,
      },
      .camera = {
        .focusElevation = 5,
        .minDistance = 10,
        .maxDistance = 20,
        .maxPitch = PI * 3 / 8,
        .horzSensitivity = 1.0f,
        .vertSensitivity = 1.0f,
        .invertY = false,
      },
      .hud = {
        .fontSize = 20,
        .screenMargins = 10,
      },
      .bulletExplosionParticles = {
        .count = 10,
        .minSize = 0.1f,
        .maxSize = 0.3f,
        .minSpeed = 5,
        .maxSpeed = 10,
        .minAngularSpeed = PI,
        .maxAngularSpeed = 8 * PI,
        .minLifeTime = 0.5f,
        .maxLifeTime = 1.0f,
      },
      .shellExplosionParticles = {
        .count = 100,
        .minSize = 0.3f,
        .maxSize = 0.5f,
        .minSpeed = 10,
        .maxSpeed = 20,
        .minAngularSpeed = PI,
        .maxAngularSpeed = 8 * PI,
        .minLifeTime = 1.0f,
        .maxLifeTime = 2.0f,
      },
      .resources = {
        .fontPath = "resources/fonts/JetBrainsMono-Bold.ttf",
        .terrainTexturePath = "resources/textures/terrain.png",
        .tree1TexturePath = "resources/textures/tree1.png",
        .tree2TexturePath = "resources/textures/tree2.png",
        .rockTexturePath = "resources/textures/rock.png",
        .carModelPath = "resources/models/jeep.gltf",
        .wheelModelPath = "resources/models/wheel.gltf",
        .gunModelPath = "resources/models/gun.gltf",
        .cannonModelPath = "resources/models/cannon.gltf",
        .tree1ModelPath = "resources/models/tree1.gltf",
        .tree2ModelPath = "resources/models/tree2.gltf",
        .rockModelPath = "resources/models/rock.gltf",
        .crosshairsTexturePath = "resources/textures/crosshairs_white.png",
      },
    }
  };

}