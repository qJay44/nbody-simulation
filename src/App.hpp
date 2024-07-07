#pragma once

#include "engine/ParticleSystem.hpp"

class App {
  public:
    ~App();

    void setup();
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
    bool showTimer = false;
    bool showGrid = false;
    bool showFPS = true;

  private:
    void draw();
};

