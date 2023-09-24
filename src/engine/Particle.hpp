#include "../pch.h"
#include "myutils.hpp"
#include <cmath>

struct Particle {
  sf::Vector2f position;
  sf::Vector2f velocity;
  sf::Vector2f acceleration;
  float mass = 1e10;
  int index;

  void update() {
    velocity += acceleration;
    position += velocity;
    acceleration *= 0.f;

    position.x = std::clamp(position.x, 0.f, WIDTH * 1.f);
    position.y = std::clamp(position.y, 0.f, HEIGHT * 1.f);
  }

  void attract(Particle& rhs) {
    constexpr float G = 6.67e-11;
    sf::Vector2f v = position - rhs.position;
    float dist = Particle::magnitude(v);
    float strength = (G * mass * rhs.mass) / (dist * dist);

    Particle::normalize(v);
    rhs.applyForce(v * strength);
  }

  private:
    inline static float magnitude(const sf::Vector2f& vec) {
      return std::max(static_cast<float>(sqrt(vec.x * vec.x + vec.y * vec.y)), 5.f);
    }

    inline static void normalize(sf::Vector2f& vec) {
      vec /= std::max(static_cast<float>(sqrt(vec.x * vec.x + vec.y * vec.y)), 0.0001f);
    }

    void applyForce(sf::Vector2f force) {
      acceleration += force / mass;
    }
};

