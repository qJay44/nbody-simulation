// http://arborjs.org/docs/barnes-hut

#include "Particle.hpp"
#include <list>

struct Rectangle {
  const float x, y, w, h;
  const float top, right, bottom, left;

  // Coords must point to the center of the rectangle
  Rectangle(float x, float y, float w, float h)
    : x(x), y(y), w(w), h(h),
      top(y - h), right(x + w), bottom(y + h), left(x - w) {}

  bool contains(const Particle* p) const {
    const float& px = p->getPosition().x;
    const float& py = p->getPosition().y;
    return (
      px >= left  &&
      px <= right &&
      py >= top   &&
      py <= bottom
    );
  }

  bool intersects(const Rectangle& r) const {
    return !(
      top    > r.bottom ||
      right  < r.left   ||
      left   > r.right  ||
      bottom < r.top
    );
  }
};

class Node {
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

  inline static float distanceSq(const sf::Vector2f& v1, const sf::Vector2f& v2) {
    sf::Vector2f v = v1 - v2;
    return v.x * v.x + v.y * v.y;
  }

  inline static bool isFar(float s, float dd) {
    return (s * s) / dd < QUAD_TREE_THETA_SQ;
  }

  void updateGravityField(const sf::Vector2f& pos, const uint32_t& m2) {
    float& m1 = gravityField.mass;
    float& x = gravityField.center.x;
    float& y = gravityField.center.y;
    float m = m1 + m2;

    x = (x * m1 + pos.x * m2) / m;
    y = (y * m1 + pos.y * m2) / m;
    m1 = m;
  }

  void subdivide() {
    const float& x = boundary.x;
    const float& y = boundary.y;
    float wHalf = boundary.w * 0.5f;
    float hHalf = boundary.h * 0.5f;

    Rectangle nwRect(x - wHalf, y - hHalf, wHalf, hHalf);
    Rectangle neRect(x + wHalf, y - hHalf, wHalf, hHalf);
    Rectangle swRect(x - wHalf, y + hHalf, wHalf, hHalf);
    Rectangle seRect(x + wHalf, y + hHalf, wHalf, hHalf);

    northWest = new Node(nwRect, depth + 1);
    northEast = new Node(neRect, depth + 1);
    southWest = new Node(swRect, depth + 1);
    southEast = new Node(seRect, depth + 1);

    for (const Particle* p : particles) {
      northWest->insert(p) ||
      northEast->insert(p) ||
      southWest->insert(p) ||
      southEast->insert(p);
    }

    particles.clear();
    divided = true;
  }

  public:
    Node(Rectangle boundary, uint8_t depth = 0)
      : boundary(boundary), depth(depth) {
      gravityField = {{boundary.x, boundary.y}, 0.f};
    }

    ~Node() {
      delete northWest;
      delete northEast;
      delete southWest;
      delete southEast;
    }

    bool insert(const Particle* p) {
      // Check if particle is within boundaries
      if (!boundary.contains(p)) return false;

      // 1. If this node does not contain a body or reached depth cap, put the new particle here
      if (particles.size() < QUAD_TREE_MAX_CAPACITY || depth == QUAD_TREE_MAX_DEPTH) {
        particles.push_back(p);
        return true;
      }

      // 2. If this node is an internal node, update the gravity field.
      // Recursively insert the particle in the appropriate quadrant
      if (divided) {
        updateGravityField(p->getPosition(), p->getMass());
        return
          northWest->insert(p) ||
          northEast->insert(p) ||
          southWest->insert(p) ||
          southEast->insert(p);

      // 3. If this node is an external node (which already containing other particle),
      // subdivide the region and recursively insert these particles into the appropriate quadrants
      } else {
        subdivide();
        return true;
      }
    }

    void solveAttraction(Particle* p1) {
      // 1. If this node is an external,
      // try to calculate the force on the particle by other particle (if have any and not the same).
      if (!divided) {
        for (const Particle* p2 : particles)
          if (p1 != p2)
            p1->attract(p2->getPosition(), p2->getMass());

      // 2. Otherwise, calculate the ration s/d. If s/d < θ,
      // treat this internal node as a single body, and calculate the force for the particle.
      } else if (isFar(boundary.w * 2.f, distanceSq(p1->getPosition(), gravityField.center)))
        p1->attract(gravityField.center, gravityField.mass);

      // 3. Otherwise, run the procedure recursively for other nodes
      else {
        northWest->solveAttraction(p1);
        northEast->solveAttraction(p1);
        southWest->solveAttraction(p1);
        southEast->solveAttraction(p1);
      }
    }

    void show(sf::RenderTarget& target, const uint8_t& limit) {
      static const sf::Color color = sf::Color(30, 30, 30);

      sf::VertexArray rect(sf::LinesStrip, 4);
      rect[0] = sf::Vertex({boundary.left , boundary.top}, color);
      rect[1] = sf::Vertex({boundary.right, boundary.top}, color);
      rect[2] = sf::Vertex({boundary.right, boundary.bottom}, color);
      rect[3] = sf::Vertex({boundary.left , boundary.bottom}, color);
      target.draw(rect);

      if (divided && depth <= limit) {
        northWest->show(target, limit);
        northEast->show(target, limit);
        southWest->show(target, limit);
        southEast->show(target, limit);
      }
    }
};

