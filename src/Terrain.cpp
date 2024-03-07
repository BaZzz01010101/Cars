#include "pch.h"
#include "Terrain.h"
#include "Helpers.h"

namespace game
{
  Terrain::~Terrain()
  {
    unloadResources();
  }

  void Terrain::unloadResources()
  {
    if (modelLoaded)
      UnloadModel(model);

    if (textureLoaded)
      UnloadTexture(texture);
  }

  float Terrain::getHeight(float x, float y) const
  {
    x = clamp(x + TERRAIN_SIZE / 2, 0.0f, TERRAIN_SIZE);
    y = clamp(y + TERRAIN_SIZE / 2, 0.0f, TERRAIN_SIZE);

    x = x / TERRAIN_SIZE * (HEIGHT_MAP_SIZE - 1);
    y = y / TERRAIN_SIZE * (HEIGHT_MAP_SIZE - 1);
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

    float h00 = heightMap2[y00 * HEIGHT_MAP_SIZE + x00];
    float h01 = heightMap2[y01 * HEIGHT_MAP_SIZE + x01];
    float h10 = heightMap2[y10 * HEIGHT_MAP_SIZE + x10];
    float h11 = heightMap2[y11 * HEIGHT_MAP_SIZE + x11];

    float h;

    // Triangles: (0,0) (1,0) (1,1) and (0,0) (0,1) (1,1)
    if (dx > dy)
    {
      h = (h00 * (1 - dx) + h10 * dx) * (1 - dy / dx) + (h00 * (1 - dx) + h11 * dx) * dy / dx;
    }
    else if (dy > dx)
    {
      h = (h00 * (1 - dy) + h01 * dy) * (1 - dx / dy) + (h00 * (1 - dy) + h11 * dy) * dx / dy;
    }
    else
    {
      h = h00 * (1 - dx) + h11 * dx;
    }

    return h * TERRAIN_HEIGHT;
  }

  float Terrain::getHeight(float worldX, float worldZ, vec3* normal) const
  {
    float x = clamp(worldX + TERRAIN_SIZE / 2, 0.0f, TERRAIN_SIZE);
    float y = clamp(worldZ + TERRAIN_SIZE / 2, 0.0f, TERRAIN_SIZE);

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

    float h00 = heightMap2[y00 * HEIGHT_MAP_SIZE + x00];
    float h01 = heightMap2[y01 * HEIGHT_MAP_SIZE + x01];
    float h10 = heightMap2[y10 * HEIGHT_MAP_SIZE + x10];
    float h11 = heightMap2[y11 * HEIGHT_MAP_SIZE + x11];

    float h;

    // Triangles: (0,0) (1,0) (1,1) and (0,0) (1,1) (0,1) 
    if (dx > dy)
    {
      h = (h00 * (1 - dx) + h10 * dx) * (1 - dy / dx) + (h00 * (1 - dx) + h11 * dx) * dy / dx;

      if (normal)
        *normal = Vector3Normalize(Vector3CrossProduct({ CELL_SIZE , (h11 - h00) * TERRAIN_HEIGHT, CELL_SIZE }, { CELL_SIZE, (h10 - h00) * TERRAIN_HEIGHT, 0 }));
    }
    else if (dy > dx)
    {
      h = (h00 * (1 - dy) + h11 * dy) * dx / dy + (h00 * (1 - dy) + h01 * dy) * (1 - dx / dy);

      if (normal)
        *normal = Vector3Normalize(Vector3CrossProduct({ 0, (h01 - h00) * TERRAIN_HEIGHT, CELL_SIZE }, { CELL_SIZE, (h11 - h00) * TERRAIN_HEIGHT, CELL_SIZE }));
    }
    else
    {
      h = h00 * (1 - dx) + h11 * dx;

      if (normal)
        *normal = Vector3Normalize(Vector3CrossProduct({ 0, (h01 - h00) * TERRAIN_HEIGHT, CELL_SIZE }, { CELL_SIZE, (h11 - h00) * TERRAIN_HEIGHT, CELL_SIZE }));
    }

    return h * TERRAIN_HEIGHT;
  }

  bool Terrain::trace(vec3 start, vec3 end, vec3* hit, vec3* normal) const
  {
    if (start == end)
      return false;

    vec3 ray = Vector3Subtract(end, start);
    float rayLength = Vector3Length(ray);
    float stepLength = 0.1f;
    const int steps = clamp(int(rayLength / stepLength), 1, 10);
    vec3 step = ray / (float)steps;

    for (int i = 0; i < steps; i++)
    {
      float h = getHeight(start.x, start.z, nullptr);

      if (start.y < h)
      {
        float h = getHeight(start.x, start.z, normal);

        return true;
      }

      start += step;
    }

    return false;
  }

  bool Terrain::intersectRayTriangle(const vec3 origin, const vec3 direction, const vec3 v0, const vec3 v1, const vec3 v2, vec3* collision, vec3* normal) const
  {
    vec3 edge01 = v1 - v0;
    vec3 edge12 = v2 - v1;
    vec3 edge20 = v0 - v2;
    vec3 norm = (edge01 % edge20).normalized();
    float originToPlaneDistanceSigned = (v0 - origin) * norm;
    float cosAngle = direction * norm;

    if (fabsf(cosAngle) < EPSILON)
      return false;

    float originToHitDistance = (originToPlaneDistanceSigned / cosAngle);

    if (originToHitDistance < 0)
      return false;

    vec3 hit = origin + direction * originToHitDistance;

    vec3 v0toHit = hit - v0;
    vec3 v1toHit = hit - v1;
    vec3 v2toHit = hit - v2;

    if ((edge01 % v0toHit) / Terrain::CELL_SIZE / Terrain::CELL_SIZE * norm > 0.001f)
      return false;

    if ((edge12 % v1toHit) / Terrain::CELL_SIZE / Terrain::CELL_SIZE * norm > 0.001f)
      return false;

    if ((edge20 % v2toHit) / Terrain::CELL_SIZE / Terrain::CELL_SIZE * norm > 0.001f)
      return false;

    if (collision)
      *collision = hit;

    if (normal)
      *normal = norm;

    return true;
  }

  std::vector<Terrain::Cell> Terrain::traceGrid2D(vec2 origin, vec2 direction, float distance) const
  {
    static const float HALF_TERRAIN_SIZE = TERRAIN_SIZE / 2;

    if (fabsf(direction.x) < EPSILON && fabsf(direction.y) < EPSILON)
    {
      return { {
          int((origin.x + HALF_TERRAIN_SIZE) / CELL_SIZE),
          int((origin.y + HALF_TERRAIN_SIZE) / CELL_SIZE)
        } };
    }

    std::vector<Cell> cells;
    constexpr int MAX_CROSSED_CELLS = int(2 * M_SQRT2 * HEIGHT_MAP_SIZE);
    cells.reserve(MAX_CROSSED_CELLS);

    vec2 current = (origin + vec2{ HALF_TERRAIN_SIZE, HALF_TERRAIN_SIZE }) / CELL_SIZE;
    // division by null is checked above
    vec2 step = direction / std::max(fabsf(direction.x), fabsf(direction.y)) * 0.999f;
    float stepLength = step.length();
    distance += CELL_SIZE;

    int x = (int)current.x;
    int y = (int)current.y;

    while (x >= 0 && x < HEIGHT_MAP_SIZE - 1 && y >= 0 && y < HEIGHT_MAP_SIZE - 1 && distance > 0)
    {
      cells.push_back({ x, y });

      vec2 next = current + step;
      int nx = (int)next.x;
      int ny = (int)next.y;

      // diagonal step
      if (nx != x && ny != y)
      {
        vec2 gridPoint = vec2{ float(std::min(x, nx) + 1), float(std::min(y, ny) + 1) };
        vec2 toGridPoint = gridPoint - current;
        vec2 right = vec2{ toGridPoint.y, -toGridPoint.x };
        float k = direction * right * sign(step.x) * sign(step.y);

        if (k > 0 && nx >= 0 && nx < HEIGHT_MAP_SIZE - 1)
          cells.push_back({ nx, y });
        else if (k < 0 && ny >= 0 && ny < HEIGHT_MAP_SIZE - 1)
          cells.push_back({ x, ny });
      }

      x = nx;
      y = ny;
      current = next;
      distance -= stepLength;
    }

    return cells;
  }

  bool Terrain::traceRay(vec3 origin, vec3 directionNormalized, float distance, vec3* collision, vec3* normal) const
  {
    distance = (distance < 0) ? 9e9f : distance;

    if (directionNormalized.isZero() || distance < EPSILON)
      return false;

    float distance2D = (1 - fabsf(directionNormalized.y) < EPSILON) ? EPSILON : distance * sqrtf(1 - directionNormalized.y * directionNormalized.y);
    auto cells = traceGrid2D(origin.xz(), directionNormalized.xz(), distance2D);
    float sqDistance = distance * distance;
    vec3 collision1, normal1, collision2, normal2;

    for (const Cell cell : cells)
    {
      vec3 v00, v01, v10, v11;
      getTrianglePair(cell.x, cell.y, &v00, &v01, &v10, &v11);

      bool isHit1 = intersectRayTriangle(origin, directionNormalized, v00, v10, v11, &collision1, &normal1);
      bool isHit2 = intersectRayTriangle(origin, directionNormalized, v00, v11, v01, &collision2, &normal2);

      float sqDistance1 = isHit1 ? (collision1 - origin).sqLength() : (10 * sqDistance);
      float sqDistance2 = isHit2 ? (collision2 - origin).sqLength() : (10 * sqDistance);

      if (sqDistance1 <= sqDistance && sqDistance1 < sqDistance2)
      {
        if (collision)
          *collision = collision1;

        if (normal)
          *normal = normal1;

        return true;
      }
      else if(sqDistance2 <= sqDistance)
      {
        if (collision)
          *collision = collision2;

        if (normal)
          *normal = normal2;

        return true;
      }
    }

    return false;
  }

  void Terrain::getTriangle(float worldX, float worldY, vec3* v1, vec3* v2, vec3* v3) const
  {
    float x = clamp(worldX + TERRAIN_SIZE / 2, 0.0f, TERRAIN_SIZE);
    float y = clamp(worldY + TERRAIN_SIZE / 2, 0.0f, TERRAIN_SIZE);

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

    float h00 = heightMap2[y00 * HEIGHT_MAP_SIZE + x00];
    float h01 = heightMap2[y01 * HEIGHT_MAP_SIZE + x01];
    float h10 = heightMap2[y10 * HEIGHT_MAP_SIZE + x10];
    float h11 = heightMap2[y11 * HEIGHT_MAP_SIZE + x11];

    float h;

    // Triangles: (0,0) (1,0) (1,1) and (0,0) (0,1) (1,1)
    if (dx > dy)
    {
      h = (h00 * (1 - dx) + h10 * dx) * (1 - dy / dx) + (h00 * (1 - dx) + h11 * dx) * dy / dx;

      if (v1)
        *v1 = { x00 * CELL_SIZE - TERRAIN_SIZE / 2, h00 * TERRAIN_HEIGHT, y00 * CELL_SIZE - TERRAIN_SIZE / 2 };

      if (v2)
        *v2 = { x11 * CELL_SIZE - TERRAIN_SIZE / 2, h11 * TERRAIN_HEIGHT, y11 * CELL_SIZE - TERRAIN_SIZE / 2 };

      if (v3)
        *v3 = { x10 * CELL_SIZE - TERRAIN_SIZE / 2, h10 * TERRAIN_HEIGHT, y10 * CELL_SIZE - TERRAIN_SIZE / 2 };
    }
    else
    {
      h = (h00 * (1 - dy) + h01 * dy) * (1 - dx / dy) + (h00 * (1 - dy) + h11 * dy) * dx / dy;

      if (v1)
        *v1 = { x00 * CELL_SIZE - TERRAIN_SIZE / 2, h00 * TERRAIN_HEIGHT, y00 * CELL_SIZE - TERRAIN_SIZE / 2 };

      if (v2)
        *v2 = { x01 * CELL_SIZE - TERRAIN_SIZE / 2, h01 * TERRAIN_HEIGHT, y01 * CELL_SIZE - TERRAIN_SIZE / 2 };

      if (v3)
        *v3 = { x11 * CELL_SIZE - TERRAIN_SIZE / 2, h11 * TERRAIN_HEIGHT, y11 * CELL_SIZE - TERRAIN_SIZE / 2 };
    }
  }

  void Terrain::getTrianglePair(int x, int y, vec3* v00, vec3* v01, vec3* v10, vec3* v11) const
  {
    float startX = x * CELL_SIZE - TERRAIN_SIZE / 2;
    float startY = y * CELL_SIZE - TERRAIN_SIZE / 2;

    float h00 = heightMap2[y * HEIGHT_MAP_SIZE + x] * TERRAIN_HEIGHT;
    float h01 = heightMap2[(y + 1) * HEIGHT_MAP_SIZE + x] * TERRAIN_HEIGHT;
    float h10 = heightMap2[y * HEIGHT_MAP_SIZE + x + 1] * TERRAIN_HEIGHT;
    float h11 = heightMap2[(y + 1) * HEIGHT_MAP_SIZE + x + 1] * TERRAIN_HEIGHT;

    *v00 = { startX, h00, startY };
    *v01 = { startX, h01, startY + CELL_SIZE };
    *v10 = { startX + CELL_SIZE, h10, startY };
    *v11 = { startX + CELL_SIZE, h11, startY + CELL_SIZE };
  }

  void Terrain::draw(bool drawWires)
  {
    if (drawWires)
      DrawModelWires(model, { -TERRAIN_SIZE / 2, 0, -TERRAIN_SIZE / 2 }, 1, WHITE);
    else
    {
      DrawModel(model, { -TERRAIN_SIZE / 2, 0, -TERRAIN_SIZE / 2 }, 1, WHITE);
      DrawModelWires(model, { -TERRAIN_SIZE / 2, 0.05f, -TERRAIN_SIZE / 2 }, 1, BLACK);
    }
  }

  void Terrain::generate(const char* texturePath, Mode mode)
  {
    unloadResources();
    heightMap2.resize(HEIGHT_MAP_SIZE * HEIGHT_MAP_SIZE);

    for (int y = 0; y < HEIGHT_MAP_SIZE; y++)
      for (int x = 0; x < HEIGHT_MAP_SIZE; x++)
        heightMap2[y * HEIGHT_MAP_SIZE + x] = calcHeight(float(x), float(y), mode);

    float min_h = 9e9f;
    float max_h = -9e9f;

    for (float h : heightMap2)
    {
      min_h = std::min(h, min_h);
      max_h = std::max(h, max_h);
    }

    if (max_h > min_h)
      for (float& h : heightMap2)
        h = (h - min_h) / (max_h - min_h);

    int square_count = (HEIGHT_MAP_SIZE - 1) * (HEIGHT_MAP_SIZE - 1);
    mesh = {};
    mesh.triangleCount = square_count * 2;
    mesh.vertexCount = mesh.triangleCount * 3;
    mesh.vertices = (float*)malloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.texcoords = (float*)malloc(mesh.vertexCount * 2 * sizeof(float));
    mesh.normals = (float*)malloc(mesh.vertexCount * 3 * sizeof(float));
    mesh.colors = (unsigned char*)malloc(mesh.vertexCount * 4 * sizeof(unsigned char));
    int i = 0;

    for (int y = 0; y < HEIGHT_MAP_SIZE - 1; y++)
      for (int x = 0; x < HEIGHT_MAP_SIZE - 1; x++)
      {
        float h00 = heightMap2[y * HEIGHT_MAP_SIZE + x];
        float h01 = heightMap2[(y + 1) * HEIGHT_MAP_SIZE + x];
        float h10 = heightMap2[y * HEIGHT_MAP_SIZE + x + 1];
        float h11 = heightMap2[(y + 1) * HEIGHT_MAP_SIZE + x + 1];

        float scale = TERRAIN_SIZE / (HEIGHT_MAP_SIZE - 1);

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
        mesh.texcoords[i * 12 + 0] = (float)x / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 1] = (float)y / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 2] = (float)x / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 3] = (float)(y + 1) / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 4] = (float)(x + 1) / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 5] = (float)(y + 1) / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 6] = (float)x / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 7] = (float)y / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 8] = (float)(x + 1) / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 9] = (float)(y + 1) / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 10] = (float)(x + 1) / (HEIGHT_MAP_SIZE - 1) * tex_scale;
        mesh.texcoords[i * 12 + 11] = (float)y / (HEIGHT_MAP_SIZE - 1) * tex_scale;

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
    modelLoaded = true;

    if (texturePath)
    {
      texture = LoadTexture(texturePath);
      textureLoaded = true;
    }

    //GenTextureMipmaps(&texture);
    //SetTextureFilter(texture, TEXTURE_FILTER_ANISOTROPIC_16X);
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
  }

  float Terrain::calcHeight(float x, float y, Mode mode) const
  {
    if (x == 0 || x == HEIGHT_MAP_SIZE - 1 || y == 0 || y == HEIGHT_MAP_SIZE - 1)
      return 1.0f;

    if (mode == Debug1)
      return
      y == HEIGHT_MAP_SIZE - 2 ? 0.3f :
      y == HEIGHT_MAP_SIZE - 3 ? 0.0f :
      y == HEIGHT_MAP_SIZE - 4 ? 0.1f :
      y == HEIGHT_MAP_SIZE - 5 ? 0.0f :
      0.1f;
    else if (mode == Debug2)
      return fabsf(y - HEIGHT_MAP_SIZE / 2) < HEIGHT_MAP_SIZE / 4 ? 1 - fabsf(y - HEIGHT_MAP_SIZE / 2) : 0.0f;

    x = x / HEIGHT_MAP_SIZE;
    y = (y + HEIGHT_MAP_SIZE / 2) / HEIGHT_MAP_SIZE;

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
        kn * k * sinf(2 * float(M_PI) * x * y * ka + add) +
        kn * k * sinf(2 * float(M_PI) * x * ka + add) +
        kn * k * sinf(2 * float(M_PI) * y * ka + add);
      sz /= 2;
      ka *= 2.0f;
      k /= 2.0f;
      add += 0.123f;
    }

    h += 0.5f;

    return h;
  }
}