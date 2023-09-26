#include "../pch.h"
#include "myutils.hpp"
#include <cmath>

struct Particle {
  sf::Vector2f position;
  sf::Vector2f velocity;
  sf::Vector2f acceleration;
  uint32_t index;
  float mass = 1e10f;

  void update() {
    velocity += acceleration;
    position += velocity;
    acceleration *= 0.f;

    position.x = std::clamp(position.x, 0.f, WIDTH * 1.f);
    position.y = std::clamp(position.y, 0.f, HEIGHT * 1.f);
  }

  void attract(const sf::Vector2f& attractorPos, const double& attractorMass) {
    constexpr float G = 6.67e-11;
    sf::Vector2f v = position - attractorPos;
    float dist = magnitude(v);
    double strength = (G * mass * attractorMass) / (dist * dist);

    applyForce(normalize(v), strength);
  }

  private:
    inline static float magnitude(const sf::Vector2f& vec) {
      return std::max(static_cast<float>(sqrt(vec.x * vec.x + vec.y * vec.y)), 5.f);
    }

    inline static sf::Vector2f normalize(const sf::Vector2f& vec) {
      return vec / std::max(static_cast<float>(sqrt(vec.x * vec.x + vec.y * vec.y)), 0.0001f);
    }

    void applyForce(sf::Vector2f direction, double& strength) {
      acceleration.x -= (direction.x * strength) / mass;
      acceleration.y -= (direction.y * strength) / mass;
    }
};

