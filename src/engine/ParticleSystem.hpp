#include "utils/ThreadPool.h"
#include "quadtree.hpp"
#include <algorithm>

class ParticleSystem : public sf::Drawable, public sf::Transformable {
  const sf::Texture* texture;
  std::vector<Particle> particles;
  sf::VertexArray vertices{sf::Quads, INITIAL_PARTICLES * 4};
  Rectangle* initBoundary = nullptr;
  QuadTree* qt = nullptr;
  ThreadPool tp;

  bool showGrid = false;

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (showGrid)
      qt->show(target);

    states.transform *= getTransform();
    states.texture = texture;
    target.draw(vertices, states);
  }

  void updateVertices() {
    for (int i = 0; i < particles.size(); i++) {
      const sf::VertexArray& va = particles[i].getVertices();
      vertices[(i << 2) + 0] = va[0];
      vertices[(i << 2) + 1] = va[1];
      vertices[(i << 2) + 2] = va[2];
      vertices[(i << 2) + 3] = va[3];
    }
  }

  void updateQuadTree() {
    delete qt; qt = new QuadTree(*initBoundary);
    for (const Particle& particle : particles)
      qt->insert(&particle);
  }

  void updateAttractionThreaded(uint32_t begin, uint32_t end) {
    for (uint32_t i = begin; i < end; i++)
      qt->solveAttraction(&particles[i]);
  }

  void updateAttraction() {
    static const uint8_t size = tp.size();
    const uint32_t slice = particles.size() / size;
    for (int i = 0; i < size; i++) {
      uint32_t begin = i * slice;
      uint32_t end = begin + slice;
      tp.queueJob([this, begin, end] () {updateAttractionThreaded(begin, end);});
    }
    tp.waitForCompletion();
  }

  void updateParticles() {
    for (int i = 0; i < particles.size(); i++)
      particles[i].update();
  }

  public:
    ParticleSystem(const sf::Texture* texture)
      : texture(texture) {
      // Setup particles
      for (int i = 0; i < INITIAL_PARTICLES; i++) {
        sf::Vector2f pos = {random(0.f, WIDTH * 1.f), random(0.f, HEIGHT * 1.f)};
        particles.push_back(Particle(pos, texture));
      }

      // Setup quad tree
      initBoundary = new Rectangle{WIDTH / 2.f, HEIGHT / 2.f, WIDTH / 2.f, HEIGHT / 2.f};
      qt = new QuadTree(*initBoundary);

      tp.start();
    }

    ~ParticleSystem() {
      delete initBoundary;
      delete qt;
    }

    void addParticle(sf::Vector2f pos) {
      particles.push_back(Particle(pos, texture, 1e13f, 12.f));
      vertices.resize(particles.size() * 4);
    }

    void toggleGrid() { showGrid = !showGrid; }

    void update() {
      updateQuadTree();
      updateAttraction();
      updateParticles();
      updateVertices();
    }
};

