#include <cmath>

#include "Spawner.hpp"

#define PI 3.14159265359f

void Spawner::spiral(std::vector<Particle>& container, sf::Vector2f center) {
  float stepRad = (2.f * PI) / SPIRAL_ARMS;

  for (int i = 0; i < SPIRAL_ARMS; i++) {
    float startRad = i * stepRad;
    for (int j = 0; j < SPIRAL_ARMS_WIDTH; j++) {
      float startArmRad = j * PI / SPIRAL_ARMS_WIDTH_VALUE / SPIRAL_ARMS_WIDTH;
      for (int k = 0; k < SPIRAL_ARM_LENGTH; k++) {
        sf::Vector2f pos = center;
        float rad = startRad + startArmRad + k * PI / SPIRAL_ARM_TWIST_VALUE;
        pos += {cosf(rad) * k, sinf(rad) * k};
        container.push_back(Particle(pos));
      }
    }
  }
}

void Spawner::random(std::vector<Particle>& container, bool heavyCenter) {
  if (heavyCenter)
    container.push_back(Particle({WIDTH * 0.5f, HEIGHT * 0.5f}, 300.f, 5.f));

  for (int i = 0; i < INITIAL_PARTICLES - 1; i++)
    container.push_back(Particle(sf::Vector2f(rand() % WIDTH, rand() % HEIGHT)));
}

