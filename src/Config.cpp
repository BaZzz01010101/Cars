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
        .enginePower = 200,
        .brakePower = 800,
        .handBrakePower = 100000,
        .maxSpeed = 50,
        .minSteeringAngle = PI / 8,
        .maxSteeringAngle = PI / 4,
        .maxSteeringSpeed = PI / 2,
        .carAligningForce = 15,
        .bodyFriction = 0.5f,
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
        .tireFriction = 0.95f,
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
    },
    .collisionGeometries = {
      .carPoints = {
        {  0.81f,  0.11f,  2.34f },
        {  0.73f,  1.37f,  0.61f },
        {  0.64f,  1.32f, -0.81f },
        {  0.68f,  0.43f, -2.19f },
        {  0.97f, -0.15f, -1.88f },
        { -0.81f,  0.11f,  2.34f },
        { -0.73f,  1.37f,  0.61f },
        { -0.64f,  1.32f, -0.81f },
        { -0.68f,  0.43f, -2.19f },
        { -0.97f, -0.15f, -1.88f },
      },
      .carSpheres = {
        {{  0.32f, 0.22f,  1.42f }, 0.75f },
        {{ -0.32f, 0.22f,  1.42f }, 0.75f },
        {{ -0.06f, 0.64f, -1.45f }, 1.23f },
        {{ -0.05f, 0.61f,  0.00f }, 1.10f },
      },
      .tree1Spheres = {
        {{  0.000f, -0.330f,  0.000f }, 1.990f },
        {{  0.000f,  2.145f,  0.165f }, 1.330f },
        {{ -0.000f,  3.630f,  0.495f }, 1.330f },
        {{  0.000f,  5.610f,  0.660f }, 1.495f },
        {{ -0.000f,  8.250f,  0.990f }, 1.660f },
        {{ -3.036f,  7.831f, -1.584f }, 1.957f },
        {{ -4.356f, 10.693f, -0.066f }, 2.848f },
        {{  3.498f, 11.383f, -1.386f }, 2.584f },
        {{  2.673f, 12.637f,  5.016f }, 3.838f },
        {{ -0.677f, 13.937f,  1.132f }, 5.452f }
      },
      .tree2Spheres = {
        {{  0.000f, -1.650f,  0.000f }, 3.640f },
        {{ -0.165f,  2.310f, -0.165f }, 1.330f },
        {{ -0.000f,  3.960f, -0.330f }, 1.330f },
        {{  0.000f,  5.940f, -0.495f }, 1.495f },
        {{ -1.150f,  9.900f,  0.495f }, 4.465f },
        {{ -0.396f, 14.431f,  0.231f }, 5.092f },
        {{  3.894f, 12.178f,  0.759f }, 3.673f },
        {{ -0.132f, 16.993f, -4.851f }, 1.924f },
        {{ -2.772f, 19.237f,  1.056f }, 2.188f },
        {{ -3.812f, 21.856f,  2.617f }, 1.162f }
      },
      .rockSpheres = {
        {{ -0.657f,  0.628f, -3.883f }, 1.115f },
        {{ -1.067f, -0.170f, -2.687f }, 1.847f },
        {{ -1.064f, -0.414f, -0.277f }, 2.077f },
        {{ -0.656f,  0.430f,  1.935f }, 1.658f },
        {{  0.238f, -0.663f,  2.621f }, 1.855f },
        {{  0.038f, -1.904f,  0.353f }, 3.805f },
        {{  1.836f,  0.532f, -1.097f }, 1.073f },
        {{  0.770f,  0.440f, -2.912f }, 1.234f },
        {{  0.424f,  1.715f, -1.137f }, 1.764f },
        {{  0.479f,  2.540f,  0.769f }, 1.567f },
      },
    }
  };

}