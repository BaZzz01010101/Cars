#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace game;

namespace math_tests
{
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
      vec3 v{ 1, 1, 1 };
      vec3 norm{ 0, 1, 1 };
      norm.normalize();
      vec3 refl = v.reflected(norm);
      const float DELTA = 0.000001f;
      vec3 expected = { -1, 1, 1 };
      Assert::IsTrue((refl - expected).isAlmostZero(DELTA));
    }
  
    TEST_METHOD(projectedOnVector)
    {
      vec3 v{ 1, 1, 1 };
      vec3 norm{ 0, 1, 1 };
      norm.normalize();
      vec3 proj = v.projectedOnVector(norm);
      const float DELTA = 0.000001f;
      vec3 expected = { 1, 0, 0 };

      Assert::IsTrue((proj - expected).isAlmostZero(DELTA));
    }
  };
}
