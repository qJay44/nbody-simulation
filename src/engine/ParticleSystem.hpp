#define _USE_MATH_DEFINES
#include <cmath>

#include <algorithm>
#include "quadtree.hpp"
#include "utils/ThreadPool.h"

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
    int slice = particles.size() / tp.size();
    for (int i = 0; i < tp.size(); i++) {
      int begin = i * slice;
      int end = begin + slice;

      tp.queueJob([this, begin, end] { updateAttractionThreaded(begin, end); });
    }

    tp.waitForCompletion();
  }

  void updateAttractionThreaded(int begin, int end) {
    for (int i = begin; i < end; i++)
      qt->solveAttraction(&particles[i]);
  }

  void updateParticles(const float& dt) {
    for (int i = 0; i < particles.size(); i++)
      particles[i].update(dt);
  }

  public:
    ParticleSystem(const sf::Texture* texture) : texture(texture) {
      static bool initialized = false; // Only one particle system should exist
      if (!initialized) {
        sf::Vector2f center{WIDTH / 2.f, HEIGHT / 2.f};

        // Distance between each new arm;
        double newArmArcStep = (2. * M_PI) / SPIRAL_ARMS;

        // Setup particles
        for (int i = 0; i < INITIAL_PARTICLES; i++) {
          sf::Vector2f pos = center;

          // A new arm will be create if current reaches its max length
          int currentArm = i / SPIRAL_ARM_LENGTH;
          double armStartRad = currentArm * newArmArcStep;

          // Calculate position based on the start position and current iteration (particle) adjusted by "SPRIRAL_STEP"
          double posRad = armStartRad + i % SPIRAL_ARM_LENGTH * SPIRAL_STEP;
          sf::Vector2f direction(cos(posRad), sin(posRad));
          pos += direction * (i % SPIRAL_ARM_LENGTH * SPIRAL_OFFSET);

          particles.push_back(Particle(pos));
        }

        // Setup quad tree
        initBoundary = new Rectangle(center.x, center.y, center.x, center.y);
        qt = new QuadTree(*initBoundary);

        tp.start();

        initialized = true;
      }
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

