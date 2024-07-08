#include <cmath>

#include "Particle.hpp"

Particle::Particle(sf::Vector2f position, float mass, float radius, sf::Color color)
  : position(position), mass(mass), radius(radius) {

  vertices[0].texCoords = {0.f, 0.f};
  vertices[1].texCoords = {CIRCLE_TEXTURE_SIZE, 0.f};
  vertices[2].texCoords = {CIRCLE_TEXTURE_SIZE, CIRCLE_TEXTURE_SIZE};
  vertices[3].texCoords = {0.f, CIRCLE_TEXTURE_SIZE};

  vertices[0].color = color;
  vertices[1].color = color;
  vertices[2].color = color;
  vertices[3].color = color;
}

const sf::Vector2f& Particle::getPosition() const    { return position; }
const sf::Vector2f& Particle::getVelocity() const    { return velocity; }
const float& Particle::getMass() const               { return mass;     }
const sf::VertexArray& Particle::getVertices() const { return vertices; }

void Particle::update(float dt) {
  updatePosition(dt);
  updatePositionVertices();
}

void Particle::update(sf::Vector2f pos) {
  updatePosition(pos);
  updatePositionVertices();
}

void Particle::attractTo(const sf::Vector2f& attractorPos, const float& attractorMass) {
  sf::Vector2f v = attractorPos - position;
  float magSq = v.x * v.x + v.y * v.y;
  float mag = std::sqrt(magSq);

  acceleration += attractorMass / (magSq * mag + ZERO_DIVISION_PREVENT_VALUE) * v;
}

void Particle::updatePosition(float dt) {
  position += velocity * dt;
  velocity += acceleration * dt;
  acceleration = {0.f, 0.f};
}

void Particle::updatePosition(sf::Vector2f pos) {
  position = pos;
}

void Particle::updatePositionVertices() {
  vertices[0].position = position + sf::Vector2f{-radius, -radius};
  vertices[1].position = position + sf::Vector2f{ radius, -radius};
  vertices[2].position = position + sf::Vector2f{ radius,  radius};
  vertices[3].position = position + sf::Vector2f{-radius,  radius};
}

