#pragma once

class Particle {
  public:
    Particle(sf::Vector2f position, float mass = INITIAL_MASS, float radius = RADIUS, sf::Color color = {30, 30, 30});

    [[nodiscard]] const sf::Vector2f& getPosition() const;
    [[nodiscard]] const sf::Vector2f& getVelocity() const;
    [[nodiscard]] const float& getMass() const;
    [[nodiscard]] const sf::VertexArray& getVertices() const;

    void update(float dt);
    void update(sf::Vector2f pos);

    void attractTo(const sf::Vector2f& attractorPos, const float& attractorMass);

  private:
    float mass;
    float radius;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    sf::VertexArray vertices{sf::Quads, 4};

  private:
    void updatePosition(float dt);
    void updatePosition(sf::Vector2f pos);
    void updatePositionVertices();
};

