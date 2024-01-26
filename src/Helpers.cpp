#include "pch.h"
#include "Helpers.h"

namespace game
{

	int fastRand(void)
	{
		gSeed = (214013 * gSeed + 2531011);

		return (gSeed >> 16) & FAST_RAND_MAX;
	}

}