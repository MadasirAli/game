#include "random.h"

using namespace game;

random::random(int seed) :
  _generator(seed)
{}

int random::get_int(int min, int max)
{
  return std::uniform_int_distribution<int>(min, max)(_generator);
}