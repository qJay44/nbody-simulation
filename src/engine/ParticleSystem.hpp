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
  QuadTree* qt = nullptr;
  ThreadPool tp;

  RuntimeOpenCL* gpuCalc = nullptr;

  bool showTimer = false;
  bool showGrid = false;
  bool useGpu = true;

  // Functions time execution in seconds
  sf::Text timerText;
  sf::Clock timer;
  float t_qt;
  float t_attraction;
  float t_particles;
  float t_vertices;

  virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
    if (showGrid)
      qt->show(target);

    states.transform *= getTransform();
    states.texture = texture;
    states.blendMode = sf::BlendAdd;

    target.draw(vertices, states);

    if (showTimer)
      target.draw(timerText);
  }

  void updateQuadTree() {
    timer.restart();

    delete qt; qt = new QuadTree(*initBoundary);
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

    static const cl_float4* clParticlesPtr = gpuCalc->getComputedParticlesPtr();
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

    // TODO: Update particles in gpu memory?
    void addParticle(sf::Vector2f pos) {
      return;
      particles.push_back(Particle(pos, 200.f, 5.f));
      vertices.resize(particles.size() * 4);
    }

    void toggleGrid()     { showGrid = !showGrid; }
    void toggleTimer()    { showTimer = !showTimer; }
    void toggleGpuMode()  { useGpu = !useGpu; }

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
};

