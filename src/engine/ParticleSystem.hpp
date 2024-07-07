#pragma once

#include "quadtree.hpp"
#include "opencl-bruteforce/RuntimeOpenCL.hpp"

class ParticleSystem : public sf::Drawable, public sf::Transformable {
  public:
    ParticleSystem(const sf::Texture* texture, const sf::Font* font);
    ~ParticleSystem();

    [[nodiscard]] const sf::Text& getTimerText() const;

    void toggleGpuMode();
    void update(const float& dt);
    void drawGrid(sf::RenderTarget& target, const uint8_t& limit = QUAD_TREE_MAX_DEPTH) const;

  private:
    const sf::Texture* texture;
    std::vector<Particle> particles;
    sf::VertexArray vertices{sf::Quads, INITIAL_PARTICLES * 4};
    qt::Rectangle* initBoundary = nullptr;
    qt::Node* qt = nullptr;
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

  private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void updateQuadTree();
    void updateAttraction();
    void updateAttractionThreaded(int begin, int end);
    void updateAttractionGpu(const float& dt);
    void updateParticles(const float& dt);
    void updateVertices();
    void updateTimerText();
};

