#pragma once

#include "Particle.hpp"
#include <list>

namespace qt {
  struct Rectangle {
    const float x, y, w, h;
    const float top, right, bottom, left;

    // Coords must point to the center of the rectangle
    Rectangle(float x, float y, float w, float h);

    bool contains(const Particle* p) const;
    bool intersects(const Rectangle& r) const;
  };

  class Node {
    public:
      Node(Rectangle boundary, uint8_t depth = 0);
      ~Node();

      bool insert(const Particle* p);

      void solveAttraction(Particle* p1);
      void show(sf::RenderTarget& target, const uint8_t& limit);

    private:
      struct GravityField {
        sf::Vector2f center;
        float mass;
      };

      GravityField gravityField;
      Rectangle boundary;
      std::list<const Particle*> particles;
      uint8_t depth;
      bool divided = false;

      Node* northWest = nullptr;
      Node* northEast = nullptr;
      Node* southWest = nullptr;
      Node* southEast = nullptr;

    private:
      void updateGravityField(const sf::Vector2f& pos, const uint32_t& m2);
      void subdivide();
  };
}

