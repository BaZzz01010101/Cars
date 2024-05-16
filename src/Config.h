#pragma once

namespace game
{

  struct Config
  {
    struct Physics
    {
      float fixedDt;
      float gravity;

      struct Car
      {
        float mass;
        float enginePower;
        float brakePower;
        float handBrakePower;
        float maxForwardSpeed;
        float maxBackwardSpeed;
        float minSteeringAngle;
        float maxSteeringAngle;
        float maxSteeringSpeed;
        float carAligningForce;
        float bodyFriction;
        float aerodynamicKoef;
        float speedSteeringDrop;
        float frontRearPowerDistributionRatio;
        int maxHealth;

        struct
        {
          struct
          {
            vec3 frontLeft;
            vec3 frontRight;
            vec3 rearLeft;
            vec3 rearRight;
          } wheels;

          struct
          {
            vec3 gun;
            vec3 cannon;
          } weapon;

        } connectionPoints;
      } car;

      struct Wheels
      {
        float mass;
        float radius;
        float suspensionStiffness;
        float suspensionDamping;
        float maxSuspensionOffset;
        float tireFriction;
        float rollingFriction;
      } frontWheels, rearWheels;

      struct Turret
      {
        float minPitch;
        float maxPitch;
        float minYaw;
        float maxYaw;
        float rotationSpeed;
        float bodyFriction;
        float fireInterval;
        float projectileSpeed;
        int baseDamage;
        float projectileLifeTime;
        float barrelElevation;
        float barrelLength;
      } gun, cannon;

      struct Explosion
      {
        float radius;
        float duration;
        float force;
      } cannonExplosion;
    } physics;

    struct Graphics
    {
      struct Screen
      {
        const char* title;
        int width;
        int height;
      } screen;

      struct Camera
      {
        float focusElevation;
        float maxDistanceFromFocus;
        float minPitch;
        float maxPitch;
        float horzSensitivity;
        float vertSensitivity;
        float rotationSharpness;
        float pursuitSharpness;
        float collisionSharpness;
        float zoomFactor;
      } camera;

      struct Hud
      {
        int fontSize;
        int screenMargins;
      } hud;

      struct ExplosionParticles
      {
        int count;
        float minSize;
        float maxSize;
        float minSpeed;
        float maxSpeed;
        float minAngularSpeed;
        float maxAngularSpeed;
        float minLifeTime;
        float maxLifeTime;
      } bulletExplosionParticles, shellExplosionParticles, carExplosionParticles;

      struct Resources
      {
        const char* fontPath;
        const char* terrainTexturePath;
        const char* tree1TexturePath;
        const char* tree2TexturePath;
        const char* rockTexturePath;
        const char* carModelPath;
        const char* wheelModelPath;
        const char* gunModelPath;
        const char* cannonModelPath;
        const char* tree1ModelPath;
        const char* tree2ModelPath;
        const char* rockModelPath;
        const char* crosshairsTexturePath;
      } resources;
    } graphics;

    struct CollisionGeometries
    {
      vec3 carPoints[10];
      Sphere carSpheres[4];
      Sphere tree1Spheres[10];
      Sphere tree2Spheres[10];
      Sphere rockSpheres[10];
    } collisionGeometries;

    struct Multiplayer
    {
    } multiplayer;

    static const Config DEFAULT;
  };

}