#include<SFML/Graphics.hpp>
#include<SFML/Window.hpp>
#include <vector>
#include <algorithm>
constexpr auto PI = 3.14159265f;

int nScreenWidth = 800;
int nScreenHeight = 600;

void wrapCoordinate(float &x, float &y);
bool isPointInsideCircle(float cx, float cy, float r, float x, float y);

struct sSpaceObject
{
    float x, y, vx, vy, angle, size;
};

int main()
{
    // =======ASTEROID SETUP=======
    std::vector<sSpaceObject> vecAsteroids{};
    vecAsteroids.push_back({50.f, 50.f, 100.f, 100.f, 0.0f, 1.0f});
    float fAsteroidRad = 100.0f;
    sf::CircleShape asteroidModelShape(fAsteroidRad);
    asteroidModelShape.setFillColor(sf::Color(0, 0, 0));
    asteroidModelShape.setOutlineColor(sf::Color(250, 150, 100));
    asteroidModelShape.setOutlineThickness(1.0f);
    asteroidModelShape.setOrigin(fAsteroidRad/2, fAsteroidRad/2);

    // =======SHIP SETUP=======
    sSpaceObject ship{float(nScreenWidth)/2.0f, float(nScreenHeight)/2.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    sf::ConvexShape shipModelShape;
    float ship_box_h = 24.0f, ship_box_w = 15.0f;
    shipModelShape.setPointCount(3);
    shipModelShape.setPoint(0, sf::Vector2f(0.0, -ship_box_h*2.0f/3.0f));
    shipModelShape.setPoint(1, sf::Vector2f(-ship_box_w/2.0f, ship_box_h/3.0f));
    shipModelShape.setPoint(2, sf::Vector2f(ship_box_w/2.0f, ship_box_h/3.0f));
    shipModelShape.setFillColor(sf::Color(255, 255, 255));
    float shipAngVel = 100.0f;
    float shipAcc = 150.0f;

    // ======BULLET SETUP======
    std::vector<sSpaceObject> vecBullets{};
    sf::RectangleShape bulletModelShape(sf::Vector2f(3.0f, 3.0f));
    float bulletVec = 250.f;
    bulletModelShape.setFillColor(sf::Color::Cyan);

    // ======CREATE WINDOW======
    sf::RenderWindow window(sf::VideoMode(nScreenWidth, nScreenHeight), "");
    window.setKeyRepeatEnabled(false);

    sf::Clock triggerClock;
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

        // steer the ship
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            ship.angle += shipAngVel * fElapsedTime;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))   
            ship.angle -= shipAngVel * fElapsedTime;
        
        // thrust the ship
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            ship.vx += shipAcc * sinf(ship.angle * PI / 180.f) * fElapsedTime;
            ship.vy += shipAcc * -cosf(ship.angle * PI / 180.f) * fElapsedTime;
        }

        // trigger bullet
        sf::Time triggerDuration = triggerClock.getElapsedTime();
        float triggerDelay = triggerDuration.asSeconds();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && triggerDelay > 0.25f)
        {
            triggerClock.restart();
            float bulletTriggerPos_x = 
                ship.x + shipModelShape.getPoint(0).x * cosf(ship.angle * PI / 180) - 
                shipModelShape.getPoint(0).y * sinf(ship.angle * PI / 180);
            float bulletTriggerPos_y =  
                ship.y + shipModelShape.getPoint(0).y * cosf(ship.angle * PI / 180) + 
                shipModelShape.getPoint(0).x * sinf(ship.angle * PI / 180);
            sSpaceObject bullet{
                bulletTriggerPos_x,
                bulletTriggerPos_y, 
                bulletVec * sinf(ship.angle * PI / 180.f),
                bulletVec * -cosf(ship.angle * PI / 180.f), 
                ship.angle, 
                1.0f
            };
            vecBullets.push_back(bullet);
        }

        // clear the window
        window.clear(sf::Color::Black);

        // draw and update player ship
        ship.x += ship.vx * fElapsedTime;
        ship.y += ship.vy * fElapsedTime;
        wrapCoordinate(ship.x, ship.y);
        shipModelShape.setPosition(ship.x, ship.y);
        shipModelShape.setRotation(ship.angle);
        window.draw(shipModelShape);
        printf("%f\n", ship.angle);

        // draw and update bullets
        for(auto &b : vecBullets)
        {
            b.x += b.vx * fElapsedTime;
            b.y += b.vy * fElapsedTime;
            bulletModelShape.setPosition(b.x, b.y);
            window.draw(bulletModelShape);
            
            // Check for asteroid collision
            for(auto &a : vecAsteroids)
            {
                if(isPointInsideCircle(a.x, a.y, a.size * fAsteroidRad, b.x, b.y))
                {
                    b.x -= 1000.0f;
                }
            }
        }

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

        // remove bullet if it is off the screen
        if(!vecBullets.empty())
        {
            auto iter = std::remove_if(
                vecBullets.begin(),
                vecBullets.end(),
                [&](sSpaceObject o) { return (o.x < 1.0f || o.y < 1.0f || o.x >= nScreenWidth-1 || o.y >= nScreenHeight-1);}
            );
            if(iter != vecBullets.end())
                vecBullets.erase(iter);
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

bool isPointInsideCircle(float cx, float cy, float r, float x, float y)
{
    float dis = sqrt((x - cx)*(x - cx) + (y - cy)*(y - cy));
    return (dis < r) ? true : false;
}