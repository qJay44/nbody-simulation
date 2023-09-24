#include "Particle.hpp"
#include "quadtree.hpp"
#include "colormap.hpp"
#include <algorithm>

class ParticleSystem : public sf::Drawable, public sf::Transformable {
  std::vector<Particle> particles;
  Rectangle<int>* initBoundary;
  QuadTree<int>* qt;
  sf::VertexArray vertices;
  bool showGrid = false;

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (showGrid)
      qt->show(target);

    states.transform *= getTransform();
    states.texture = NULL;
    target.draw(vertices, states);
  }

  void updateQuadTree() {
    delete qt;
    qt = new QuadTree<int>(*initBoundary, 40);

    for (const Particle& particle : particles) {
      const sf::Vector2f& pos = particle.position;
      qt->insert(Point<int>{pos.x, pos.y, &particle.index});
    }
  }

  void updateAttraction() {
    for (Particle& p1 : particles) {
      const sf::Vector2f& pos = p1.position;
      Rectangle<int> range{pos.x, pos.y, 200.f, 200.f};

      std::vector<Point<int>> found;
      qt->query(found, range);

      for (const Point<int>& point : found) {
        Particle& p2 = particles[*point.data];
        if (&p1 != &p2)
          p1.attract(p2);
      }
    }
  }

  void updateParticles() {
    for (int i = 0; i < particles.size(); i++) {
      particles[i].update();
      vertices[i].position = particles[i].position;
    }
  }

  void updateColors() {
    constexpr int cellSize = 100;
    constexpr float cellWidth = cellSize / 2.f;  // From the center
    constexpr float cellHeight = cellSize / 2.f; // From the center

    for (int x = 0; x < WIDTH / cellSize; x++) {
      for (int y = 0; y < HEIGHT / cellSize; y++) {
        Rectangle<int> range{x * cellSize + cellWidth, y * cellSize + cellHeight, cellWidth, cellHeight};
        std::vector<Point<int>> found;
        qt->query(found, range);

        for (Point<int>& p : found)
          vertices[*p.data].color = colormap[found.size() / particles.size() * (colormap.size() - 1)];
      }
    }
  }

  public:
    ParticleSystem(unsigned int count) : particles(count), vertices(sf::Points, count) {
      // Setup particles
      for (int i = 0; i < count; i++) {
        sf::Vector2f pos = {random(0.f, WIDTH * 1.f), random(0.f, HEIGHT * 1.f)};
        particles[i].position = pos;
        particles[i].index = i;
        vertices[i].position = pos;
      }

      // Setup quad tree
      initBoundary = new Rectangle<int>{WIDTH / 2.f, HEIGHT / 2.f, WIDTH / 2.f, HEIGHT / 2.f};
      qt = new QuadTree<int>(*initBoundary, 4);
    }

    ~ParticleSystem() {
      delete initBoundary;
      delete qt;
    }

    void addParticle(sf::Vector2f pos) {
      particles.push_back(Particle{pos});
      vertices.append(sf::Vertex{pos});
    }

    void toggleGrid() {
      showGrid = !showGrid;
    }

    void update() {
      updateQuadTree();
      updateAttraction();
      updateParticles();
      updateColors();
    }
};

