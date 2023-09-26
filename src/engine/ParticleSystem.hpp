#include "quadtree.hpp"
#include "colormap.hpp"
#include <algorithm>

class ParticleSystem : public sf::Drawable, public sf::Transformable {
  std::vector<Particle> particles;
  sf::VertexArray vertices;
  Rectangle* initBoundary = nullptr;
  QuadTree* qt = nullptr;

  bool showGrid = false;

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (showGrid) {
      qt->show(target);
    }

    sf::CircleShape c(3.f);
    c.setPosition(qt->gravityField.center);
    c.setFillColor(sf::Color::Magenta);
    target.draw(c);

    states.transform *= getTransform();
    states.texture = NULL;
    target.draw(vertices, states);
  }

  void updateQuadTree() {
    delete qt;
    qt = new QuadTree(*initBoundary);

    for (const Particle& particle : particles) {
      const sf::Vector2f& pos = particle.position;
      qt->insert(particle);
    }
  }

  void updateAttraction() {
    for (Particle& p : particles) {
      qt->solveAttraction(p);
    }
  }

  void updateParticles() {
    for (int i = 0; i < particles.size(); i++) {
      particles[i].update();
      vertices[i].position = particles[i].position;
    }
  }

  void updateColors() {
    for (int x = 0; x < WIDTH / CELL_SIZE; x++) {
      for (int y = 0; y < HEIGHT / CELL_SIZE; y++) {
        sf::Vector2f middle{x * CELL_SIZE + CELL_WIDTH, y * CELL_SIZE + CELL_HEIGHT};
        Rectangle range(middle.x, middle.y, CELL_WIDTH, CELL_HEIGHT);
        std::vector<Particle> found;
        qt->query(found, range);

        for (const Particle& p : found) {
          /* int index = static_cast<float>(found.size()) / particles.size() * (colormap.size() - 1); */
          /* vertices[*p.data].color = colormap[index]; */
        }
      }
    }
  }

  public:
    ParticleSystem(uint32_t count) : particles(count), vertices(sf::Points, count) {
      // Setup particles
      for (int i = 0; i < count; i++) {
        sf::Vector2f pos = {random(0.f, WIDTH * 1.f), random(0.f, HEIGHT * 1.f)};
        particles[i].position = pos;
        particles[i].index = i;
        vertices[i].position = pos;
      }

      // Setup quad tree
      initBoundary = new Rectangle{WIDTH / 2.f, HEIGHT / 2.f, WIDTH / 2.f, HEIGHT / 2.f};
      qt = new QuadTree(*initBoundary);
    }

    ~ParticleSystem() {
      delete initBoundary;
      delete qt;
    }

    void addParticle(sf::Vector2f pos) {
      particles.push_back(Particle{pos});
      vertices.append(sf::Vertex{pos});
    }

    void toggleGrid() { showGrid = !showGrid; }

    void update() {
      updateQuadTree();
      updateAttraction();
      updateParticles();
      updateColors();
    }
};

