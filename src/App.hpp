#include "myutils.hpp"
#include "engine/ParticleSystem.hpp"

class App {
  sf::RenderWindow window;
  sf::Font genericFont;
  sf::Clock clock;
  sf::Text fpsText;
  sf::Vector2f mouse;

  sf::RenderTexture backgroundTexture;
  sf::Sprite backgroundSprite;
  sf::Shader shader;
  sf::Texture circleTexture;

  ParticleSystem* particles;
  bool useShader = true;

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

    // Main canvas setup
    backgroundTexture.create(WIDTH, HEIGHT);
    backgroundSprite.setTexture(backgroundTexture.getTexture());

    // Load circle texture
    circleTexture.loadFromFile("../../src/res/circle.png");
    circleTexture.generateMipmap();
    circleTexture.setSmooth(true);

    // Shader setup
    shader.loadFromFile("../../src/shaders/blur.frag", sf::Shader::Fragment);
    shader.setUniform("texture", backgroundTexture.getTexture());
  }

  void setupProgram() {
    particles = new ParticleSystem(&circleTexture);
  }

  void fade() {
    sf::RectangleShape rect({WIDTH, HEIGHT});
    rect.setFillColor({0, 0, 0, 80});
    backgroundTexture.draw(rect);
  }

  void draw(float dt) {
    fade();
    backgroundTexture.draw(*particles);
    window.draw(backgroundSprite);

    // Apply bloom shader
    if (useShader) {
      bool isHorizontal = true;
      for (int i = 0; i < 50; i++) {
        shader.setUniform("isHorizontal", isHorizontal);
        backgroundTexture.draw(backgroundSprite, &shader);
        isHorizontal = !isHorizontal;
      }
      window.draw(backgroundSprite, sf::BlendAdd);
    }

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
              case sf::Keyboard::Key::S:
                useShader = !useShader;
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

        backgroundTexture.display();

        window.clear();

        particles->update();

        draw(clock.restart().asSeconds());

        window.display();
      }
    }
};

