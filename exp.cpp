#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include <vector>

int nScreenWidth = 800;
int nScreenHeight = 600;

void wrapCoordinate(float &x, float &y);

struct sSpaceObject
{
    float x, y, vx, vy, angle, size;
};

int main()
{
    std::vector<sSpaceObject> vecAsteroids{};
    vecAsteroids.push_back({50.f, 50.f, 100.f, 100.f, 0.0f, 1.0f});
    float fRadius = 100.0f;
    sf::CircleShape asteroidModelShape(fRadius);
    asteroidModelShape.setFillColor(sf::Color(0, 0, 0));
    asteroidModelShape.setOutlineColor(sf::Color(250, 150, 100));
    asteroidModelShape.setOutlineThickness(1.0f);
    asteroidModelShape.setOrigin(fRadius/2, fRadius/2);

    sSpaceObject ship{float(nScreenWidth)/2.0f, float(nScreenHeight)/2.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    //sf::CircleShape shipModelShape(20.0f, 3);
    sf::ConvexShape shipModelShape;
    float ship_box_h = 24.0f, ship_box_w = 15.0f;
    shipModelShape.setOrigin(ship_box_w / 2.0f, ship_box_h / 2.0f);
    shipModelShape.setPointCount(3);
    shipModelShape.setPoint(0, sf::Vector2f(0.0, -ship_box_h*2.0f/3.0f));
    shipModelShape.setPoint(1, sf::Vector2f(-ship_box_w/2.0f, ship_box_h/3.0f));
    shipModelShape.setPoint(2, sf::Vector2f(ship_box_w/2.0f, ship_box_h/3.0f));
    shipModelShape.setFillColor(sf::Color(255, 255, 255));

    sf::RenderWindow window(sf::VideoMode(nScreenWidth, nScreenHeight), "");

    sf::Clock clock;
    // run the program as long as the window is open
    while (window.isOpen())
    {
        sf::Time duration = clock.restart();
        float fElapsedTime = duration.asSeconds();

        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window
        window.clear(sf::Color::Black);

        // draw and update player ship
        shipModelShape.setPosition(ship.x, ship.y);
        shipModelShape.rotate(ship.angle);
        window.draw(shipModelShape);

        // draw and update asteroids
        for(auto &a : vecAsteroids)
        {
            a.x += a.vx * fElapsedTime;
            a.y += a.vy * fElapsedTime;
            wrapCoordinate(a.x, a.y);
            
            asteroidModelShape.setPosition(a.x, a.y);
            asteroidModelShape.scale(a.size, a.size);
            asteroidModelShape.rotate(a.angle);
            window.draw(asteroidModelShape);
        }

        window.display();
    }

    return 0;
}


//----------------FUCTION DECLARATIONS------------------
void wrapCoordinate(float &x, float &y)
{
    if(x <= 0.0f)           x += nScreenWidth;
    if(x >= nScreenWidth)   x -= nScreenWidth;
    if(y <= 0.0f)           y += nScreenHeight;
    if(y >= nScreenHeight)  y -= nScreenHeight;
}