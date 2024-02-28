#include "pch.h"
#include "CppUnitTest.h"
#include "Terrain.h"
#include "Helpers.h"
#include <functional>
//#include "../3rd_party/catch2/catch.hpp"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace game;
using namespace std;

//TEST_CASE("Factorials are computed", "[factorial]")
//{
//  REQUIRE(1 == 1);
//  REQUIRE(2 == 2);
//  REQUIRE(3 == 6);
//  REQUIRE((10) == 3628800);
//}

namespace math_tests
{
  constexpr float TEST_EPSILON = 0.000001f;

  TEST_CLASS(vec3test)
  {
  public:
    TEST_METHOD(normalize)
    {
      vec3 norm{ 1, 2, 3 };
      norm.normalize();
      vec3 expected{ 0.267261f, 0.534522f, 0.801784f };
      const float DELTA = 0.000001f;
      Assert::IsTrue((norm - expected).isAlmostZero(DELTA));
    }

    TEST_METHOD(reflected)
    {
      for (int i = 0; i < 1000; i++)
      {
        vec3 v = vec3::randomInCube(1);
        vec3 norm = vec3::randomInCube(1).normalized();
        vec3 refl = v.reflected(norm);

        Assert::IsTrue(fabsf(refl * norm + v * norm) < TEST_EPSILON);

        Assert::IsTrue(fabsf(refl.length() - v.length()) < TEST_EPSILON);

        Assert::IsTrue(fabsf((refl - v).length() - 2 * fabsf(v * norm)) < TEST_EPSILON);
      }
    }

    TEST_METHOD(projectedOnVector)
    {
      for (int i = 0; i < 1000; i++)
      {
        vec3 v = vec3::randomInCube(1);
        vec3 norm = vec3::randomInCube(1).normalized();
        vec3 proj = v.projectedOnVector(norm);

        Assert::IsTrue(((proj % norm) - vec3::zero).sqLength() < TEST_EPSILON);

        Assert::IsTrue(fabsf(fabsf(proj * norm) - proj.length()) < TEST_EPSILON);
      }
    }
  };

  TEST_CLASS(TerrainTest)
  {
  private:
    struct Line
    {
      vec3 begin{};
      vec3 end{};
    };

    static Terrain terrain;
    static inline const int TRY_COUNT = 1000000;
    static inline const float MIN_XZ = -Terrain::TERRAIN_SIZE / 2 + 0.01f;
    static inline const float MAX_XZ = Terrain::TERRAIN_SIZE / 2 - 0.01f;
    static inline const float MIN_Y = -Terrain::TERRAIN_HEIGHT - 1;
    static inline const float MAX_Y = Terrain::TERRAIN_HEIGHT + 1;

    void runTest(function<Line()> createLine)
    {
      std::wstring msg;
      std::vector<Line> failed;

      msg.reserve(100 * TRY_COUNT);
      failed.reserve(TRY_COUNT);

      bool allHit = true;

      for (int i = 0; i < TRY_COUNT; i++)
      {
        Line line = createLine();

        vec3 collision, normal;
        bool hit = terrain.traceRay(line.begin, (line.end - line.begin).normalized(), &collision, &normal);
        allHit = allHit && hit;

        if (!hit)
          failed.push_back(line);
      }

      wchar_t buf[256];
      swprintf_s(buf, 256, L"\n Failed: %i\n", (int)failed.size());
      msg += buf;

      for (const Line& line : failed)
      {
        swprintf_s(buf, 256, L"{ %.12f, %.12f, %.12f }, { %.12f, %.12f, %.12f },\n", line.begin.x, line.begin.y, line.begin.z, line.end.x, line.end.y, line.end.z);
        msg += buf;
      }

      Assert::IsTrue(allHit, msg.c_str());
    }

    void runTestNoHit(function<Line()> createLine)
    {
      std::wstring msg;
      std::vector<Line> failed;

      msg.reserve(100 * TRY_COUNT);
      failed.reserve(TRY_COUNT);

      bool anyHit = false;

      for (int i = 0; i < TRY_COUNT; i++)
      {
        Line line = createLine();

        vec3 collision, normal;
        bool hit = terrain.traceRay(line.begin, (line.end - line.begin).normalized(), &collision, &normal);
        anyHit = anyHit || hit;

        if (hit)
          failed.push_back(line);
      }

      wchar_t buf[256];
      swprintf_s(buf, 256, L"\n Failed: %i\n", (int)failed.size());
      msg += buf;

      for (const Line& line : failed)
      {
        swprintf_s(buf, 256, L"{ %f, %f, %f }, { %f, %f, %f },\n", line.begin.x, line.begin.y, line.begin.z, line.end.x, line.end.y, line.end.z);
        msg += buf;
      }

      Assert::IsFalse(anyHit, msg.c_str());
    }

  public:
    TEST_CLASS_INITIALIZE(initClass)
    {
      InitWindow(640, 480, "");
      terrain.generate2(nullptr, Terrain::Normal);
    }

    TEST_METHOD(traceRay_random)
    {
      runTest([]() {
        vec3 begin = {
          randf(MIN_XZ, MAX_XZ),
          MAX_Y,
          randf(MIN_XZ, MAX_XZ),
        };

        vec3 end = {
          randf(MIN_XZ, MAX_XZ),
          MIN_Y,
          randf(MIN_XZ, MAX_XZ),
        };

        return Line{ begin, end };
      });
    }

    TEST_METHOD(traceRay_parallel_x)
    {
      runTest([]() {
        vec3 begin = {
          randf(MIN_XZ, MAX_XZ),
          MAX_Y,
          randf(MIN_XZ, MAX_XZ),
        };

        vec3 end = {
          begin.x,
          MIN_Y,
          randf(MIN_XZ, MAX_XZ),
        };

        return Line{ begin, end };
      });
    }

    TEST_METHOD(traceRay_parallel_z)
    {
      runTest([]() {
        vec3 begin = {
          randf(MIN_XZ, MAX_XZ),
          MAX_Y,
          randf(MIN_XZ, MAX_XZ),
        };

        vec3 end = {
          randf(MIN_XZ, MAX_XZ),
          MIN_Y,
          begin.z,
        };

        return Line{ begin, end };
      });
    }

    TEST_METHOD(traceRay_diagonal)
    {
      runTest([]() {
        vec3 begin = {
          randf(MIN_XZ, MAX_XZ),
          MAX_Y,
          randf(MIN_XZ, MAX_XZ),
        };

        vec3 end = {
          begin.z,
          MIN_Y,
          begin.x,
        };

        return Line{ begin, end };
      });
    }

    TEST_METHOD(traceRay_1_cell_intensive)
    {
      float x = -0.01f;
      float z = -0.01f;
      float dx = Terrain::CELL_SIZE / TRY_COUNT * 1.1f;
      float dz = Terrain::CELL_SIZE / TRY_COUNT * 1.1f;

      runTest([&]() {
        vec3 begin = {
          (x += dx),
          MAX_Y,
          (z += dz),
        };

        vec3 end = {
          begin.x + Terrain::TERRAIN_SIZE,
          MIN_Y,
          begin.z + Terrain::TERRAIN_SIZE,
        };

        return Line{ begin, end };
      });
    }

    TEST_METHOD(traceRay_vertical)
    {
      runTest([]() {
        vec3 begin = {
          randf(MIN_XZ, MAX_XZ),
          MAX_Y,
          randf(MIN_XZ, MAX_XZ),
        };

        vec3 end = {
          begin.x,
          MIN_Y,
          begin.z,
        };

        return Line{ begin, end };
      });
    }

    TEST_METHOD(traceRay_no_hit_vertical)
    {
      runTestNoHit([]() {
        vec3 begin = {
          randf(MIN_XZ, MAX_XZ),
          MAX_Y,
          randf(MIN_XZ, MAX_XZ),
        };

        vec3 end = {
          begin.x,
          MAX_Y + 10,
          begin.z,
        };

        return Line{ begin, end };
      });
    }

    TEST_METHOD(traceRay_no_hit_random_above)
    {
      runTestNoHit([]() {
        vec3 begin = {
          randf(MIN_XZ, MAX_XZ),
          randf(MAX_Y, 2 * MAX_Y),
          randf(MIN_XZ, MAX_XZ),
        };

        vec3 end = {
          randf(MIN_XZ, MAX_XZ),
          randf(2 * MAX_Y, 4 * MAX_Y),
          randf(MIN_XZ, MAX_XZ),
        };

        return Line{ begin, end };
      });
    }

    TEST_METHOD(traceRay_no_hit_random_below)
    {
      runTestNoHit([]() {
        vec3 begin = {
          randf(MIN_XZ, MAX_XZ),
          randf(-MAX_Y, -2 * MAX_Y),
          randf(MIN_XZ, MAX_XZ),
        };

        vec3 end = {
          randf(MIN_XZ, MAX_XZ),
          randf(-2 * MAX_Y, -4 * MAX_Y),
          randf(MIN_XZ, MAX_XZ),
        };

        return Line{ begin, end };
      });
    }

    TEST_METHOD(traceRay_no_hit_random_outside)
    {
      runTestNoHit([]() {
        vec3 begin = {
          randf(MAX_XZ + 1, 2 * MAX_XZ),
          MAX_Y,
          randf(MIN_XZ, MAX_XZ),
        };

        vec3 end = {
          randf(2 * MAX_XZ + 1, 4 * MAX_XZ),
          MIN_Y,
          randf(MIN_XZ, MAX_XZ),
        };

        return Line{ begin, end };
      });
    }

  };

  Terrain TerrainTest::terrain;
}
