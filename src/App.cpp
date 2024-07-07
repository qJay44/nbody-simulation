#include "App.hpp"

App::~App() {
  if (particles) delete particles;
}

void App::setup() {
  // Window
  window.create(sf::VideoMode(WIDTH, HEIGHT), "NBody simulation", sf::Style::Close);
  window.setFramerateLimit(90);

  // Font for some test text
  genericFont.loadFromFile("res/fonts/Minecraft rus.ttf");

  // FPS text
  fpsText.setString("99");
  fpsText.setFont(genericFont);
  fpsText.setCharacterSize(20);
  fpsText.setOutlineColor(sf::Color(31, 31, 31));
  fpsText.setOutlineThickness(3.f);
  fpsText.setPosition({WIDTH - fpsText.getLocalBounds().width, 0});

  // Main canvas
  backgroundTexture.create(WIDTH, HEIGHT);
  backgroundSprite.setTexture(backgroundTexture.getTexture());

  // Load circle texture
  circleTexture.loadFromFile("res/circle.png");
  circleTexture.generateMipmap();
  circleTexture.setSmooth(true);

  shader.loadFromFile("res/shaders/intencity.frag", sf::Shader::Fragment);
  shader.setUniform("texture", backgroundTexture.getTexture());
  shader.setUniformArray("colormap", colormaps::inferno, 256);

  particles = new ParticleSystem(&circleTexture, &genericFont);
}

void App::run() {
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::KeyReleased)
        switch (event.key.code) {
          case sf::Keyboard::Key::Q:
            window.close();
            break;
          case sf::Keyboard::Key::G:
            showGrid = !showGrid;
            break;
          case sf::Keyboard::Key::R:
            delete particles; particles = new ParticleSystem(&circleTexture, &genericFont);
            break;
          case sf::Keyboard::Key::F:
            showFPS = !showFPS;
            break;
          case sf::Keyboard::Key::T:
            showTimer = !showTimer;
            break;
          case sf::Keyboard::Key::A:
            particles->toggleGpuMode();
            break;
          default:
            break;
        }

      if (event.type == sf::Event::MouseMoved)
        mousePos = sf::Vector2f{sf::Mouse::getPosition(window)};
    }

    dt = clock.restart().asSeconds();
    particles->update(dt);

    backgroundTexture.clear();
    backgroundTexture.display();

    window.clear();
    draw();
    window.display();
  }
}

void App::draw() {
  backgroundTexture.draw(*particles);
  window.draw(backgroundSprite, &shader);

  if (showGrid)
    particles->drawGrid(window, 7);

  if (showTimer)
    window.draw(particles->getTimerText(), sf::RenderStates());

  if (showFPS) {
    int fps = static_cast<int>(1.f / dt);
    fpsText.setString(std::to_string(fps));
    window.draw(fpsText);
  }
}

