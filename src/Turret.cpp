#include "pch.h"
#include "Turret.h"
#include "Helpers.h"

namespace game
{
  void Turret::init(const Config::Physics::Turret& config, const Model& model, float scale)
  {
    turretConfig = config;
    this->scale = scale;
    Renderable::init(model);
  }

  void Turret::update(float dt, const Terrain& terrain, const Positionable& parent, vec3 parentConnectionPoint, vec3 target)
  {
    vec3 globalConnectionPoint = parentConnectionPoint.rotatedBy(parent.rotation);

    position = parent.position + globalConnectionPoint;
    rotation = parent.rotation.rotatedByYAngle(yaw);
  }

  void Turret::draw(bool drawWires)
  {
    Matrix transform = MatrixMultiply(MatrixMultiply(QuaternionToMatrix(rotation), MatrixScale(scale, scale, scale)), MatrixTranslate(position.x, position.y, position.z));
    Renderable::draw(transform, drawWires);

    drawDebug();
  }
  void Turret::drawDebug()
  {
    //drawVector(position, )
  }

  void Turret::reset()
  {
  }
}
