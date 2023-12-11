#include "../pch.h"
#include <cmath>

class Particle {
  float mass;
  float radius;
  sf::Vector2f position;
  sf::Vector2f velocity;
  sf::Vector2f acceleration;
  sf::VertexArray vertices{sf::Quads, 4};

  void updatePosition(const float& dt) {
    position += velocity * dt;
    velocity += acceleration * dt;
    acceleration *= 0.f;
  }

  void updatePositionVertices() {
    vertices[0].position = position + sf::Vector2f{-radius, -radius};
    vertices[1].position = position + sf::Vector2f{ radius, -radius};
    vertices[2].position = position + sf::Vector2f{ radius,  radius};
    vertices[3].position = position + sf::Vector2f{-radius,  radius};
  }

  public:
    Particle(sf::Vector2f position, float mass = INITIAL_MASS, float radius = RADIUS)
      : position(position), mass(mass), radius(radius) {
      static const sf::Color color = sf::Color(60, 60, 60);

      vertices[0].texCoords = {0.f, 0.f};
      vertices[1].texCoords = {CIRCLE_TEXTURE_SIZE, 0.f};
      vertices[2].texCoords = {CIRCLE_TEXTURE_SIZE, CIRCLE_TEXTURE_SIZE};
      vertices[3].texCoords = {0.f, CIRCLE_TEXTURE_SIZE};

      vertices[0].color = color;
      vertices[1].color = color;
      vertices[2].color = color;
      vertices[3].color = color;
    }

    const sf::Vector2f& getPosition() const {
      return position;
    }

    const float& getMass() const {
      return mass;
    }

    const sf::VertexArray& getVertices() const {
      return vertices;
    }

    void update(const float& dt) {
      updatePosition(dt);
      updatePositionVertices();
    }

    void attract(const sf::Vector2f& attractorPos, const float& attractorMass) {
      constexpr float distMin = 0.01f;

      sf::Vector2f v = attractorPos - position;
      float magSq = std::max(v.x * v.x + v.y * v.y, distMin);
      float mag = std::sqrt(magSq);
      sf::Vector2f strength = attractorMass / (magSq * mag) * v;

      acceleration += strength;
    }
};

