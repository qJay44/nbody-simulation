#pragma once

#include "Particle.hpp"
#include <list>

namespace qt {
  class Rectangle {
    friend class Node;

    public:
      // Coords must point to the center of the rectangle
      Rectangle(float x, float y, float w, float h);

      bool contains(const Particle* p) const;
      bool intersects(const Rectangle& r) const;

    private:
      const float x, y, w, h; // The width and height are distances from the center to the edges
      const float top, right, bottom, left;
  };

  class Node {
    public:
      Node(Rectangle boundary, uint32_t depth = 0);
      ~Node();

      static void printMaxReachedDepth();

      bool insert(const Particle* p);

      void solveAttraction(Particle* p1);

      // The deeper Quadtree the more time to draw the grid
      void show(sf::RenderTarget& target, const uint32_t& depthLimit);

    private:
      static uint32_t maxDepth;

      struct Gravity {
        sf::Vector2f center;
        float mass;
        void update(const sf::Vector2f& pos, const float& m2);
      };

      std::list<const Particle*> container;
      Gravity gravity;
      Rectangle boundary;
      uint32_t depth;

      Node* northWest = nullptr;
      Node* northEast = nullptr;
      Node* southWest = nullptr;
      Node* southEast = nullptr;

    private:
      void subdivide(const Particle* p);
  };
}

