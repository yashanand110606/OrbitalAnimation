#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>

const sf::Vector2f EARTH_CENTER = { 600.f, 450.f };
const float G = 0.2f;
const float EARTH_MASS = 5000.f;
const float J2_STRENGTH = 0.00005f;
const float EARTH_RADIUS = 90.f;          // match drawn earth radius
const float MIN_DIST = 1e-3f;             // avoid divide by zero
const size_t MAX_TRAIL = 3000;
const float MAX_DT = 0.05f;               // clamp timestep for stability

// Lowering this value makes satellites orbit slower (increases orbital period).
// Set to 1.0 for original speed, <1.0 to slow, >1.0 to speed up.
// Increased from 0.5 to 3.0 to make orbital period ~6x shorter (orbits run 6x faster).
const float ORBIT_SPEED_SCALE = 4.0f;

struct Satellite
{
    sf::CircleShape shape;
    sf::Vector2f velocity;
    std::vector<sf::Vertex> trail;
    bool alive = true;
};

static float length(const sf::Vector2f& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

static sf::Vector2f normalize(const sf::Vector2f& v)
{
    float m = length(v);
    if (m <= MIN_DIST) return { 0.f, 0.f };
    return v / m;
}

static float energy(const sf::Vector2f& pos, const sf::Vector2f& vel)
{
    float r = std::max(length(pos), MIN_DIST);
    float KE = 0.5f * (vel.x * vel.x + vel.y * vel.y);
    float PE = -G * EARTH_MASS / r;
    return KE + PE;
}

static std::vector<sf::Vertex> predictOrbit(sf::Vector2f pos, sf::Vector2f vel, float dt = 0.02f, int steps = 400)
{
    std::vector<sf::Vertex> ghost;
    ghost.reserve(steps);

    sf::Vector2f p = pos;
    sf::Vector2f v = vel;

    for (int i = 0; i < steps; ++i)
    {
        sf::Vector2f toEarth = -p;
        float dist = length(toEarth);
        if (dist <= EARTH_RADIUS) break; // stop prediction when intersecting Earth

        sf::Vector2f dir = normalize(toEarth);

        float accel = G * EARTH_MASS / (dist * dist + MIN_DIST); // protect divide by zero
        sf::Vector2f a = dir * accel;

        // simple J2 fake perturbation drift (kept as a small hack)
        sf::Vector2f tangent = { -dir.y, dir.x };
        a += tangent * J2_STRENGTH * dist;

        // integrate with dt (semi-explicit Euler)
        v += a * dt;
        p += v * dt;

        ghost.emplace_back(p, sf::Color(200, 200, 255, 120));
    }

    return ghost;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1200,900 }), "INSANE Orbital Simulator");
    window.setFramerateLimit(60);

    sf::View view = window.getDefaultView();
    view.setCenter({ 600.f, 450.f });


    sf::CircleShape earth(EARTH_RADIUS);
    earth.setFillColor(sf::Color(60, 120, 255));
    earth.setOrigin({ EARTH_RADIUS, EARTH_RADIUS });
    earth.setPosition({ 600.f,450.f });

    std::vector<Satellite> sats;
    sats.reserve(16);

    // starter satellite
    {
        Satellite s;
        s.shape = sf::CircleShape(6.f);
        s.shape.setFillColor(sf::Color::Red);
        s.shape.setOrigin({ 6,6 });
        s.shape.setPosition({ 350,0 });
        // apply speed scale to lengthen/shorten orbital period
        s.velocity = { 0, std::sqrt(G * EARTH_MASS / 350.f) * ORBIT_SPEED_SCALE };
        s.trail.reserve(512);
        sats.push_back(std::move(s));
    }

    sf::Clock clock;
    int energyCounter = 0;

    while (window.isOpen())
    {
        // compute delta time and clamp for stability
        float dt = clock.restart().asSeconds();
        if (dt <= 0.f) dt = 1.f / 60.f;
        dt = std::min(dt, MAX_DT);

        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Zoom
            if (auto wheel = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                if (wheel->delta > 0) view.zoom(0.9f);
                else view.zoom(1.1f);

                // clamp zoom a little (prevent runaway)
                float minSize = 50.f;
                float maxSize = 5000.f;
                sf::Vector2f size = view.getSize();
                size.x = std::clamp(size.x, minSize, maxSize);
                size.y = std::clamp(size.y, minSize * 0.75f, maxSize * 0.75f);
                view.setSize(size);
            }

            // Click spawn satellite
            if (auto click = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (click->button == sf::Mouse::Button::Left)
                {
                    sf::Vector2f worldPos =
                        window.mapPixelToCoords(
                            sf::Mouse::getPosition(window));

                    float r = length(worldPos - EARTH_CENTER);
                    if (r > EARTH_RADIUS + 5.f) // require spawn outside Earth's surface
                    {
                        Satellite ns;
                        ns.shape = sf::CircleShape(5.f);
                        ns.shape.setFillColor(sf::Color::Yellow);
                        ns.shape.setOrigin({ 5,5 });
                        ns.shape.setPosition(worldPos);

                        sf::Vector2f dir = normalize(worldPos - EARTH_CENTER);
                        sf::Vector2f tangent = { -dir.y, dir.x };

                        // apply speed scale to make spawned satellites orbit slower/faster
                        float v = std::sqrt(G * EARTH_MASS / std::max(r, MIN_DIST)) * ORBIT_SPEED_SCALE;
                        ns.velocity = tangent * v;

                        ns.trail.reserve(256);
                        sats.push_back(std::move(ns));
                    }
                }
            }
        }

        // Camera pan (scale pan by view zoom so movement feels consistent)
        float camBase = 8.f;
        sf::Vector2f viewSize = view.getSize();
        float zoomScale = viewSize.x / 1200.f;
        float cam = camBase * zoomScale;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) view.move({ -cam,0 });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) view.move({ cam,0 });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) view.move({ 0,-cam });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) view.move({ 0,cam });

        // Physics update (iterate backwards to allow safe removal)
        for (int i = static_cast<int>(sats.size()) - 1; i >= 0; --i)
        {
            Satellite& sat = sats[i];
            if (!sat.alive) { sats.erase(sats.begin() + i); continue; }

            sf::Vector2f pos = sat.shape.getPosition();
            sf::Vector2f toEarth = EARTH_CENTER - pos;


            float dist = length(toEarth);
            if (dist <= EARTH_RADIUS)
            {
                // simple collision: mark dead (could add explosion, scoring, etc.)
                sat.alive = false;
                continue;
            }

            sf::Vector2f dir = normalize(toEarth);

            float accel = G * EARTH_MASS / (dist * dist + MIN_DIST);
            sf::Vector2f a = dir * accel;

            // Fake J2 drift
            sf::Vector2f tangent = { -dir.y, dir.x };
            a += tangent * J2_STRENGTH * dist;

            // integrate with dt
            sat.velocity += a * dt;
            pos += sat.velocity * dt;

            sat.shape.setPosition(pos);

            // Trail: append, and remove excess in larger blocks to avoid O(n^2)
            sat.trail.emplace_back(pos, sf::Color::Green);
            if (sat.trail.size() > MAX_TRAIL)
            {
                // remove oldest block to amortize cost
                size_t removeCount = sat.trail.size() - MAX_TRAIL;
                if (removeCount < 16) removeCount = 16;
                sat.trail.erase(sat.trail.begin(), sat.trail.begin() + static_cast<long>(removeCount));
            }

            // Energy print debug: once per 200 physics updates (global)
            ++energyCounter;
            if (energyCounter % 200 == 0)
            {
                std::cout << "Energy: " << energy(pos, sat.velocity) << std::endl;
            }
        }

        window.clear(sf::Color::Black);
        window.setView(view);

        window.draw(earth);

        // Draw predicted path for the first satellite (if any)
        if (!sats.empty())
        {
            auto ghost = predictOrbit(sats[0].shape.getPosition(), sats[0].velocity, 0.02f, 400);
            if (!ghost.empty())
                window.draw(&ghost[0], ghost.size(), sf::PrimitiveType::LineStrip);
        }

        // Draw satellites + trails
        for (auto& sat : sats)
        {
            if (!sat.trail.empty())
                window.draw(&sat.trail[0], sat.trail.size(), sf::PrimitiveType::LineStrip);

            window.draw(sat.shape);
        }

        window.display();
    }

    return 0;
}