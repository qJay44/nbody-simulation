#pragma once

#include "quadtree.hpp"
#include "opencl-bruteforce/RuntimeOpenCL.hpp"

class ParticleSystem : public sf::Drawable, public sf::Transformable {
  public:
    ParticleSystem(const sf::Texture* texture, const sf::Font* font);
    ~ParticleSystem();

    [[nodiscard]] const sf::Text& getTimerText() const;

    void toggleGpuMode();
    void update(float dt);
    void drawGrid(sf::RenderTarget& target, const uint8_t& limit = QUAD_TREE_MAX_DEPTH) const;

  private:
    const sf::Texture* texture;
    const sf::Vector2f center{WIDTH * 0.5f, HEIGHT * 0.5f};

    std::vector<Particle> particles;
    sf::VertexArray vertices{sf::Quads, INITIAL_PARTICLES * 4};
    qt::Rectangle initBoundary{center.x, center.y, center.x, center.y};
    qt::Node* qt = nullptr;
    ThreadPool tp;

    RuntimeOpenCL* gpuCalc = nullptr;
    bool useGpu = false;

  private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void updateQuadTree();
    void updateAttraction();
    void updateAttractionThreaded(int begin, int end);
    void updateAttractionGpu(float dt);
    void updateParticles(float dt);
    void updateVertices();
};

