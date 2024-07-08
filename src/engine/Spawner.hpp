#pragma once

#include <vector>

#include "Particle.hpp"

struct Spawner {
  static void spiral(std::vector<Particle>& container, sf::Vector2f center);
  static void random(std::vector<Particle>& container, bool heavyCenter = true);
};

