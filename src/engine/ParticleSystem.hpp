#include "quadtree.hpp"
#include <algorithm>

class ParticleSystem : public sf::Drawable, public sf::Transformable {
  const sf::Texture* texture;
  std::vector<Particle> particles;
  sf::VertexArray vertices{sf::Quads, INITIAL_PARTICLES * 4};
  Rectangle* initBoundary = nullptr;
  QuadTree* qt = nullptr;

  bool showGrid = false;

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (showGrid)
      qt->show(target);

    states.transform *= getTransform();
    states.texture = texture;
    states.blendMode = sf::BlendAdd;
    target.draw(vertices, states);
  }

  void updateVertices() {
    for (int i = 0; i < particles.size(); i++) {
      const sf::VertexArray& va = particles[i].getVertices();
      int ii = i << 2;
      vertices[ii + 0] = va[0];
      vertices[ii + 1] = va[1];
      vertices[ii + 2] = va[2];
      vertices[ii + 3] = va[3];
    }
  }

  void updateQuadTree() {
    delete qt; qt = new QuadTree(*initBoundary);
    for (const Particle& particle : particles)
      qt->insert(&particle);
  }

  void updateAttraction() {
    for (Particle& particle : particles)
      qt->solveAttraction(&particle);
  }

  void updateParticles(const float& dt) {
    for (int i = 0; i < particles.size(); i++)
      particles[i].update(dt);
  }

  public:
    ParticleSystem(const sf::Texture* texture)
      : texture(texture) {
      // Setup particles
      for (int i = 0; i < INITIAL_PARTICLES; i++) {
        sf::Vector2f pos = {Random::GetFloat(0.f, WIDTH * 1.f), Random::GetFloat(0.f, HEIGHT * 1.f)};
        particles.push_back(Particle(pos));
      }

      // Setup quad tree
      initBoundary = new Rectangle(WIDTH / 2.f, HEIGHT / 2.f, WIDTH / 2.f, HEIGHT / 2.f);
      qt = new QuadTree(*initBoundary);
    }

    ~ParticleSystem() {
      delete initBoundary;
      delete qt;
    }

    void addParticle(sf::Vector2f pos) {
      particles.push_back(Particle(pos, 200.f, 5.f));
      vertices.resize(particles.size() * 4);
    }

    void toggleGrid() { showGrid = !showGrid; }

    void update(const float& dt) {
      updateQuadTree();
      updateAttraction();
      updateParticles(dt);
      updateVertices();
    }
};

