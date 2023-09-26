#include "myutils.hpp"
#include "engine/ParticleSystem.hpp"

class App {
  sf::RenderWindow window;
  sf::Font genericFont;
  sf::Clock clock;
  sf::Text fpsText;
  sf::Vector2f mouse;

  ParticleSystem* particles;

  void setupSFML() {
    // Setup main window
    window.create(sf::VideoMode(WIDTH, HEIGHT), "Template text", sf::Style::Close);
    window.setFramerateLimit(90);

    // Font for some test text
    genericFont.loadFromFile("../../src/fonts/Minecraft rus.ttf");

    // FPS text setup
    fpsText.setString("99");
    fpsText.setFont(genericFont);
    fpsText.setCharacterSize(20);
    fpsText.setOutlineColor(sf::Color(31, 31, 31));
    fpsText.setOutlineThickness(3.f);
    fpsText.setPosition({ WIDTH - fpsText.getLocalBounds().width, 0 });
  }

  void setupProgram() {
    particles = new ParticleSystem(1000);
  }

  void draw(float dt) {
    window.draw(*particles);

    int fps = static_cast<int>(1.f / dt);
    fpsText.setString(std::to_string(fps));
    window.draw(fpsText);
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
              default:
                break;
            }

          if (event.type == sf::Event::MouseMoved)
            mouse = sf::Vector2f{sf::Mouse::getPosition(window)};

          if (event.type == sf::Event::MouseButtonReleased)
            if (event.mouseButton.button == sf::Mouse::Left)
              particles->addParticle(sf::Vector2f{sf::Mouse::getPosition(window)});
        }

        window.clear();
        particles->update();

        draw(clock.restart().asSeconds());

        window.display();
      }
    }
};

