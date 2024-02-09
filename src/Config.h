#pragma once

namespace game
{

  struct Config
  {
    struct Physics
    {
      float maxDt;
      float gravity;

      struct Car
      {
        float mass;
        float enginePower;
        float brakePower;
        float handBrakePower;
        float maxSpeed;
        float minSteeringAngle;
        float maxSteeringAngle;
        float maxSteeringSpeed;
        float carAligningForce;
        float bodyFriction;
        float aerodynamicKoef;
        float speedSteeringDrop;
      } car;

      struct Turret
      {
        float minPitch;
        float maxPitch;
        float yawSpeed;
        float pitchSpeed;
        float bodyFriction;
      } turret;

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
        float minDistance;
        float maxDistance;
        float maxPitch;
        float horzSensitivity;
        float vertSensitivity;
        bool invertY;
      } camera;

      struct Hud
      {
        int fontSize;
        int screenMargins;
      } hud;

      struct Resources
      {
        const char* fontPath;
        const char* terrainTexturePath;
        const char* carModelPath;
        const char* wheelModelPath;
        const char* turretModelPath;
      } resources;
    } graphics;
  };

}