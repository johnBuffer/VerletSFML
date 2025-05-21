#include <iostream>
#include <SFML/Graphics.hpp>
#include "solver.hpp"
#include "renderer.hpp"
#include "utils/number_generator.hpp"
#include "utils/math.hpp"


static sf::Color getRainbow(float t)
{
    const float r = sin(t);
    const float g = sin(t + 0.33f * 2.0f * Math::PI);
    const float b = sin(t + 0.66f * 2.0f * Math::PI);
    return {static_cast<uint8_t>(255.0f * r * r),
            static_cast<uint8_t>(255.0f * g * g),
            static_cast<uint8_t>(255.0f * b * b)};
}


int32_t main(int32_t, char*[])
{
    // Create window
    constexpr int32_t window_width  = 1000;
    constexpr int32_t window_height = 1000;

    sf::ContextSettings settings;
    settings.antialiasingLevel = 1;
    sf::RenderWindow window(sf::VideoMode(window_width, window_height), "Verlet", sf::Style::Default, settings);
    const uint32_t frame_rate = 60;
    const float dt = 1.0f / static_cast<float>(frame_rate);
    window.setFramerateLimit(frame_rate);
    bool unlock_frame_rate = false;
    float time = 0.0f;

    Solver   solver;
    Renderer renderer{window};

    // Solver configuration
    float constexpr constraint_radius{450.0f};
    sf::Vector2f const constraint_position{static_cast<float>(window_width) * 0.5f, static_cast<float>(window_height) * 0.5f};
    solver.setConstraint(constraint_position, constraint_radius);
    solver.setSubStepsCount(8);
    solver.setSimulationUpdateRate(frame_rate);

    // Set simulation attributes
    const float        object_spawn_delay    = 0.025f;
    const float        object_spawn_speed    = 1200.0f;
    const sf::Vector2f object_spawn_position = {500.0f, 200.0f};
    const float        object_min_radius     = 10.0f;
    const float        object_max_radius     = 10.0f;
    const uint32_t     max_objects_count     = 1850;
    const float        max_angle             = 1.0f;

    sf::Image image;
    image.loadFromFile("img.png");
    auto const mapObjectPositionToPxlPosition = [&](sf::Vector2f position) -> sf::Vector2i {
        sf::Vector2f const image_size{image.getSize()};
        sf::Vector2f const scale_coef{image_size / (2.0f * constraint_radius)};
        sf::Vector2f const relative_position{position - constraint_position + sf::Vector2f{constraint_radius, constraint_radius}};
        return sf::Vector2i(sf::Vector2f{relative_position.x * scale_coef.x, relative_position.y * scale_coef.y});
    };
    std::vector<sf::Color> object_color;

    // Main loop
    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::S) {
                    unlock_frame_rate = !unlock_frame_rate;
                    if (unlock_frame_rate) {
                        window.setFramerateLimit(0);
                    } else {
                        window.setFramerateLimit(frame_rate);
                    }
                } else if (event.key.code == sf::Keyboard::L) {
                    object_color.clear();
                    // Loads the image and applies it to objects
                    for (uint32_t i = 0; i < solver.getObjectsCount(); ++i) {
                        auto& obj = solver.getObject(i);
                        sf::Vector2i const pxl_position{mapObjectPositionToPxlPosition(obj.position)};
                        obj.color = image.getPixel(pxl_position.x, pxl_position.y);
                        object_color.push_back(obj.color);
                    }
                } else if (event.key.code == sf::Keyboard::R) {
                    // Resets the simulation
                    solver.reset();
                    time = 0.0f;
                    RNGf::reset();
                }
            }
        }

        uint64_t const object_count{solver.getObjectsCount()};
        if (object_count < max_objects_count && time >= object_spawn_delay) {
            auto&       object = solver.addObject(object_spawn_position, RNGf::getRange(object_min_radius, object_max_radius));
            const float t      = solver.getTime();
            const float angle  = max_angle * sin(t) + Math::PI * 0.5f;
            solver.setObjectVelocity(object, object_spawn_speed * sf::Vector2f{cos(angle), sin(angle)});
            if (object_count < object_color.size()) {
                object.color = object_color[object_count];
            } else {
                object.color = sf::Color::White;
            }
            time = 0.0f;
        }

        solver.update();
        window.clear(sf::Color::White);
        renderer.render(solver);
		window.display();

        time += dt;
    }

    return 0;
}
