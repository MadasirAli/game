#include "random.h"

using namespace game;

random::random() :
  _generator(0)
{}

int random::get_int(int min, int max)
{
  return std::uniform_int_distribution<int>(min, max)(_generator);
}