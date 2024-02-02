#pragma once

#include <raylib.h>
#include <cstdlib>

class Point {
public:
  Point() = default;

  Vector2 getPos() { return this->pos; }

  void setPos(Vector2 new_pos) { this->pos = new_pos; }

private:
  Vector2 pos = {static_cast<float>(300 + rand() % (100)),
                 static_cast<float>(100 + rand() % (250))};
};
