#include "core.h"
#include "Renderer.h"
#include "Config.h"
#include "GridWalker.h"

namespace game
{

  Renderer::Renderer(const Config& config, const CustomCamera& camera, const Scene& scene, const Hud& hud) :
    camera(camera),
    config(config),
    scene(scene),
    hud(hud)
  {
  }

  void Renderer::init()
  {
    InitWindow(config.graphics.screen.width, config.graphics.screen.height, config.graphics.screen.title);
    SetTargetFPS(60);
    loadResources();
  }

  void Renderer::shutdown()
  {
    unloadResources();
    CloseWindow();
  }

  void Renderer::inject(std::function<void()> callback)
  {
    injectionCallback = callback;
  }

  void Renderer::draw(float lerpFactor)
  {
    ClearBackground(BLACK);

    BeginDrawing();

    BeginMode3D(camera);

    drawTerrain();

    for (int i = 0; i < scene.cars.capacity(); i++)
      if (scene.cars.isAlive(i))
      {
        const Car& car = scene.cars[i];
        drawCar(car, lerpFactor);
      }

    if (injectionCallback)
      injectionCallback();

    for (int i = 0; i < scene.projectiles.capacity(); i++)
      if (scene.projectiles.isAlive(i))
      {
        const Projectile& projectile = scene.projectiles[i];
        drawProjectile(projectile, lerpFactor);
      }

    for (int i = 0; i < scene.explosionParticles.capacity(); i++)
      if (scene.explosionParticles.isAlive(i))
      {
        const ExplosionParticle& explosionParticle = scene.explosionParticles[i];
        drawExplosionParticle(explosionParticle, lerpFactor);
      }

    EndMode3D();

    hud.draw(camera, scene);

    EndDrawing();
  }

  void Renderer::drawCar(const Car& car, float lerpFactor)
  {
    vec3 position = vec3::lerp(car.lastPosition, car.position, lerpFactor);
    quat rotation = quat::slerp(car.lastRotation, car.rotation, lerpFactor);
    Matrix transform = MatrixMultiply(QuaternionToMatrix(rotation), MatrixTranslate(position.x, position.y, position.z));

    drawModel(carModel, transform);

    drawWheel(car.frontLeftWheel, lerpFactor);
    drawWheel(car.frontRightWheel, lerpFactor);
    drawWheel(car.rearLeftWheel, lerpFactor);
    drawWheel(car.rearRightWheel, lerpFactor);

    drawTurret(car.gun, lerpFactor);
    drawTurret(car.cannon, lerpFactor);

    drawCarDebug(car);
  }

  void Renderer::drawWheel(const Wheel& wheel, float lerpFactor)
  {
    vec3 position = vec3::lerp(wheel.lastPosition, wheel.position, lerpFactor);
    quat rotation = quat::slerp(wheel.lastRotation, wheel.rotation, lerpFactor);
    Matrix transform = MatrixMultiply(QuaternionToMatrix(rotation), MatrixTranslate(position.x, position.y, position.z));

    drawModel(wheelModel, transform);

    drawWheelDebug(wheel);
  }

  void Renderer::drawTurret(const Turret& turret, float lerpFactor)
  {
    vec3 position = vec3::lerp(turret.lastPosition, turret.position, lerpFactor);
    quat rotation = quat::slerp(turret.lastRotation, turret.rotation, lerpFactor);
    float scale = turret.scale;
    Matrix transform = MatrixMultiply(MatrixMultiply(QuaternionToMatrix(rotation), MatrixScale(scale, scale, scale)), MatrixTranslate(position.x, position.y, position.z));

    drawModel(gunModel, transform);
  }

  void Renderer::drawModel(const Model& model, const Matrix& transform)
  {
    for (int i = 0; i < model.meshCount; i++)
    {
      if (!drawWires)
        DrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]], transform);

      rlEnableWireMode();
      DrawMesh(model.meshes[i], wiresMaterial, transform);
      rlDisableWireMode();
    }
  }

  void Renderer::drawProjectile(const Projectile& projectile, float lerpFactor)
  {
    vec3 position = vec3::lerp(projectile.lastPosition, projectile.position, lerpFactor);
    vec3 velocity = vec3::lerp(projectile.lastVelocity, projectile.velocity, lerpFactor);
    float size = projectile.size;

    DrawCapsule(position, position + 0.1f * size * velocity, size, 5, 2, WHITE);
  }

  void Renderer::drawExplosionParticle(const ExplosionParticle& explosionParticle, float lerpFactor)
  {
    vec3 position = vec3::lerp(explosionParticle.lastPosition, explosionParticle.position, lerpFactor);
    quat rotation = quat::slerp(explosionParticle.lastRotation, explosionParticle.rotation, lerpFactor);
    const vec3(&vertices)[3] = explosionParticle.vertices;

    vec3 v0 = position + vertices[0].rotatedBy(rotation);
    vec3 v1 = position + vertices[1].rotatedBy(rotation);
    vec3 v2 = position + vertices[2].rotatedBy(rotation);

    DrawTriangle3D(v0, v1, v2, WHITE);
    DrawTriangle3D(v0, v2, v1, WHITE);
  }

  void Renderer::drawTerrain()
  {
    Matrix transform = MatrixTranslate(-Terrain::TERRAIN_SIZE_2, 0, -Terrain::TERRAIN_SIZE_2);

    drawModel(terrainModel, transform);

    const Terrain::ObjectsPool& terrainObjects = scene.terrain.objects;

    for (int i = 0; i < terrainObjects.capacity(); i++)
      if (terrainObjects.isAlive(i))
        drawTerrainObject(terrainObjects[i]);
  }

  void Renderer::drawTerrainObject(const TerrainObject& terrainObject)
  {
    const Matrix& transform = terrainObject.transform;

    switch (terrainObject.type)
    {
      case TerrainObject::Tree1:
        drawModel(tree1Model, transform);
        break;

      case TerrainObject::Tree2:
        drawModel(tree2Model, transform);
        break;

      case TerrainObject::Rock:
        drawModel(rockModel, transform);
        break;

      default:
        break;
    }
  }

  void Renderer::drawTerrainDebug()
  {
    //for (int i = 0; i < scene.terrain.objectCollisionGeometries.capacity(); i++)
    //  if (scene.terrain.objectCollisionGeometries.isAlive(i))
    //    scene.terrain.objectCollisionGeometries.get(i).drawDebug();

    //std::vector<std::pair<vec3, vec3>> lines =
    //{
    //  {{ -100, 50, -50}, { 50, -50, 100}},
    //};

    //for (auto [start, end] : lines)
    //{
    //  vec3 origin = start;
    //  vec3 direction = end - start;
    //  float distance = direction.length();
    //  vec3 directionNormalized = direction / distance;

    //  visualizeTerrainRayTracing(origin, directionNormalized, distance);
    //}
  }

  void Renderer::drawCarDebug(const Car& car)
  {
    vec3 position = car.position;

    //drawVector(position, 3 * forward(), WHITE);
    //drawVector(position, 3 * left(), LIGHTGRAY);
    //drawVector(position, 3 * up(), DARKGRAY);

    //drawVector(position, 0.001f * suspecsionForce, RED);
    //drawVector(position, 0.5f * moment.logarithmic(), BLUE);
    drawVector(position, 5 * vec3::forward, WHITE);
    drawVector(position, 5 * vec3::left, LIGHTGRAY);

    vec3 hitPosition, normal;
    float distance;
    const Turret& gun = car.gun;
    DrawLine3D(gun.barrelPosition(), gun.barrelPosition() + gun.forward() * 10, BLUE);

    if (scene.terrain.traceRay(gun.barrelPosition(), gun.forward(), 10, &hitPosition, &normal, &distance))
    {
      DrawSphere(hitPosition, 0.1f, YELLOW);
      DrawLine3D(hitPosition, hitPosition + normal * 5, YELLOW);
      DrawSphere(hitPosition + normal * 5, 0.1f, YELLOW);
    }
  }

  void Renderer::drawWheelDebug(const Wheel& wheel)
  {
    if (wheel.isGrounded)
    {
      vec3 bottom = wheel.position + vec3 { 0, -wheel.wheelConfig.radius + 0.2f, 0 };
      //DrawSphere(bottom, 0.3f, ORANGE);

      //drawVector(bottom, velocity, LIME);
      drawVector(bottom, wheel.frictionVelocity, GREEN);
      drawVector(wheel.position, 0.001f * wheel.suspecsionForce, RED);
      drawVector(bottom, 0.001f * wheel.frictionForce, ORANGE);
    }
  }

  void Renderer::loadResources()
  {
    wiresMaterial = LoadMaterialDefault();
    wiresMaterial.maps->color = Color { 255, 255, 255, 64 };

    carModel = LoadModel(config.graphics.resources.carModelPath);
    wheelModel = LoadModel(config.graphics.resources.wheelModelPath);
    gunModel = LoadModel(config.graphics.resources.gunModelPath);
    cannonModel = LoadModel(config.graphics.resources.cannonModelPath);
    tree1Model = LoadModel(config.graphics.resources.tree1ModelPath);
    tree2Model = LoadModel(config.graphics.resources.tree2ModelPath);
    rockModel = LoadModel(config.graphics.resources.rockModelPath);

    terrainTexture = LoadTexture(config.graphics.resources.terrainTexturePath);
    tree1Texture = LoadTexture(config.graphics.resources.tree1TexturePath);
    tree2Texture = LoadTexture(config.graphics.resources.tree2TexturePath);
    rockTexture = LoadTexture(config.graphics.resources.rockTexturePath);

    for(int i=0; i< tree1Model.materialCount; i++)
      tree1Model.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture = tree1Texture;

    for (int i = 0; i < tree2Model.materialCount; i++)
      tree2Model.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture = tree2Texture;
    
    for (int i = 0; i < rockModel.materialCount; i++)
      rockModel.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture = rockTexture;
  }

  void Renderer::unloadResources()
  {
    UnloadMaterial(wiresMaterial);

    UnloadModel(terrainModel);
    UnloadModel(carModel);
    UnloadModel(wheelModel);
    UnloadModel(gunModel);
    UnloadModel(cannonModel);
    UnloadModel(tree1Model);
    UnloadModel(tree2Model);
    UnloadModel(rockModel);

    UnloadTexture(terrainTexture);
    UnloadTexture(tree1Texture);
    UnloadTexture(tree2Texture);
    UnloadTexture(rockTexture);
  }

  void Renderer::updateTerrainModel()
  {
    int square_count = Terrain::GRID_SIZE * Terrain::GRID_SIZE;
    Mesh mesh = {};
    mesh.triangleCount = square_count * 2;
    mesh.vertexCount = mesh.triangleCount * 3;
    mesh.vertices = (float*)malloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)malloc(mesh.vertexCount * 2 * sizeof(float));
    mesh.normals = (float*)malloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.colors = (unsigned char*)malloc(mesh.vertexCount * 4 * sizeof(unsigned char));
    const Terrain::HeightMap(&heightMap) = scene.terrain.heightMap;
    int i = 0;

    for (int y = 0; y < Terrain::GRID_SIZE; y++)
      for (int x = 0; x < Terrain::GRID_SIZE; x++)
      {
        float h00 = heightMap[y * Terrain::HEIGHT_MAP_SIZE + x];
        float h01 = heightMap[(y + 1) * Terrain::HEIGHT_MAP_SIZE + x];
        float h10 = heightMap[y * Terrain::HEIGHT_MAP_SIZE + x + 1];
        float h11 = heightMap[(y + 1) * Terrain::HEIGHT_MAP_SIZE + x + 1];

        float scale = Terrain::TERRAIN_SIZE / Terrain::GRID_SIZE;

        vec3 v00 = { x * scale, h00 * Terrain::TERRAIN_HEIGHT, y * scale };
        vec3 v01 = { x * scale, h01 * Terrain::TERRAIN_HEIGHT, (y + 1) * scale };
        vec3 v10 = { (x + 1) * scale, h10 * Terrain::TERRAIN_HEIGHT, y * scale };
        vec3 v11 = { (x + 1) * scale, h11 * Terrain::TERRAIN_HEIGHT, (y + 1) * scale };

        mesh.vertices[i * 18 + 0] = v00.x;
        mesh.vertices[i * 18 + 1] = v00.y;
        mesh.vertices[i * 18 + 2] = v00.z;
        mesh.vertices[i * 18 + 3] = v01.x;
        mesh.vertices[i * 18 + 4] = v01.y;
        mesh.vertices[i * 18 + 5] = v01.z;
        mesh.vertices[i * 18 + 6] = v11.x;
        mesh.vertices[i * 18 + 7] = v11.y;
        mesh.vertices[i * 18 + 8] = v11.z;
        mesh.vertices[i * 18 + 9] = v00.x;
        mesh.vertices[i * 18 + 10] = v00.y;
        mesh.vertices[i * 18 + 11] = v00.z;
        mesh.vertices[i * 18 + 12] = v11.x;
        mesh.vertices[i * 18 + 13] = v11.y;
        mesh.vertices[i * 18 + 14] = v11.z;
        mesh.vertices[i * 18 + 15] = v10.x;
        mesh.vertices[i * 18 + 16] = v10.y;
        mesh.vertices[i * 18 + 17] = v10.z;

        const float tex_scale = Terrain::TERRAIN_SIZE / 10.0f;
        mesh.texcoords[i * 12 + 0] = (float)x / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 1] = (float)y / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 2] = (float)x / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 3] = (float)(y + 1) / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 4] = (float)(x + 1) / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 5] = (float)(y + 1) / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 6] = (float)x / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 7] = (float)y / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 8] = (float)(x + 1) / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 9] = (float)(y + 1) / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 10] = (float)(x + 1) / Terrain::GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 11] = (float)y / Terrain::GRID_SIZE * tex_scale;

        vec3 n1 = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(v01, v00), Vector3Subtract(v10, v00)));
        vec3 n2 = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(v11, v01), Vector3Subtract(v10, v01)));

        mesh.normals[i * 18 + 0] = n1.x;
        mesh.normals[i * 18 + 1] = n1.y;
        mesh.normals[i * 18 + 2] = n1.z;
        mesh.normals[i * 18 + 3] = n1.x;
        mesh.normals[i * 18 + 4] = n1.y;
        mesh.normals[i * 18 + 5] = n1.z;
        mesh.normals[i * 18 + 6] = n1.x;
        mesh.normals[i * 18 + 7] = n1.y;
        mesh.normals[i * 18 + 8] = n1.z;
        mesh.normals[i * 18 + 9] = n2.x;
        mesh.normals[i * 18 + 10] = n2.y;
        mesh.normals[i * 18 + 11] = n2.z;
        mesh.normals[i * 18 + 12] = n2.x;
        mesh.normals[i * 18 + 13] = n2.y;
        mesh.normals[i * 18 + 14] = n2.z;
        mesh.normals[i * 18 + 15] = n2.x;
        mesh.normals[i * 18 + 16] = n2.y;
        mesh.normals[i * 18 + 17] = n2.z;

        auto calc_color = [](float h) -> unsigned char {
          float min_c = 80;
          float max_c = 255;

          return (unsigned char)(min_c + h * (max_c - min_c));
        };

        mesh.colors[i * 24 + 0] = calc_color(h00);
        mesh.colors[i * 24 + 1] = calc_color(h00);
        mesh.colors[i * 24 + 2] = calc_color(h00);
        mesh.colors[i * 24 + 3] = 255;
        mesh.colors[i * 24 + 4] = calc_color(h01);
        mesh.colors[i * 24 + 5] = calc_color(h01);
        mesh.colors[i * 24 + 6] = calc_color(h01);
        mesh.colors[i * 24 + 7] = 255;
        mesh.colors[i * 24 + 8] = calc_color(h11);
        mesh.colors[i * 24 + 9] = calc_color(h11);
        mesh.colors[i * 24 + 10] = calc_color(h11);
        mesh.colors[i * 24 + 11] = 255;
        mesh.colors[i * 24 + 12] = calc_color(h00);
        mesh.colors[i * 24 + 13] = calc_color(h00);
        mesh.colors[i * 24 + 14] = calc_color(h00);
        mesh.colors[i * 24 + 15] = 255;
        mesh.colors[i * 24 + 16] = calc_color(h11);
        mesh.colors[i * 24 + 17] = calc_color(h11);
        mesh.colors[i * 24 + 18] = calc_color(h11);
        mesh.colors[i * 24 + 19] = 255;
        mesh.colors[i * 24 + 20] = calc_color(h10);
        mesh.colors[i * 24 + 21] = calc_color(h10);
        mesh.colors[i * 24 + 22] = calc_color(h10);
        mesh.colors[i * 24 + 23] = 255;

        i++;
      }

    UploadMesh(&mesh, false);

    UnloadModel(terrainModel);
    terrainModel = LoadModelFromMesh(mesh);
    terrainModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = terrainTexture;
  }

  void Renderer::visualizeTerrainRayTracing(vec3 origin, vec3 directionNormalized, float distance) const
  {
    const Terrain& terrain = scene.terrain;
    vec3 hitPosition1, hitNormal1;
    float hitDistance1;
    vec3 hitPosition2, hitNormal2;
    float hitDistance2;

    bool isHit1 = terrain.traceRayWithObjects(origin, directionNormalized, distance, &hitPosition1, &hitNormal1, &hitDistance1);
    bool isHit2 = terrain.traceRayWithObjects_Unoptimized(origin, directionNormalized, distance, &hitPosition2, &hitNormal2, &hitDistance2);

    vec3 diff = hitPosition1 - hitPosition2;

    if (isHit1)
      DrawSphere(hitPosition1 + vec3::up * 0.1f, 0.2f, GREEN);

    if (isHit2)
      DrawSphere(hitPosition2 - vec3::up * 0.1f, 0.2f, YELLOW);

    DrawLine3D(origin, origin + directionNormalized * distance, RED);
    DrawLine3D(origin, origin + vec3::up * 1000, RED);

    GridWalker gridWalker({ -Terrain::TERRAIN_SIZE_2, -Terrain::TERRAIN_SIZE_2, Terrain::TERRAIN_SIZE_2, Terrain::TERRAIN_SIZE_2 }, Terrain::CG_GRID_CELL_SIZE);

    gridWalker.walkByLine(origin, directionNormalized, distance, [&](int x, int y) {
      float x0 = x * Terrain::CG_GRID_CELL_SIZE - Terrain::TERRAIN_SIZE_2;
      float y0 = y * Terrain::CG_GRID_CELL_SIZE - Terrain::TERRAIN_SIZE_2;
      float x1 = x * Terrain::CG_GRID_CELL_SIZE + Terrain::CG_GRID_CELL_SIZE - Terrain::TERRAIN_SIZE_2;
      float y1 = y * Terrain::CG_GRID_CELL_SIZE + Terrain::CG_GRID_CELL_SIZE - Terrain::TERRAIN_SIZE_2;
      const float step = 1;

      for (float yy = y0 + step / 2; yy <= y1; yy += step)
        for (float xx = x0 + step / 2; xx <= x1; xx += step)
        {
          float h = terrain.getHeight(xx + step / 2, yy + step / 2, nullptr) + 0.5f;
          float k = std::max(fabsf((xx - (x0 + x1) / 2) / (x1 - x0) * 2), fabsf((yy - (y0 + y1) / 2) / (y1 - y0)) * 2);
          bool isBorder = (xx - x0 < 0.75 * step || x1 - xx < 0.75 * step || yy - y0 < 0.75 * step || y1 - yy < 0.75 * step);
          unsigned char transparency = isBorder ? 64 : (unsigned char)(16 + k * k * 16);
          DrawPlane({ xx, h, yy }, { step, step }, { 255, 255, 255, transparency });
        }

      return false;
    });
  }

}
