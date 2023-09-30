#include "../pch.h"
#include "myutils.hpp"
#include <cmath>

class Particle {
  float mass;
  float radius;
  sf::Vector2f position;
  sf::Vector2f velocity;
  sf::Vector2f acceleration;
  sf::VertexArray vertices{sf::Quads, 4};

  inline static float magnitude(const sf::Vector2f& vec) {
    return std::max(static_cast<float>(sqrt(vec.x * vec.x + vec.y * vec.y)), 5.f);
  }

  inline static sf::Vector2f normalize(const sf::Vector2f& vec) {
    return vec / std::max(static_cast<float>(sqrt(vec.x * vec.x + vec.y * vec.y)), 0.0001f);
  }

  void applyForce(sf::Vector2f direction, const double& strength) {
    acceleration.x -= (direction.x * strength) / mass;
    acceleration.y -= (direction.y * strength) / mass;
  }

  void updatePosition() {
    velocity += acceleration;
    position += velocity;
    acceleration *= 0.f;
  }

  void updatePositionVertices() {
    vertices[0].position = position + sf::Vector2f{-radius, -radius};
    vertices[1].position = position + sf::Vector2f{ radius, -radius};
    vertices[2].position = position + sf::Vector2f{ radius,  radius};
    vertices[3].position = position + sf::Vector2f{-radius,  radius};
  }

  public:
    Particle(sf::Vector2f position, const sf::Texture* tex, float mass = INITIAL_MASS, float radius = RADIUS)
      : position(position), mass(mass), radius(radius) {
      float texWidght = tex->getSize().x;
      float texHeight = tex->getSize().y;

      vertices[0].texCoords = {0.f      , 0.f      };
      vertices[1].texCoords = {texWidght, 0.f      };
      vertices[2].texCoords = {texWidght, texHeight};
      vertices[3].texCoords = {0.f      , texHeight};

      vertices[0].color = {1, 50, 40};
      vertices[1].color = {1, 50, 40};
      vertices[2].color = {1, 50, 40};
      vertices[3].color = {1, 50, 40};
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

    void update() {
      updatePosition();
      updatePositionVertices();
    }

    void attract(const sf::Vector2f& attractorPos, const double& attractorMass) {
      constexpr float G = 6.67e-11;
      sf::Vector2f v = position - attractorPos;
      float dist = magnitude(v);
      double strength = (G * mass * attractorMass) / (dist * dist);

      applyForce(normalize(v), strength);
    }

    void setColor(sf::Color col) {
      vertices[0].color = col;
      vertices[1].color = col;
      vertices[2].color = col;
      vertices[3].color = col;
    }
};

