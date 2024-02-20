#include "pch.h"
#include "Turret.h"
#include "Helpers.h"

namespace game
{
  void Turret::init(const Config::Physics::Turret& config, const Model& model, const Terrain& terrain, const Object& parent, vec3 parentConnectionPoint, float scale)
  {
    turretConfig = config;
    this->terrain = &terrain;
    this->parent = &parent;
    this->parentConnectionPoint = parentConnectionPoint;
    this->scale = scale;

    Renderable::init(model);
  }

  void Turret::reset()
  {
    position = vec3::zero;
    rotation = quat::identity;
    yaw = 0;
    pitch = 0;
  }

  void Turret::update(float dt)
  {
    _ASSERT(parent != nullptr);

    //rotation = parent->rotation;// .rotatedByXAngle(pitch).rotatedByYAngle(yaw);
    
    float targetYaw, targetPitch;
    target.rotatedBy(parent->rotation.inverted()).yawPitch(&targetYaw, &targetPitch);

    yaw = moveTo(yaw, targetYaw, turretConfig.rotationSpeed * dt);
    pitch = moveTo(pitch, targetPitch, turretConfig.rotationSpeed * dt);

    yaw = clamp(yaw, turretConfig.minYaw, turretConfig.maxYaw);
    pitch = clamp(pitch, turretConfig.minPitch, turretConfig.maxPitch);

    vec3 globalConnectionPoint = parentConnectionPoint.rotatedBy(parent->rotation);
    position = parent->position + globalConnectionPoint;
    rotation = parent->rotation * quat::identity.rotatedByXAngle(pitch).rotatedByYAngle(yaw);
    rotation.normalize();

    //vec3 currentDir = vec3::forward.rotatedBy(rotation);
    //vec3 axis = currentDir % target;
    //float angle = turretConfig.rotationSpeed * dt;
    //rotation = rotation * quat::fromAxisAngle(axis, angle);
  }

  void Turret::updateTransform()
  {
    transform = MatrixMultiply(MatrixMultiply(QuaternionToMatrix(rotation), MatrixScale(scale, scale, scale)), MatrixTranslate(position.x, position.y, position.z));
  }

  void Turret::drawDebug()
  {
    //drawVector(position, )
  }

}
