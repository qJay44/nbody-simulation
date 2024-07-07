#include "ParticleSystem.hpp"

#define _USE_MATH_DEFINES
#include <cmath>

ParticleSystem::ParticleSystem(const sf::Texture* texture, const sf::Font* font) : texture(texture) {
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
  initBoundary = new qt::Rectangle(center.x, center.y, center.x, center.y);
  qt = new qt::Node(*initBoundary);

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

ParticleSystem::~ParticleSystem() {
  delete initBoundary;
  delete qt;
  delete gpuCalc;
  tp.stop();
}

const sf::Text& ParticleSystem::getTimerText() const {
  return timerText;
}

void ParticleSystem::toggleGpuMode() {
  useGpu = !useGpu;
}

void ParticleSystem::update(const float& dt) {
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

void ParticleSystem::drawGrid(sf::RenderTarget& target, const uint8_t& limit) const {
  qt->show(target, limit);
}

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.transform *= getTransform();
  states.texture = texture;
  states.blendMode = sf::BlendAdd;

  target.draw(vertices, states);
}

void ParticleSystem::updateQuadTree() {
  timer.restart();

  delete qt; qt = new qt::Node(*initBoundary);
  for (const Particle& particle : particles)
    qt->insert(&particle);

  t_qt = timer.restart().asSeconds();
}

void ParticleSystem::updateAttraction() {
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

void ParticleSystem::updateAttractionThreaded(int begin, int end) {
  for (int i = begin; i < end; i++)
    qt->solveAttraction(&particles[i]);
}

void ParticleSystem::updateAttractionGpu(const float& dt) {
  timer.restart();

  const cl_float4* clParticlesPtr = gpuCalc->getComputedParticlesPtr();
  gpuCalc->run(dt);

  for (int i = 0; i < particles.size(); i++)
    particles[i].update(clParticlesPtr[i].x, clParticlesPtr[i].y);

  t_attraction = timer.restart().asSeconds();
}

void ParticleSystem::updateParticles(const float& dt) {
  timer.restart();

  for (Particle& p : particles)
    p.update(dt);

  t_particles = timer.restart().asSeconds();
}

void ParticleSystem::updateVertices() {
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

void ParticleSystem::updateTimerText() {
  std::string mode = useGpu ? "GPU mode" : "CPU mode";
  std::string text("Update time ");
  text += "(" + mode + "):\n\n";
  text += "quad tree:  "  + std::to_string(t_qt) + "\n";
  text += "attraction: "  + std::to_string(t_attraction) + "\n";
  text += "particles:   " + std::to_string(t_particles) + "\n";
  text += "vertices:    " + std::to_string(t_vertices);

  timerText.setString(text);
}

