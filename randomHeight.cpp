#include <cstdlib>
#include <time.h>
#include "randomHeight.h"

float getRandomHeight(int from, int to)
{

	return from + static_cast<float>(rand()) / RAND_MAX * (to - from);
}