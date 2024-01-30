#pragma once
struct Config
{
  struct Physics
  {
    float maxDt;

    struct Car
    {
      float mass;
      float enginePower;
      float brakePower;
      float handBrakePower;
      float maxSpeed;
      float maxSteeringAngle;
      float maxSteeringSpeed;
      float carAligningForce;
      float bodyFriction;
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
  } physics;

  struct Graphics
  {
    struct Hud
    {
      float fontSize;
      float screenMargins;
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

