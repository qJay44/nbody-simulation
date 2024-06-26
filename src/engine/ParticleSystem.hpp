#define _USE_MATH_DEFINES
#include <cmath>

#include "quadtree.hpp"
#include "opencl-bruteforce/RuntimeOpenCL.hpp"
#include "utils/ThreadPool.h"

class ParticleSystem : public sf::Drawable, public sf::Transformable {
  const sf::Texture* texture;
  std::vector<Particle> particles;
  sf::VertexArray vertices{sf::Quads, INITIAL_PARTICLES * 4};
  Rectangle* initBoundary = nullptr;
  Node* qt = nullptr;
  ThreadPool tp;

  RuntimeOpenCL* gpuCalc = nullptr;
  bool useGpu = false;

  // Functions time execution in seconds
  sf::Text timerText;
  sf::Clock timer;
  float t_qt;
  float t_attraction;
  float t_particles;
  float t_vertices;

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = texture;
    states.blendMode = sf::BlendAdd;

    target.draw(vertices, states);
  }

  void updateQuadTree() {
    timer.restart();

    delete qt; qt = new Node(*initBoundary);
    for (const Particle& particle : particles)
      qt->insert(&particle);

    t_qt = timer.restart().asSeconds();
  }

  void updateAttraction() {
    timer.restart();

    int slice = particles.size() / tp.size();
    for (int i = 0; i < tp.size(); i++) {
      int begin = i * slice;
      int end = begin + slice;

      tp.queueJob([this, begin, end] { updateAttractionThreaded(begin, end); });
    }
    tp.waitForCompletion();

    t_attraction = timer.restart().asSeconds();
  }

  void updateAttractionThreaded(int begin, int end) {
    for (int i = begin; i < end; i++)
      qt->solveAttraction(&particles[i]);
  }

  void updateAttractionGpu(const float& dt) {
    timer.restart();

    const cl_float4* clParticlesPtr = gpuCalc->getComputedParticlesPtr();
    gpuCalc->run(dt);

    for (int i = 0; i < particles.size(); i++)
      particles[i].update(clParticlesPtr[i].x, clParticlesPtr[i].y);

    t_attraction = timer.restart().asSeconds();
  }

  void updateParticles(const float& dt) {
    timer.restart();

    for (Particle& p : particles)
      p.update(dt);

    t_particles = timer.restart().asSeconds();
  }

  void updateVertices() {
    timer.restart();

    for (int i = 0; i < particles.size(); i++) {
      const sf::VertexArray& va = particles[i].getVertices();
      int ii = i << 2;
      vertices[ii + 0] = va[0];
      vertices[ii + 1] = va[1];
      vertices[ii + 2] = va[2];
      vertices[ii + 3] = va[3];
    }

    t_vertices = timer.restart().asSeconds();
  }

  void updateTimerText() {
    std::string mode = useGpu ? "GPU mode" : "CPU mode";
    std::string text("Update time ");
    text += "(" + mode + "):\n\n";
    text += "quad tree:  "  + std::to_string(t_qt) + "\n";
    text += "attraction: "  + std::to_string(t_attraction) + "\n";
    text += "particles:   " + std::to_string(t_particles) + "\n";
    text += "vertices:    " + std::to_string(t_vertices);

    timerText.setString(text);
  }

  public:
    ParticleSystem(const sf::Texture* texture, const sf::Font* font) : texture(texture) {
      sf::Vector2f center{WIDTH / 2.f, HEIGHT / 2.f};
      double stepRad = (2. * M_PI) / SPIRAL_ARMS;

      for (int i = 0; i < SPIRAL_ARMS; i++) {
        double startRad = i * stepRad;
        for (int j = 0; j < SPIRAL_ARMS_WIDTH; j++) {
          double startArmRad = j * M_PI / SPIRAL_ARMS_WIDTH_VALUE / SPIRAL_ARMS_WIDTH;
          for (int k = 0; k < SPIRAL_ARM_LENGTH; k++) {
            sf::Vector2f pos = center;
            double rad = startRad + startArmRad + k * M_PI / SPIRAL_ARM_TWIST_VALUE;
            pos += sf::Vector2f(cos(rad) * k, sin(rad) * k);
            particles.push_back(Particle(pos));
          }
        }
      }

      // Setup quad tree
      initBoundary = new Rectangle(center.x, center.y, center.x, center.y);
      qt = new Node(*initBoundary);

      // Timer text setup
      timerText.setString("sample long name: 0.00000000");
      timerText.setFont(*font);
      timerText.setCharacterSize(15);
      timerText.setOutlineColor(sf::Color(31, 31, 31));
      timerText.setOutlineThickness(3.f);
      timerText.setLineSpacing(1.25f);
      timerText.setLetterSpacing(2.f);

      gpuCalc = new RuntimeOpenCL(particles);

      tp.start();
    }

    ~ParticleSystem() {
      delete initBoundary;
      delete qt;
      delete gpuCalc;
      tp.stop();
    }

    void toggleGpuMode() { useGpu = !useGpu; }

    void update(const float& dt) {
      if (useGpu) {
        updateAttractionGpu(dt);
        updateVertices();
      } else {
        updateQuadTree();
        updateAttraction();
        updateParticles(dt);
        updateVertices();
      }
      updateTimerText();
    }

    void drawGrid(sf::RenderTarget& target, const uint8_t& limit = QUAD_TREE_MAX_DEPTH) const {
      qt->show(target, limit);
    }

    const sf::Text& getTimerText() const {
      return timerText;
    }
};

