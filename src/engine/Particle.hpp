#pragma once

class Particle {
  public:
    Particle(sf::Vector2f position, float mass = INITIAL_MASS, float radius = RADIUS, sf::Color color = {30, 30, 30, 150});

    [[nodiscard]] const sf::Vector2f& getPosition() const;
    [[nodiscard]] const sf::Vector2f& getVelocity() const;
    [[nodiscard]] const float& getMass() const;
    [[nodiscard]] const sf::VertexArray& getVertices() const;

    void update(const float& dt);
    void update(float x, float y);

    void attract(const sf::Vector2f& attractorPos, const float& attractorMass);

  private:
    float mass;
    float radius;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    sf::VertexArray vertices{sf::Quads, 4};

  private:
    void updatePosition(const float& dt);
    void updatePosition(float x, float y);
    void updatePositionVertices();
};

