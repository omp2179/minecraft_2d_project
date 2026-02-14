#pragma once
#include <cstddef>
#include <functional>
#include <iostream>

struct Coord {
  int x=0;
  int y=0;

  bool operator==(const Coord &other) const {
    return x == other.x && y == other.y;
  }

  Coord operator+(const Coord &other) const {
    return {x + other.x, y + other.y};
  }

  Coord operator-(const Coord &other) const {
    return {x - other.x, y - other.y};
  }
};

inline std::ostream &operator<<(std::ostream &os, const Coord &coord) {
  os << "(" << coord.x << ", " << coord.y << ")";
  return os;
}

struct CoordHash {
  size_t operator()(const Coord &c) const {
    size_t h1 = std::hash<int>{}(c.x);
    size_t h2 = std::hash<int>{}(c.y);
    return h1 ^ (h2 * 2654435761u);
  }
};