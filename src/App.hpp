#pragma once

#include "engine/ParticleSystem.hpp"

class App {
  public:
    App();
    ~App();

    void run();

  private:
    sf::RenderWindow window;
    sf::Font genericFont;
    sf::Clock clock;
    sf::Text fpsText;
    sf::Vector2f mousePos;

    sf::RenderTexture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture circleTexture;
    sf::Shader shader;

    ParticleSystem* particles = nullptr;
    float dt;
    bool showGrid = false;
    bool showFPS = true;

  private:
    void draw();
};

