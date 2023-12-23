#include "myutils.hpp"
#include "engine/ParticleSystem.hpp"
#include "engine/utils/colormap.hpp"
#include <string>

class App {
  sf::RenderWindow window;
  sf::Font genericFont;
  sf::Clock clock;
  sf::Text fpsText;
  sf::Vector2f mousePos;

  sf::RenderTexture backgroundTexture;
  sf::Sprite backgroundSprite;
  sf::Texture circleTexture;
  sf::Shader shader;

  ParticleSystem* particles;
  float dt;
  bool showFPS = true;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "NBody simulation", sf::Style::Close);
    window.setFramerateLimit(90);

    // Font for some test text
    genericFont.loadFromFile("../../src/res/Minecraft rus.ttf");

    // FPS text setup
    fpsText.setString("99");
    fpsText.setFont(genericFont);
    fpsText.setCharacterSize(20);
    fpsText.setOutlineColor(sf::Color(31, 31, 31));
    fpsText.setOutlineThickness(3.f);
    fpsText.setPosition({ WIDTH - fpsText.getLocalBounds().width, 0 });

    // Main canvas setup
    backgroundTexture.create(WIDTH, HEIGHT);
    backgroundSprite.setTexture(backgroundTexture.getTexture());

    // Load circle texture
    circleTexture.loadFromFile("../../src/res/circle.png");
    circleTexture.generateMipmap();
    circleTexture.setSmooth(true);

    shader.loadFromFile("../../src/engine/utils/colormap.frag", sf::Shader::Fragment);
    shader.setUniform("texture", backgroundTexture.getTexture());
    shader.setUniformArray("colormap", colormap, 256);
  }

  void setupProgram() {
    particles = new ParticleSystem(&circleTexture, &genericFont);
  }

  void draw() {
    backgroundTexture.draw(*particles);
    window.draw(backgroundSprite, &shader);

    if (showFPS) {
      int fps = static_cast<int>(1.f / dt);
      fpsText.setString(std::to_string(fps));
      window.draw(fpsText);
    }
  }

  public:
    App() {}

    ~App() {
      delete particles;
    }

    void setup() {
      setupSFML();
      setupProgram();
    }

    void run() {
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
                particles->toggleGrid();
                break;
              case sf::Keyboard::Key::R:
                delete particles; particles = new ParticleSystem(&circleTexture, &genericFont);
                break;
              case sf::Keyboard::Key::F:
                showFPS = !showFPS;
                break;
              case sf::Keyboard::Key::T:
                particles->toggleTimer();
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
};

