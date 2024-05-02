#include "core.h"
#include "CppUnitTest.h"
#include "Terrain.h"
#include "Helpers.h"
#include "SemiVector.hpp"
#include "Ring.hpp"
#include "PlayerControl.h"
#include "PlayerState.h"
#include "PlayerJoin.h"
#include "PlayerLeave.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace game;
using namespace std;
using namespace dto;

namespace game_tests
{

  class Terrain : public game::Terrain
  {
  public:
    CGGridCell(&cgGrid)[CG_GRID_SIZE][CG_GRID_SIZE] = game::Terrain::cgGrid;

    Terrain(const Config& config) :
      game::Terrain(config)
    {
    }

    bool traceRayWithTerrain(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
    {
      return game::Terrain::traceRayWithTerrain(origin, directionNormalized, distance, hitPosition, hitNormal, hitDistance);
    }

    bool traceRayWithObjects(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
    {
      return game::Terrain::traceRayWithObjects(origin, directionNormalized, distance, hitPosition, hitNormal, hitDistance);
    }

    bool traceRayWithObjects_Unoptimized(vec3 origin, vec3 directionNormalized, float distance, vec3* hitPosition, vec3* hitNormal, float* hitDistance) const
    {
      return game::Terrain::traceRayWithObjects_Unoptimized(origin, directionNormalized, distance, hitPosition, hitNormal, hitDistance);
    }
  };

  constexpr float TEST_EPSILON = 0.000001f;

  TEST_CLASS(vec3bench)
  {
  public:
    TEST_METHOD(randomInSphere)
    {
      const int count = 10000000;
      vector<vec3> vv;
      vv.reserve(count);

      for (int i = 0; i < count; i++)
        vv.push_back(vec3::randomInSphere(1));

      Assert::IsTrue(vv.size() == count);
    }

    TEST_METHOD(randomOnSphereSurface)
    {
      const int count = 10000000;
      vector<vec3> vv;
      vv.reserve(count);

      for (int i = 0; i < count; i++)
        vv.push_back(vec3::randomOnSphereSurface(1));

      Assert::IsTrue(vv.size() == count);
    }

    TEST_METHOD(randomInHollowSphere)
    {
      const int count = 10000000;
      vector<vec3> vv;
      vv.reserve(count);

      for (int i = 0; i < count; i++)
        vv.push_back(vec3::randomInHollowSphere(0.5, 1));

      Assert::IsTrue(vv.size() == count);
    }

  };

  TEST_CLASS(vec3test)
  {
  public:
    TEST_METHOD(normalize)
    {
      vec3 norm { 1, 2, 3 };
      norm.normalize();
      vec3 expected { 0.267261f, 0.534522f, 0.801784f };
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

  TEST_CLASS(SemiVectorTest)
  {
  public:
    TEST_METHOD(add_read)
    {
      SemiVector<int, 3> sv;
      sv.add(0);
      sv.add(1);
      sv.add(2);
      sv.add(3);
      sv.add(4);
      sv.add(5);

      Assert::AreEqual(6, sv.size());
      Assert::AreEqual(0, sv[0]);
      Assert::AreEqual(1, sv[1]);
      Assert::AreEqual(2, sv[2]);
      Assert::AreEqual(3, sv[3]);
      Assert::AreEqual(4, sv[4]);
      Assert::AreEqual(5, sv[5]);
    }

    TEST_METHOD(add_clear_add)
    {
      SemiVector<int, 3> sv;
      sv.add(0);
      sv.add(1);
      sv.add(2);
      sv.add(3);
      sv.add(4);
      sv.add(5);

      Assert::AreEqual(6, sv.size());
      sv.clear();
      Assert::AreEqual(0, sv.size());

      sv.add(6);
      sv.add(7);
      sv.add(8);
      sv.add(9);
      sv.add(10);
      sv.add(11);

      Assert::AreEqual(6, sv.size());
      Assert::AreEqual(6, sv[0]);
      Assert::AreEqual(7, sv[1]);
      Assert::AreEqual(8, sv[2]);
      Assert::AreEqual(9, sv[3]);
      Assert::AreEqual(10, sv[4]);
      Assert::AreEqual(11, sv[5]);
    }

    TEST_METHOD(remove_arr_first)
    {
      SemiVector<int, 3> sv;
      sv.add(0);
      sv.add(1);
      sv.add(2);
      sv.add(3);
      sv.add(4);
      sv.add(5);

      sv.remove(0);
      Assert::AreEqual(5, sv.size());
      Assert::AreEqual(1, sv[0]);
      Assert::AreEqual(2, sv[1]);
      Assert::AreEqual(3, sv[2]);
      Assert::AreEqual(4, sv[3]);
      Assert::AreEqual(5, sv[4]);
    }

    TEST_METHOD(remove_arr_middle)
    {
      SemiVector<int, 3> sv;
      sv.add(0);
      sv.add(1);
      sv.add(2);
      sv.add(3);
      sv.add(4);
      sv.add(5);

      sv.remove(1);
      Assert::AreEqual(5, sv.size());
      Assert::AreEqual(0, sv[0]);
      Assert::AreEqual(2, sv[1]);
      Assert::AreEqual(3, sv[2]);
      Assert::AreEqual(4, sv[3]);
      Assert::AreEqual(5, sv[4]);
    }

    TEST_METHOD(remove_arr_last)
    {
      SemiVector<int, 3> sv;
      sv.add(0);
      sv.add(1);
      sv.add(2);
      sv.add(3);
      sv.add(4);
      sv.add(5);

      sv.remove(2);
      Assert::AreEqual(5, sv.size());
      Assert::AreEqual(0, sv[0]);
      Assert::AreEqual(1, sv[1]);
      Assert::AreEqual(3, sv[2]);
      Assert::AreEqual(4, sv[3]);
      Assert::AreEqual(5, sv[4]);
    }

    TEST_METHOD(remove_vec_first)
    {
      SemiVector<int, 3> sv;
      sv.add(0);
      sv.add(1);
      sv.add(2);
      sv.add(3);
      sv.add(4);
      sv.add(5);

      sv.remove(3);
      Assert::AreEqual(5, sv.size());
      Assert::AreEqual(0, sv[0]);
      Assert::AreEqual(1, sv[1]);
      Assert::AreEqual(2, sv[2]);
      Assert::AreEqual(4, sv[3]);
      Assert::AreEqual(5, sv[4]);
    }

    TEST_METHOD(remove_vec_middle)
    {
      SemiVector<int, 3> sv;
      sv.add(0);
      sv.add(1);
      sv.add(2);
      sv.add(3);
      sv.add(4);
      sv.add(5);

      sv.remove(4);
      Assert::AreEqual(5, sv.size());
      Assert::AreEqual(0, sv[0]);
      Assert::AreEqual(1, sv[1]);
      Assert::AreEqual(2, sv[2]);
      Assert::AreEqual(3, sv[3]);
      Assert::AreEqual(5, sv[4]);
    }

    TEST_METHOD(remove_vec_last)
    {
      SemiVector<int, 3> sv;
      sv.add(0);
      sv.add(1);
      sv.add(2);
      sv.add(3);
      sv.add(4);
      sv.add(5);

      sv.remove(5);
      Assert::AreEqual(5, sv.size());
      Assert::AreEqual(0, sv[0]);
      Assert::AreEqual(1, sv[1]);
      Assert::AreEqual(2, sv[2]);
      Assert::AreEqual(3, sv[3]);
      Assert::AreEqual(4, sv[4]);
    }

    TEST_METHOD(remove_vec_add)
    {
      SemiVector<int, 3> sv;
      sv.add(0);
      sv.add(1);
      sv.add(2);
      sv.add(3);
      sv.add(4);
      sv.add(5);

      sv.remove(5);
      sv.add(6);
      Assert::AreEqual(6, sv.size());
      Assert::AreEqual(0, sv[0]);
      Assert::AreEqual(1, sv[1]);
      Assert::AreEqual(2, sv[2]);
      Assert::AreEqual(3, sv[3]);
      Assert::AreEqual(4, sv[4]);
      Assert::AreEqual(6, sv[5]);
    }

  };

  TEST_CLASS(RingCollectionTest)
  {
  public:
    TEST_METHOD(push_read)
    {
      Ring<int, 5> sv(0);
      sv.push(1);
      sv.push(2);
      sv.push(3);
      sv.push(4);

      Assert::AreEqual(4, sv[0]);
      Assert::AreEqual(2, sv[2]);

      sv.push(5);

      Assert::AreEqual(5, sv[0]);
      Assert::AreEqual(3, sv[2]);
    }

  };

  TEST_CLASS(TerrainTest)
  {
  private:
    struct Line
    {
      vec3 begin {};
      vec3 end {};
    };

    static Terrain terrain;
#ifdef _DEBUG
    static inline const size_t TRY_COUNT = 10000;
#else
    static inline const size_t TRY_COUNT = 100000;
#endif
    static inline const float MIN_XZ = -Terrain::TERRAIN_SIZE / 2 + 0.01f;
    static inline const float MAX_XZ = Terrain::TERRAIN_SIZE / 2 - 0.01f;
    static inline const float MIN_Y = -Terrain::TERRAIN_HEIGHT - 1;
    static inline const float MAX_Y = Terrain::TERRAIN_HEIGHT + 1;

    typedef function<bool(const Terrain& terrain, Line line)> TraceWorkerCallback;
    typedef function<Line()> LineCreatorCallback;

    static void runTest(const Terrain& terrain, LineCreatorCallback createLine, TraceWorkerCallback doTrace)
    {
      std::wstring msg;
      std::vector<Line> failed;

      msg.reserve(100 * TRY_COUNT);
      failed.reserve(TRY_COUNT);

      bool allSuccessfull = true;

      for (int i = 0; i < TRY_COUNT; i++)
      {
        Line line = createLine();
        bool isSuccess = doTrace(terrain, line);
        allSuccessfull = allSuccessfull && isSuccess;

        if (!isSuccess)
          failed.push_back(line);
      }

      wchar_t buf[256];
      swprintf_s(buf, 256, L"\nTotal: %i\nFailed: %i\n", (int)TRY_COUNT, (int)failed.size());
      msg += buf;

      for (const Line& line : failed)
      {
        swprintf_s(buf, 256, L"{ %.12f, %.12f, %.12f }, { %.12f, %.12f, %.12f },\n", line.begin.x, line.begin.y, line.begin.z, line.end.x, line.end.y, line.end.z);
        msg += buf;
      }

      Assert::IsTrue(allSuccessfull, msg.c_str());
    }

    static bool handleHitPositiveTrace(const Terrain& terrain, Line line)
    {
      vec3 hitPosition, hitNormal;
      float hitDistance;
      bool isHit = terrain.traceRayWithTerrain(line.begin, (line.end - line.begin).normalized(), (line.end - line.begin).length(), &hitPosition, &hitNormal, &hitDistance);

      return isHit;
    }

    static bool handleHitPositiveTraceWithDistanceCheck(const Terrain& terrain, Line line)
    {
      vec3 hitPosition, hitNormal;
      float hitDistance;
      bool isHit = terrain.traceRayWithTerrain(line.begin, (line.end - line.begin).normalized(), (line.end - line.begin).length(), &hitPosition, &hitNormal, &hitDistance);

      if (isHit)
        Assert::IsTrue(fabsf(sqr(hitDistance) - (hitPosition - line.begin).sqLength()) < 0.1);

      return isHit;
    }

    static bool handleHitNegativeTrace(const Terrain& terrain, Line line)
    {
      bool isHit = terrain.traceRayWithTerrain(line.begin, (line.end - line.begin).normalized(), (line.end - line.begin).length(), nullptr, nullptr, nullptr);

      return !isHit;
    }

    static bool handleObjectsHitPositiveTrace(const Terrain& terrain, Line line)
    {
      vec3 hitPosition1, hitNormal1;
      float hitDistance1;
      vec3 hitPosition2, hitNormal2;
      float hitDistance2;
      vec3 origin = line.begin;
      vec3 direction = line.end - line.begin;
      float distance = direction.length();
      vec3 directionNormalized = direction / distance;

      bool isHit1 = terrain.traceRayWithObjects(origin, directionNormalized, distance, &hitPosition1, &hitNormal1, &hitDistance1);
      bool isHit2 = terrain.traceRayWithObjects_Unoptimized(origin, directionNormalized, distance, &hitPosition2, &hitNormal2, &hitDistance2);

      bool isSuccess = isHit1 == isHit2 && (!isHit1 || (hitPosition1 == hitPosition2 && hitNormal1 == hitNormal2 && hitDistance1 == hitDistance2));

      if (!isSuccess)
      {
        isHit2 = terrain.traceRayWithObjects_Unoptimized(origin, directionNormalized, distance, &hitPosition2, &hitNormal2, &hitDistance2);
        isHit1 = terrain.traceRayWithObjects(origin, directionNormalized, distance, &hitPosition1, &hitNormal1, &hitDistance1);
      }

      return isSuccess;
    }

  public:
    TEST_CLASS_INITIALIZE(initClass)
    {
      terrain.init();
    }

    TEST_METHOD(traceRay_random)
    {
      auto createLine = []() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitPositiveTraceWithDistanceCheck);
    }

    TEST_METHOD(traceRay_random_from_outside)
    {
      auto createLine = []() {
        vec3 begin = randf(1) > 0.5f ? vec3 {
          randf(MAX_XZ + 1, MAX_XZ * 1.1f),
          MAX_Y,
          randf(MIN_XZ / 1.1f, MAX_XZ / 1.1f),
        } : vec3 {
          randf(MIN_XZ, MAX_XZ),
          MAX_Y,
          randf(MAX_XZ + 1, MAX_XZ * 1.1f),
        };

        vec3 end = {
          -begin.x,
          MIN_Y,
          -begin.z,
        };

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitPositiveTrace);
    }

    TEST_METHOD(traceRay_parallel_x)
    {
      auto createLine = []() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitPositiveTrace);
    }

    TEST_METHOD(traceRay_parallel_z)
    {
      auto createLine = []() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitPositiveTrace);
    }

    TEST_METHOD(traceRay_diagonal)
    {
      auto createLine = []() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitPositiveTrace);
    }

    TEST_METHOD(traceRay_1_cell_intensive)
    {
      float x = -0.01f;
      float z = -0.01f;
      float dx = Terrain::CELL_SIZE / TRY_COUNT * 1.1f;
      float dz = Terrain::CELL_SIZE / TRY_COUNT * 1.1f;

      auto createLine = [&]() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitPositiveTrace);
    }

    TEST_METHOD(traceRay_vertical)
    {
      auto createLine = []() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitPositiveTrace);
    }

    TEST_METHOD(traceRay_no_hit_vertical)
    {
      auto createLine = []() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitNegativeTrace);
    }

    TEST_METHOD(traceRay_no_hit_random_above)
    {
      auto createLine = []() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitNegativeTrace);
    }

    TEST_METHOD(traceRay_no_hit_random_below)
    {
      auto createLine = []() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitNegativeTrace);
    }

    TEST_METHOD(traceRay_no_hit_random_outside)
    {
      auto createLine = []() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleHitNegativeTrace);
    }

    TEST_METHOD(traceRayWithObjects_random)
    {
      auto createLine = []() {
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

        return Line { begin, end };
      };

      runTest(terrain, createLine, handleObjectsHitPositiveTrace);
    }

  };

  Terrain TerrainTest::terrain(Config::DEFAULT);

  TEST_CLASS(MessagesTest)
  {
  private:
    template <typename Msg>
    void testMessage(Msg msg, MessageID expectedType)
    {
      BitStream stream;
      msg.writeTo(stream);

      MessageID type;
      stream.Read(type);

      Assert::IsTrue(type == expectedType);

      Msg actual;
      actual.readFrom(stream);

      Assert::IsTrue(msg == actual);
    }


  public:
    TEST_METHOD(PlayerControlMessage)
    {
      PlayerControl msg {
        .physicalFrame = -1234567890123456789,
        .guid = 9876543210987654321,
        .steeringAxis = 0.1f,
        .accelerationAxis = 0.2f,
        .thrustAxis = 0.3f,
        .target = { 0.4f, 0.5f, 0.6f },
        .primaryFire = true,
        .secondaryFire = false,
        .handBrake = true
      };

      testMessage(msg, ID_PLAYER_CONTROL);
    }

    TEST_METHOD(PlayerStateMessage)
    {
      PlayerState msg {
        .physicalFrame = -1234567890123456789,
        .guid = 9876543210987654321,
        .position = { 0.1f, 0.2f, 0.3f },
        .rotation = { 0.4f, 0.5f, 0.6f, 0.7f },
        .velocity = { 0.8f, 0.9f, 1.0f },
        .angularVelocity = { 1.1f, 1.2f, 1.3f },
        .steeringAngle = 1.4f,
        .frontLeftWheelState = { 1.5f, 1.6f, 1.7f },
        .frontRightWheelState = { 1.8f, 1.9f, 2.0f },
        .rearLeftWheelState = { 2.1f, 2.2f, 2.3f },
        .rearRightWheelState = { 2.4f, 2.5f, 2.6f },
        .gunState = { 2.7f, 2.8f },
        .cannonState = { 2.9f, 3.0f },
      };

      testMessage(msg, ID_PLAYER_STATE);
    }

    TEST_METHOD(PlayerJoinMessage)
    {
      PlayerJoin msg {
        .guid = 1234567890,

        .position = {0.1f, 0.2f, 0.3f},
        .rotation = { 0.4f, 0.5f, 0.6f, 0.7f },
      };

      testMessage(msg, ID_PLAYER_JOIN);
    }

    TEST_METHOD(PlayerLeaveMessage)
    {
      PlayerLeave msg {
        .guid = 1234567890,
      };

      testMessage(msg, ID_PLAYER_LEAVE);
    }
  };

}