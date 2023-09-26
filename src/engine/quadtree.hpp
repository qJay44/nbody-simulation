#include "../pch.h"
#include "../preferences.h"
#include "Particle.hpp"
#include <vector>

struct Rectangle {
  // NOTE: Coords must point to the center of a rectangle
  const float x, y, w, h;
  const float top, right, bottom, left;

  Rectangle(float x, float y, float w, float h)
    : x(x), y(y), w(w), h(h),
      top(y - h), right(x + w), bottom(y + h), left(x - w) {}

  bool contains(const Particle& p) const {
    return (
      p.position.x >= left  &&
      p.position.x <= right &&
      p.position.y >= top   &&
      p.position.y <= bottom
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

class QuadTree {
  struct GravityField {
    sf::Vector2f center;
    double mass = 0;
  };

  Rectangle boundary;
  std::vector<Particle> particles;
  uint8_t depth;
  bool divided = false;

  QuadTree* northWest = nullptr;
  QuadTree* northEast = nullptr;
  QuadTree* southWest = nullptr;
  QuadTree* southEast = nullptr;

  inline static float distance(const sf::Vector2f& v1, const sf::Vector2f& v2) {
    sf::Vector2f v = v1 - v2;
    return sqrt(v1.x * v1.x + v2.y * v2.y);
  }

  inline static bool isFar(float s, float d) {
    return s / d < QUAD_TREE_THETA;
  }

  void updateGravityField(const sf::Vector2f& pos, const uint32_t& m2) {
    double& m1 = gravityField.mass;
    float& x = gravityField.center.x;
    float& y = gravityField.center.y;
    double m = m1 + m2;

    x = (x * m1 + pos.x * m2) / m;
    y = (y * m1 + pos.y * m2) / m;
    m1 = m;
  }

  void subdivide() {
    const float& x = boundary.x;
    const float& y = boundary.y;
    const float& w = boundary.w;
    const float& h = boundary.h;

    Rectangle nwRect(x - w / 2, y - h / 2, w / 2, h / 2);
    Rectangle neRect(x + w / 2, y - h / 2, w / 2, h / 2);
    Rectangle swRect(x - w / 2, y + h / 2, w / 2, h / 2);
    Rectangle seRect(x + w / 2, y + h / 2, w / 2, h / 2);

    northWest = new QuadTree(nwRect, depth + 1);
    northEast = new QuadTree(neRect, depth + 1);
    southWest = new QuadTree(swRect, depth + 1);
    southEast = new QuadTree(seRect, depth + 1);

    for (const Particle& p : particles) {
      northWest->insert(p) ||
      northEast->insert(p) ||
      southWest->insert(p) ||
      southEast->insert(p);
    }

    particles.clear();
    particles.reserve(0);
    divided = true;
  }

  public:
    GravityField gravityField;
    QuadTree(Rectangle boundary, uint8_t depth = 0)
      : boundary(boundary), depth(depth) {
      particles.reserve(QUAD_TREE_CAPACITY);
      gravityField.center = {boundary.x, boundary.y};
    }

    ~QuadTree() {
      delete northWest;
      delete northEast;
      delete southWest;
      delete southEast;
    }

    bool insert(const Particle& p) {
      // Do not insert if particle is not within boundaries
      if (!boundary.contains(p)) return false;


      if (!divided) {
        if (particles.size() < QUAD_TREE_CAPACITY || depth == QUAD_TREE_DEPTH) {
          particles.push_back(p);
          updateGravityField(p.position, p.mass);
          return true;
        }
        subdivide();
      }

      return
        northWest->insert(p) ||
        northEast->insert(p) ||
        southWest->insert(p) ||
        southEast->insert(p);
    }

    void query(std::vector<Particle>& found, const Rectangle& range) {
      if (boundary.intersects(range)) {
        for (const Particle& p : particles)
          if (range.contains(p))
            found.push_back(p);
      }

      if (divided) {
        northWest->query(found, range);
        northEast->query(found, range);
        southWest->query(found, range);
        southEast->query(found, range);
      }
    }

    bool solveAttraction(Particle& p) {
      if (divided) {
        if (!isFar(boundary.w * 2.f, distance(p.position, gravityField.center))) {
          p.attract(gravityField.center, gravityField.mass);
        } else {
          northWest->solveAttraction(p) ||
          northEast->solveAttraction(p) ||
          southWest->solveAttraction(p) ||
          southEast->solveAttraction(p);
        }
      } else
        p.attract(gravityField.center, gravityField.mass);
      return true;
    }

    void show(sf::RenderTarget& target) {
      static sf::Color color = sf::Color(30, 30, 30);

      sf::VertexArray rect(sf::LinesStrip, 4);
      rect[0] = sf::Vertex({boundary.left , boundary.top}, color);
      rect[1] = sf::Vertex({boundary.right, boundary.top}, color);
      rect[2] = sf::Vertex({boundary.right, boundary.bottom}, color);
      rect[3] = sf::Vertex({boundary.left , boundary.bottom}, color);
      target.draw(rect);

      if (divided) {
        northWest->show(target);
        northEast->show(target);
        southWest->show(target);
        southEast->show(target);
      }
    }
};

