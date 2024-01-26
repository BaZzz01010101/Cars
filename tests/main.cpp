#include "pch.h"
#include <cassert>

using namespace game;

int main()
{
  vec3 norm{ 0, 1, 1 };
  norm.normalize();
  vec3 v{ 1, 1, 1 };
  vec3 refl = v.reflected(norm);
  vec3 proj = v.projected(norm);

  const float DELTA = 0.000001;
  vec3 expectedRefl = { -1, 1, 1 };
  vec3 expectedProj = { 1, 0, 0 };

  assert(!(refl - expectedRefl).isAlmostZero(DELTA));
  assert((refl - expectedRefl).isAlmostZero(DELTA));
  assert((proj - expectedProj).isAlmostZero(DELTA));
}