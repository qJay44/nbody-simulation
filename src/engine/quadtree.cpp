// http://arborjs.org/docs/barnes-hut

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <format>
#include <stdexcept>

#include "quadtree.hpp"

using namespace qt;

uint32_t Node::maxDepth = 0;

inline float mag(const sf::Vector2f& v1, const sf::Vector2f& v2) {
  sf::Vector2f v = v1 - v2;
  return sqrtf(v.x * v.x + v.y * v.y);
}

inline bool isFar(float s, float d) {
  return s / (d + ZERO_DIVISION_PREVENT_VALUE) < QUAD_TREE_THETA;
}

Rectangle::Rectangle(float x, float y, float w, float h)
  : x(x), y(y), w(w), h(h),
    top(y - h), right(x + w), bottom(y + h), left(x - w) {}

bool Rectangle::contains(const Particle* p) const {
  const float& px = p->getPosition().x;
  const float& py = p->getPosition().y;

  return (
    px >= left  &&
    px <= right &&
    py >= top   &&
    py <= bottom
  );
}

bool Rectangle::intersects(const Rectangle& r) const {
  return !(
    top    > r.bottom ||
    right  < r.left   ||
    left   > r.right  ||
    bottom < r.top
  );
}

Node::Node(Rectangle boundary, uint32_t depth)
  : boundary(boundary), depth(depth) {
  gravity = {{boundary.x, boundary.y}, 0.f};
  maxDepth = std::max(maxDepth, depth);
}

Node::~Node() {
  delete northWest;
  delete northEast;
  delete southWest;
  delete southEast;
}

void Node::printMaxReachedDepth() {
  printf("Maximum reached depth: %d\n", maxDepth);
}

bool Node::insert(const Particle* p) {
  // Check if particle is within boundaries
  if (!boundary.contains(p)) return false;

  // 1. If this node does not reached the container limit or exceeded the depth limit, put the new particle here
  // REVIEW: Possible without depth limit?
  if (container.size() < QUAD_TREE_CONTAINER_LIMIT || depth >= QUAD_TREE_MAX_DEPTH) {

    // 2. If this node is an internal (divided) node, update the gravity field.
    // Recursively insert the particles in the appropriate quadrant
    if (northWest) {
      gravity.update(p->getPosition(), p->getMass());

      return
        northWest->insert(p) ||
        northEast->insert(p) ||
        southWest->insert(p) ||
        southEast->insert(p);

    } else {
      container.push_back(p);
      return true;
    }

  // 3. If this node is an external node (which already containing other particle),
  // subdivide the region and recursively insert the particles into the appropriate quadrants
  } else {
    subdivide(p);
    return true;
  }

  throw std::runtime_error(std::format("A particle wasn't inserted, depth: {}\n", depth));
}

void Node::solveAttraction(Particle* p2) {
  // 1. If this node is an external,
  // try to calculate the force on the particle by other particles (if have any and not the same).
  if (!northWest) {
    for (const Particle* p1 : container)
      if (p2 != p1)
        p2->attractTo(p1->getPosition(), p1->getMass());

  // 2. Otherwise, calculate the ration s/d. If s/d < θ,
  // treat this internal node as a single body, and calculate the force for the particle.
  } else if (isFar(boundary.w * 2.f, mag(p2->getPosition(), gravity.center)))
    p2->attractTo(gravity.center, gravity.mass);

  // 3. Otherwise, run the procedure recursively for other nodes
  else {
    northWest->solveAttraction(p2);
    northEast->solveAttraction(p2);
    southWest->solveAttraction(p2);
    southEast->solveAttraction(p2);
  }
}

void Node::show(sf::RenderTarget& target, const uint32_t& depthLimit) {
  static const sf::Color color = sf::Color(30, 30, 30);

  sf::VertexArray rect(sf::LinesStrip, 4);
  rect[0] = sf::Vertex({boundary.left , boundary.top}, color);
  rect[1] = sf::Vertex({boundary.right, boundary.top}, color);
  rect[2] = sf::Vertex({boundary.right, boundary.bottom}, color);
  rect[3] = sf::Vertex({boundary.left , boundary.bottom}, color);
  target.draw(rect);

  if (northWest && depth <= depthLimit) {
    northWest->show(target, depthLimit);
    northEast->show(target, depthLimit);
    southWest->show(target, depthLimit);
    southEast->show(target, depthLimit);
  }
}

void Node::Gravity::update(const sf::Vector2f& pos, const float& m2) {
  float& m1 = mass;
  float& x = center.x;
  float& y = center.y;
  float m = m1 + m2;
  assert(m != 0.f);

  x = (x * m1 + pos.x * m2) / m;
  y = (y * m1 + pos.y * m2) / m;
  m1 = m;
}

void Node::subdivide(const Particle* p2) {
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

  // Reallocate this (node) particles
  for (const Particle* p1 : container) {
    northWest->insert(p1) ||
    northEast->insert(p1) ||
    southWest->insert(p1) ||
    southEast->insert(p1);
    gravity.update(p1->getPosition(), p1->getMass());
  }

  // Insert the new particle
  northWest->insert(p2) ||
  northEast->insert(p2) ||
  southWest->insert(p2) ||
  southEast->insert(p2);

  gravity.update(p2->getPosition(), p2->getMass());

  container.clear();
}

