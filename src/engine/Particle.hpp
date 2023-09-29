#include "../pch.h"
#include "myutils.hpp"
#include <cmath>

class Particle {
  float mass = 1e10f;
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

  void applyForce(sf::Vector2f direction, double& strength) {
    acceleration.x -= (direction.x * strength) / mass;
    acceleration.y -= (direction.y * strength) / mass;
  }

  void updatePosition() {
    velocity += acceleration;
    position += velocity;
    acceleration *= 0.f;

    position.x = std::clamp(position.x, 0.f, WIDTH * 1.f);
    position.y = std::clamp(position.y, 0.f, HEIGHT * 1.f);
  }

  void updatePositionVertices() {
    vertices[0].position = position + sf::Vector2f{-RADIUS, -RADIUS};
    vertices[1].position = position + sf::Vector2f{ RADIUS, -RADIUS};
    vertices[2].position = position + sf::Vector2f{ RADIUS,  RADIUS};
    vertices[3].position = position + sf::Vector2f{-RADIUS,  RADIUS};
  }

  public:
    Particle(sf::Vector2f position, const sf::Texture* tex, float mass = INITIAL_MASS)
      : position(position), mass(mass) {
      float texWidght = tex->getSize().x;
      float texHeight = tex->getSize().y;

      sf::Vertex& tl = vertices[0];
      sf::Vertex& tr = vertices[1];
      sf::Vertex& br = vertices[2];
      sf::Vertex& bl = vertices[3];

      tl.texCoords = {0.f      , 0.f      };
      tr.texCoords = {texWidght, 0.f      };
      br.texCoords = {texWidght, texHeight};
      bl.texCoords = {0.f      , texHeight};
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

