#include "../pch.h"
#include "../preferences.h"
#include <vector>

template<class T>
struct Point {
  float x, y;
  const T* data;
};

template<class T>
struct Rectangle {
  // NOTE: Coords must point to the center of a rectangle
  float x, y, w, h;

  float top    = y - h;
  float right  = x + w;
  float bottom = y + h;
  float left   = x - w;

  bool contains(const Point<T>& p) const {
    return (
      p.x >= x - w &&
      p.x <= x + w &&
      p.y >= y - h &&
      p.y <= y + h
    );
  }

  bool intersects(const Rectangle& r) const {
    return (
      top    < r.bottom ||
      right  < r.left   ||
      left   > r.right  ||
      bottom > r.top
    );
  }
};

template<class T>
class QuadTree {
  Rectangle<T> boundary;
  std::vector<Point<T>> points;
  bool divided = false;
  sf::Color color = sf::Color(30, 30, 30);

  QuadTree* northWest = nullptr;
  QuadTree* northEast = nullptr;
  QuadTree* southWest = nullptr;
  QuadTree* southEast = nullptr;

  void subdivide() {
    if (divided) return;
    divided = true;

    const float& x = boundary.x;
    const float& y = boundary.y;
    const float& w = boundary.w;
    const float& h = boundary.h;

    Rectangle<T> nwRect{x - w / 2, y - h / 2, w / 2, h / 2};
    Rectangle<T> neRect{x + w / 2, y - h / 2, w / 2, h / 2};
    Rectangle<T> swRect{x - w / 2, y + h / 2, w / 2, h / 2};
    Rectangle<T> seRect{x + w / 2, y + h / 2, w / 2, h / 2};

    northWest = new QuadTree(nwRect);
    northEast = new QuadTree(neRect);
    southWest = new QuadTree(swRect);
    southEast = new QuadTree(seRect);
  }

  public:
    QuadTree(Rectangle<T> boundary)
      : boundary(boundary) {
        points.reserve(QUAD_TREE_CAPACITY);
    }

    ~QuadTree() {
      delete northWest;
      delete northEast;
      delete southWest;
      delete southEast;
    }

    bool insert(Point<T> p) {
      if (!boundary.contains(p)) return false;

      if (points.size() < QUAD_TREE_CAPACITY) {
        points.push_back(p);
        return true;
      } else {
        subdivide();
        if (
          northWest->insert(p) ||
          northEast->insert(p) ||
          southWest->insert(p) ||
          southEast->insert(p)
        ) return true;
        else
          throw std::runtime_error("The point somehow didn't inserted in any quad");
      }
    }

    void query(std::vector<Point<T>>& found, const Rectangle<T>& range) {
      if (boundary.intersects(range)) {
        for (const Point<T>& p : points)
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

    void show(sf::RenderTarget& target) {
      const float& x = boundary.x;
      const float& y = boundary.y;
      const float& w = boundary.w;
      const float& h = boundary.h;

      sf::Vertex lineVertical[] = {
        sf::Vertex({x, y - h}, color),
        sf::Vertex({x, y + h}, color)
      };

      sf::Vertex lineHorizontal[] = {
        sf::Vertex({x - w, y}, color),
        sf::Vertex({x + w, y}, color)
      };

      target.draw(lineVertical, 2, sf::Lines);
      target.draw(lineHorizontal, 2, sf::Lines);

      if (divided) {
        northWest->show(target);
        northEast->show(target);
        southWest->show(target);
        southEast->show(target);
      }
    }
};

