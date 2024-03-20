#include "pch.h"
#include "Terrain.h"
#include "Helpers.h"

namespace game
{
  Matrix const Terrain::transform = MatrixTranslate(-TERRAIN_SIZE_2, 0, -TERRAIN_SIZE_2);

  Terrain::Terrain(const Texture& terrainTexture, const Model& tree1Model, const Model& tree2Model, const Model& rockModel) :
    terrainTexture(terrainTexture),
    tree1Model(tree1Model),
    tree2Model(tree2Model),
    rockModel(rockModel)
  {
  }

  Terrain::~Terrain()
  {
    unloadResources();
  }

  void Terrain::unloadResources()
  {
    if (modelLoaded)
      UnloadModel(model);
  }

  void Terrain::init()
  {
    wiresMaterial = LoadMaterialDefault();
    wiresMaterial.maps->color = WHITE;
    generate(Terrain::Mode::Normal);
  }

  float Terrain::getHeight(float worldX, float worldZ, vec3* normal) const
  {
    float x = clamp(worldX + TERRAIN_SIZE_2, 0.0f, TERRAIN_SIZE);
    float y = clamp(worldZ + TERRAIN_SIZE_2, 0.0f, TERRAIN_SIZE);

    x = x / CELL_SIZE;
    y = y / CELL_SIZE;
    int x00 = clamp(int(x), 0, HEIGHT_MAP_SIZE - 2);
    int y00 = clamp(int(y), 0, HEIGHT_MAP_SIZE - 2);
    int x01 = x00;
    int y01 = y00 + 1;
    int x10 = x00 + 1;
    int y10 = y00;
    int x11 = x00 + 1;
    int y11 = y00 + 1;

    float dx = x - (float)x00;
    float dy = y - (float)y00;

    float h00 = heightMap[y00 * HEIGHT_MAP_SIZE + x00];
    float h01 = heightMap[y01 * HEIGHT_MAP_SIZE + x01];
    float h10 = heightMap[y10 * HEIGHT_MAP_SIZE + x10];
    float h11 = heightMap[y11 * HEIGHT_MAP_SIZE + x11];

    // Triangles: (0,0) (1,0) (1,1) and (0,0) (1,1) (0,1) 
    if (normal)
    {
      if (dx > dy)
      {
        vec3 edge1 = { CELL_SIZE , (h11 - h00) * TERRAIN_HEIGHT, CELL_SIZE };
        vec3 edge2 = { CELL_SIZE, (h10 - h00) * TERRAIN_HEIGHT, 0 };
        *normal = (edge1 % edge2).normalized();
      }
      else
      {
        vec3 edge1 = { 0, (h01 - h00) * TERRAIN_HEIGHT, CELL_SIZE };
        vec3 edge2 = { CELL_SIZE, (h11 - h00) * TERRAIN_HEIGHT, CELL_SIZE };
        *normal = (edge1 % edge2).normalized();
      }
    }

    if (dx > dy)
      return TERRAIN_HEIGHT * ((h00 * (1 - dx) + h10 * dx) * (1 - dy / dx) + (h00 * (1 - dx) + h11 * dx) * dy / dx);
    else if (dy > dx)
      return TERRAIN_HEIGHT * ((h00 * (1 - dy) + h11 * dy) * dx / dy + (h00 * (1 - dy) + h01 * dy) * (1 - dx / dy));
    else
      return TERRAIN_HEIGHT * (h00 * (1 - dx) + h11 * dx);
  }

  bool Terrain::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
  {
    vec3 currentHitPosition, currentNormal;
    float currentHitDistance;

    vec3 closestHitPosition, closestNormal;
    float closestHitDistance = FLT_MAX;

    bool hitTerrain = traceRayWithTerrain(origin, directionNormalized, distance, &currentHitPosition, &currentNormal, &currentHitDistance);

    if (hitTerrain)
    {
      closestHitPosition = currentHitPosition;
      closestNormal = currentNormal;
      closestHitDistance = currentHitDistance;
    }

    int ii = CG_GRID_SIZE;
    // TODO: implement optimized solution by grouping geometries
    for (int i = 0; i < objectCollisionGeometries.count(); i++)
      if (objectCollisionGeometries.isAlive(i))
      {
        const CollisionGeometry& geometry = objectCollisionGeometries.get(i);

        bool hitObject = geometry.traceRay(origin, directionNormalized, distance, &currentHitPosition, &currentNormal, &currentHitDistance);
        if (hitObject && currentHitDistance < closestHitDistance)
        {
          closestHitPosition = currentHitPosition;
          closestNormal = currentNormal;
          closestHitDistance = currentHitDistance;
        }
      }

    if (closestHitDistance < FLT_MAX)
    {
      if (hitPosition)
        *hitPosition = closestHitPosition;

      if (hitNormal)
        *hitNormal = closestNormal;

      if (hitDistance)
        *hitDistance = closestHitDistance;

      return true;
    }

    return false;
  }

  bool Terrain::traceRayWithTerrain(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
  {
    distance = (distance < 0) ? FLT_MAX : distance;

    if (directionNormalized.isZero() || distance < EPSILON)
      return false;

    float maxAxis = std::max(fabsf(directionNormalized.x), fabsf(directionNormalized.z));

    if (maxAxis < EPSILON)
      return getTrianglePair(
        int((origin.x + TERRAIN_SIZE_2) / CELL_SIZE),
        int((origin.z + TERRAIN_SIZE_2) / CELL_SIZE)
      ).traceRay(origin, directionNormalized, distance, hitPosition, hitNormal, hitDistance);

    vec2 direction2D = directionNormalized.xz();
    vec2 step = direction2D / maxAxis * 0.999f;
    float stepLength = step.length();
    float sqDistance = distance * distance;
    vec2 origin2D = origin.xz();

    float distance2D = (1 - fabsf(directionNormalized.y) < EPSILON) ?
      EPSILON :
      distance * sqrtf(1 - directionNormalized.y * directionNormalized.y);

    vec2 current = (origin2D + vec2 { TERRAIN_SIZE_2, TERRAIN_SIZE_2 }) / CELL_SIZE;
    distance2D += CELL_SIZE * float(M_SQRT2);

    int x = (int)current.x;
    int y = (int)current.y;

    while (x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE && distance2D > 0)
    {
      if (getTrianglePair(x, y).traceRay(origin, directionNormalized, distance, hitPosition, hitNormal, hitDistance))
        return true;

      vec2 next = current + step;
      int nx = (int)next.x;
      int ny = (int)next.y;

      // diagonal step
      if (nx != x && ny != y)
      {
        vec2 gridPoint = vec2 { float(std::min(x, nx) + 1), float(std::min(y, ny) + 1) };
        vec2 toGridPoint = gridPoint - current;
        vec2 right = vec2 { toGridPoint.y, -toGridPoint.x };
        float k = direction2D * right * sign(step.x) * sign(step.y);

        if (k > 0 && nx >= 0 && nx < GRID_SIZE)
        {
          if (getTrianglePair(nx, y).traceRay(origin, directionNormalized, distance, hitPosition, hitNormal, hitDistance))
            return true;
        }
        else if (k < 0 && ny >= 0 && ny < GRID_SIZE)
        {
          if (getTrianglePair(x, ny).traceRay(origin, directionNormalized, distance, hitPosition, hitNormal, hitDistance))
            return true;
        }
      }

      x = nx;
      y = ny;
      current = next;
      distance2D -= stepLength;
    }

    return false;
  }

  Terrain::Triangle Terrain::getTriangle(float worldX, float worldZ) const
  {
    float x = clamp(worldX + TERRAIN_SIZE_2, 0.0f, TERRAIN_SIZE);
    float y = clamp(worldZ + TERRAIN_SIZE_2, 0.0f, TERRAIN_SIZE);

    x = x / CELL_SIZE;
    y = y / CELL_SIZE;
    int x00 = clamp(int(x), 0, HEIGHT_MAP_SIZE - 2);
    int y00 = clamp(int(y), 0, HEIGHT_MAP_SIZE - 2);
    int x01 = x00;
    int y01 = y00 + 1;
    int x10 = x00 + 1;
    int y10 = y00;
    int x11 = x00 + 1;
    int y11 = y00 + 1;

    float dx = x - (float)x00;
    float dy = y - (float)y00;

    float h00 = heightMap[y00 * HEIGHT_MAP_SIZE + x00];
    float h01 = heightMap[y01 * HEIGHT_MAP_SIZE + x01];
    float h10 = heightMap[y10 * HEIGHT_MAP_SIZE + x10];
    float h11 = heightMap[y11 * HEIGHT_MAP_SIZE + x11];

    float h;

    // Triangles: (0,0) (1,0) (1,1) and (0,0) (0,1) (1,1)
    if (dx > dy)
    {
      h = (h00 * (1 - dx) + h10 * dx) * (1 - dy / dx) + (h00 * (1 - dx) + h11 * dx) * dy / dx;

      return {
        { x00 * CELL_SIZE - TERRAIN_SIZE_2, h00 * TERRAIN_HEIGHT, y00 * CELL_SIZE - TERRAIN_SIZE_2 },
        { x11 * CELL_SIZE - TERRAIN_SIZE_2, h11 * TERRAIN_HEIGHT, y11 * CELL_SIZE - TERRAIN_SIZE_2 },
        { x10 * CELL_SIZE - TERRAIN_SIZE_2, h10 * TERRAIN_HEIGHT, y10 * CELL_SIZE - TERRAIN_SIZE_2 },
      };
    }
    else
    {
      h = (h00 * (1 - dy) + h01 * dy) * (1 - dx / dy) + (h00 * (1 - dy) + h11 * dy) * dx / dy;

      return {
        { x00 * CELL_SIZE - TERRAIN_SIZE_2, h00 * TERRAIN_HEIGHT, y00 * CELL_SIZE - TERRAIN_SIZE_2 },
        { x01 * CELL_SIZE - TERRAIN_SIZE_2, h01 * TERRAIN_HEIGHT, y01 * CELL_SIZE - TERRAIN_SIZE_2 },
        { x11 * CELL_SIZE - TERRAIN_SIZE_2, h11 * TERRAIN_HEIGHT, y11 * CELL_SIZE - TERRAIN_SIZE_2 },
      };
    }
  }

  Terrain::TrianglePair Terrain::getTrianglePair(int x, int y) const
  {
    x = clamp(x, 0, GRID_SIZE - 1);
    y = clamp(y, 0, GRID_SIZE - 1);

    float startX = x * CELL_SIZE - TERRAIN_SIZE_2;
    float startY = y * CELL_SIZE - TERRAIN_SIZE_2;

    float h00 = heightMap[y * HEIGHT_MAP_SIZE + x] * TERRAIN_HEIGHT;
    float h01 = heightMap[(y + 1) * HEIGHT_MAP_SIZE + x] * TERRAIN_HEIGHT;
    float h10 = heightMap[y * HEIGHT_MAP_SIZE + x + 1] * TERRAIN_HEIGHT;
    float h11 = heightMap[(y + 1) * HEIGHT_MAP_SIZE + x + 1] * TERRAIN_HEIGHT;

    return {
      { startX, h00, startY },
      { startX, h01, startY + CELL_SIZE },
      { startX + CELL_SIZE, h10, startY },
      { startX + CELL_SIZE, h11, startY + CELL_SIZE },
    };
  }

  void Terrain::draw(bool drawWires)
  {
    for (int i = 0; i < model.meshCount; i++)
    {
      if (!drawWires)
        DrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]], transform);

      rlEnableWireMode();
      DrawMesh(model.meshes[i], wiresMaterial, transform);
      rlDisableWireMode();
    }

    for (int i = 0; i < objects.capacity(); i++)
      if (objects.isAlive(i))
      {
        TerrainObject& obj = objects.get(i);
        obj.draw(drawWires);
      }

    //drawDebug();
  }

  void Terrain::drawDebug() const
  {
    for (int i = 0; i < objectCollisionGeometries.count(); i++)
      if (objectCollisionGeometries.isAlive(i))
        objectCollisionGeometries.get(i).drawDebug();
  }

  void Terrain::generate(Mode mode)
  {
    unloadResources();
    heightMap.resize(HEIGHT_MAP_SIZE * HEIGHT_MAP_SIZE);

    for (int y = 0; y < HEIGHT_MAP_SIZE; y++)
      for (int x = 0; x < HEIGHT_MAP_SIZE; x++)
        heightMap[y * HEIGHT_MAP_SIZE + x] = calcHeight(x, y, mode);

    float min_h = FLT_MAX;
    float max_h = -FLT_MAX;

    for (float h : heightMap)
    {
      min_h = std::min(h, min_h);
      max_h = std::max(h, max_h);
    }

    if (max_h > min_h)
      for (float& h : heightMap)
        h = (h - min_h) / (max_h - min_h);

#ifndef UNIT_TEST

    int square_count = GRID_SIZE * GRID_SIZE;
    mesh = {};
    mesh.triangleCount = square_count * 2;
    mesh.vertexCount = mesh.triangleCount * 3;
    mesh.vertices = (float*)malloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)malloc(mesh.vertexCount * 2 * sizeof(float));
    mesh.normals = (float*)malloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.colors = (unsigned char*)malloc(mesh.vertexCount * 4 * sizeof(unsigned char));
    int i = 0;

    for (int y = 0; y < GRID_SIZE; y++)
      for (int x = 0; x < GRID_SIZE; x++)
      {
        float h00 = heightMap[y * HEIGHT_MAP_SIZE + x];
        float h01 = heightMap[(y + 1) * HEIGHT_MAP_SIZE + x];
        float h10 = heightMap[y * HEIGHT_MAP_SIZE + x + 1];
        float h11 = heightMap[(y + 1) * HEIGHT_MAP_SIZE + x + 1];

        float scale = TERRAIN_SIZE / GRID_SIZE;

        vec3 v00 = { x * scale, h00 * TERRAIN_HEIGHT, y * scale };
        vec3 v01 = { x * scale, h01 * TERRAIN_HEIGHT, (y + 1) * scale };
        vec3 v10 = { (x + 1) * scale, h10 * TERRAIN_HEIGHT, y * scale };
        vec3 v11 = { (x + 1) * scale, h11 * TERRAIN_HEIGHT, (y + 1) * scale };

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

        const float tex_scale = TERRAIN_SIZE / 10.0f;
        mesh.texcoords[i * 12 + 0] = (float)x / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 1] = (float)y / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 2] = (float)x / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 3] = (float)(y + 1) / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 4] = (float)(x + 1) / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 5] = (float)(y + 1) / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 6] = (float)x / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 7] = (float)y / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 8] = (float)(x + 1) / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 9] = (float)(y + 1) / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 10] = (float)(x + 1) / GRID_SIZE * tex_scale;
        mesh.texcoords[i * 12 + 11] = (float)y / GRID_SIZE * tex_scale;

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

    model = LoadModelFromMesh(mesh);
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = terrainTexture;
    modelLoaded = true;

    generateObjects();

#endif // UNIT_TEST
  }

  void Terrain::generateObjects()
  {
    objects.clear();
    objectCollisionGeometries.clear();

    struct
    {
      float x = 0;
      float z = 0;
      float radius = 0;

      inline bool collidingWith(float x, float z, float radius)
      {
        return sqr(x - this->x) + sqr(z - this->z) < sqr(radius + this->radius);
      }
    } objs[OBJECT_COUNT];

    for (int i = 0; i < OBJECT_COUNT; i++)
    {
      std::function<CollisionGeometry (vec3, float, float)> createCollisionGeometry;
      const Model* model = nullptr;
      float scale = 0;
      float radius = 0;
      float dy = 0;
      float x = 0;
      float z = 0;
      const int MAX_ATTEMPTS = 100;
      int attempts = MAX_ATTEMPTS;

      while (--attempts >= 0)
      {
        if (attempts == MAX_ATTEMPTS - 1 || attempts < MAX_ATTEMPTS / 2)
        {
          int type = randi(100);

          if (type > 80)
          {
            createCollisionGeometry = &createRockCollisionGeometry;
            model = &rockModel;
            scale = randf(1, 5);
            dy = TERRAIN_HEIGHT / TERRAIN_SIZE * scale * 10;
          }
          else if (type > 30)
          {
            createCollisionGeometry = &createTree1CollisionGeometry;
            model = &tree1Model;
            scale = randf(0.25f, 0.75f);
            dy = TERRAIN_HEIGHT / TERRAIN_SIZE * scale * 5;
          }
          else
          {
            createCollisionGeometry = &createTree2CollisionGeometry;
            model = &tree2Model;
            scale = randf(0.15f, 0.5f);
            dy = TERRAIN_HEIGHT / TERRAIN_SIZE * scale * 5;
          };

          BoundingBox box = GetModelBoundingBox(*model);
          radius = std::max(box.max.x - box.min.x, box.max.z - box.min.z) * scale * 0.5f;
        }

        x = randf(-TERRAIN_SIZE_2 + radius, TERRAIN_SIZE_2 - radius);
        z = randf(-TERRAIN_SIZE_2 + radius, TERRAIN_SIZE_2 - radius);

        int j = i;

        while (--j >= 0)
          if (objs[j].collidingWith(x, z, radius))
            break;

        if (j < 0)
        {
          objs[i] = { x, z, radius };
          break;
        }
      }

      if (attempts < 0)
        break;

      float y = getHeight(x, z) - dy;
      vec3 position = { x, y, z };
      float angle = randf(2 * PI);
      objects.tryAdd(*model, position, angle, scale);
      objectCollisionGeometries.tryAdd(createCollisionGeometry(position, angle, scale));
    }
  }

  CollisionGeometry Terrain::createTree1CollisionGeometry(vec3 position, float angle, float scale)
  {
    static std::pair<vec3, float> spheres[] = {
      {{ 0.000f, -0.330f,  0.000f}, 1.990f},
      {{ 0.000f,  2.145f,  0.165f}, 1.330f},
      {{-0.000f,  3.630f,  0.495f}, 1.330f},
      {{ 0.000f,  5.610f,  0.660f}, 1.495f},
      {{-0.000f,  8.250f,  0.990f}, 1.660f},
      {{-3.036f,  7.831f, -1.584f}, 1.957f},
      {{-4.356f, 10.693f, -0.066f}, 2.848f},
      {{ 3.498f, 11.383f, -1.386f}, 2.584f},
      {{ 2.673f, 12.637f,  5.016f}, 3.838f},
      {{-0.677f, 13.937f,  1.132f}, 5.452f}
    };

    quat rotation = quat::fromYAngle(angle);
    CollisionGeometry cg;

    for (auto& p : spheres)
      cg.add(position + p.first.rotatedBy(rotation) * scale, p.second * scale);

    return cg;
  }

  CollisionGeometry Terrain::createTree2CollisionGeometry(vec3 position, float angle, float scale)
  {
    static std::pair<vec3, float> spheres[] = {
      {{ 0.000f, -1.650f,  0.000f}, 3.640f},
      {{-0.165f,  2.310f, -0.165f}, 1.330f},
      {{-0.000f,  3.960f, -0.330f}, 1.330f},
      {{ 0.000f,  5.940f, -0.495f}, 1.495f},
      {{-1.150f,  9.900f,  0.495f}, 4.465f},
      {{-0.396f, 14.431f,  0.231f}, 5.092f},
      {{ 3.894f, 12.178f,  0.759f}, 3.673f},
      {{-0.132f, 16.993f, -4.851f}, 1.924f},
      {{-2.772f, 19.237f,  1.056f}, 2.188f},
      {{-3.812f, 21.856f,  2.617f}, 1.162f}
    };

    quat rotation = quat::fromYAngle(angle);
    CollisionGeometry cg;

    for (auto& p : spheres)
      cg.add(position + p.first.rotatedBy(rotation) * scale, p.second * scale);

    return cg;
  }

  CollisionGeometry Terrain::createRockCollisionGeometry(vec3 position, float angle, float scale)
  {
    static std::pair<vec3, float> spheres[] = {
      {{-0.825f, -0.115f, -3.465f}, 1.825f},
      {{ 0.000f,  1.040f, -2.310f}, 1.330f},
      {{-1.155f, -0.115f, -1.320f}, 1.660f},
      {{-0.990f,  0.380f,  2.310f}, 1.165f},
      {{-0.495f,  1.205f,  1.320f}, 1.495f},
      {{ 0.330f,  1.535f, -0.990f}, 1.990f},
      {{ 0.825f,  1.205f,  0.990f}, 1.165f},
      {{ 0.495f,  2.525f,  0.825f}, 1.660f},
    };

    quat rotation = quat::fromYAngle(angle);
    CollisionGeometry cg;

    for (auto& p : spheres)
      cg.add(position + p.first.rotatedBy(rotation) * scale, p.second * scale);

    return cg;
  }

  float Terrain::calcHeight(int x, int y, Mode mode) const
  {
    if (x == 0 || x == GRID_SIZE || y == 0 || y == GRID_SIZE)
      return 1.0f;

    if (mode == Debug1)
      return
      y == HEIGHT_MAP_SIZE - 2 ? 0.3f :
      y == HEIGHT_MAP_SIZE - 3 ? 0.0f :
      y == HEIGHT_MAP_SIZE - 4 ? 0.1f :
      y == HEIGHT_MAP_SIZE - 5 ? 0.0f :
      0.1f;
    else if (mode == Debug2)
      return abs(y - HEIGHT_MAP_SIZE_2) < HEIGHT_MAP_SIZE_4 ?
      1 - float(abs(y - HEIGHT_MAP_SIZE_2)) / HEIGHT_MAP_SIZE_4 :
      (y - HEIGHT_MAP_SIZE_2) > HEIGHT_MAP_SIZE_4 ?
      float(y - HEIGHT_MAP_SIZE_2 - HEIGHT_MAP_SIZE_4) / HEIGHT_MAP_SIZE_4 :
      0.0f;

    float xf = float(x) / HEIGHT_MAP_SIZE;
    float yf = float(y + HEIGHT_MAP_SIZE_2) / HEIGHT_MAP_SIZE;

    int sz = HEIGHT_MAP_SIZE;
    float ka = 1.0f;
    float k = 1.0f;
    float h = 0.0f;
    float add = 0.0f;
    const int parts = 3;
    const float kn = 0.5f / parts;

    while (sz)
    {
      h +=
        kn * k * sinf(2 * float(M_PI) * xf * yf * ka + add) +
        kn * k * sinf(2 * float(M_PI) * xf * ka + add) +
        kn * k * sinf(2 * float(M_PI) * yf * ka + add);
      sz /= 2;
      ka *= 2.0f;
      k /= 2.0f;
      add += 0.123f;
    }

    h += 0.5f;

    return h;
  }

  bool Terrain::Triangle::traceRay(vec3 origin, vec3 directionNormalized, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
  {
    vec3 edge01 = v1 - v0;
    vec3 edge12 = v2 - v1;
    vec3 edge20 = v0 - v2;
    vec3 norm = (edge20 % edge01).normalized();
    float originToPlaneDistanceSigned = (v0 - origin) * norm;
    float cosAngle = directionNormalized * norm;

    if (fabsf(cosAngle) < EPSILON)
      return false;

    float originToHitDistance = (originToPlaneDistanceSigned / cosAngle);

    if (originToHitDistance < 0)
      return false;

    vec3 hit = origin + directionNormalized * originToHitDistance;

    vec3 v0toHit = hit - v0;
    vec3 v1toHit = hit - v1;
    vec3 v2toHit = hit - v2;

    if ((v0toHit % edge01) / Terrain::CELL_SIZE / Terrain::CELL_SIZE * norm > 0.001f)
      return false;

    if ((v1toHit % edge12) / Terrain::CELL_SIZE / Terrain::CELL_SIZE * norm > 0.001f)
      return false;

    if ((v2toHit % edge20) / Terrain::CELL_SIZE / Terrain::CELL_SIZE * norm > 0.001f)
      return false;

    if (hitPosition)
      *hitPosition = hit;

    if (hitNormal)
      *hitNormal = norm;

    if (hitDistance)
      *hitDistance = originToHitDistance;

    return true;
  }

  bool Terrain::TrianglePair::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
  {
    vec3 hitPosition1, normal1, hitPosition2, normal2;
    float hitDistance1 = FLT_MAX, hitDistance2 = FLT_MAX;

    Triangle tr1 = { v00, v10, v11 };
    Triangle tr2 = { v00, v11, v01 };

    bool isHit1 = tr1.traceRay(origin, directionNormalized, &hitPosition1, &normal1, &hitDistance1);
    bool isHit2 = tr2.traceRay(origin, directionNormalized, &hitPosition2, &normal2, &hitDistance2);

    if (isHit1 && hitDistance1 <= distance && hitDistance1 < hitDistance2)
    {
      if (hitPosition)
        *hitPosition = hitPosition1;

      if (hitNormal)
        *hitNormal = normal1;

      if (hitDistance)
        *hitDistance = hitDistance1;

      return true;
    }
    else if (isHit2 && hitDistance2 <= distance)
    {
      if (hitPosition)
        *hitPosition = hitPosition2;

      if (hitNormal)
        *hitNormal = normal2;

      if (hitDistance)
        *hitDistance = hitDistance2;

      return true;
    }

    return false;
  }

}