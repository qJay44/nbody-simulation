#include "ParticleSystem.hpp"
#include "Spawner.hpp"

ParticleSystem::ParticleSystem(const sf::Texture* texture) : texture(texture) {
  Spawner::spiral(particles, center);

  qt = new qt::Node(initBoundary);
  gpuCalc = new RuntimeOpenCL(particles);

  tp.start();
}

ParticleSystem::~ParticleSystem() {
  delete qt;
  delete gpuCalc;
  tp.stop();
}

void ParticleSystem::toggleGpuMode() {
  useGpu = !useGpu;
}

void ParticleSystem::update(float dt) {
  if (useGpu) {
    updateAttractionGpu(dt);
    updateVertices();
  } else {
    updateQuadTree();
    updateAttraction();
    updateParticles(dt);
    updateVertices();
  }
}

void ParticleSystem::drawGrid(sf::RenderTarget& target, const uint32_t& limit) const {
  qt->show(target, limit);
}

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
  states.transform *= getTransform();
  states.texture = texture;
  states.blendMode = sf::BlendAdd;

  target.draw(vertices, states);
}

void ParticleSystem::updateQuadTree() {
  delete qt; qt = new qt::Node(initBoundary);
  for (Particle& particle : particles)
    qt->insert(&particle);
}

void ParticleSystem::updateAttraction() {
  int slice = particles.size() / tp.size();
  for (int i = 0; i < tp.size(); i++) {
    int begin = i * slice;
    int end = begin + slice;

    tp.queueJob([this, begin, end] {updateAttractionThreaded(begin, end);});
  }
  tp.waitForCompletion();
}

void ParticleSystem::updateAttractionThreaded(int begin, int end) {
  for (int i = begin; i < end; i++)
    qt->solveAttraction(&particles[i]);
}

void ParticleSystem::updateAttractionGpu(float dt) {
  gpuCalc->run(dt);
  const cl_float4* clParticlesPtr = gpuCalc->getComputedParticlesPtr();

  for (int i = 0; i < particles.size(); i++)
    particles[i].update({clParticlesPtr[i].x, clParticlesPtr[i].y});
}

void ParticleSystem::updateParticles(float dt) {
  for (Particle& p : particles)
    p.update(dt);
}

void ParticleSystem::updateVertices() {
  for (int i = 0; i < particles.size(); i++) {
    const sf::VertexArray& va = particles[i].getVertices();
    int ii = i << 2;
    vertices[ii + 0] = va[0];
    vertices[ii + 1] = va[1];
    vertices[ii + 2] = va[2];
    vertices[ii + 3] = va[3];
  }
}

