#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <algorithm>
#include <string>
constexpr auto PI = 3.14159265f;

int nScreenWidth = 800;
int nScreenHeight = 600;

void wrapCoordinate(float &x, float &y);
bool isPointInsideCircle(float cx, float cy, float r, float x, float y);
// Triangular functions that take degree argument
inline float sind(float randian) { return sinf(randian * PI / 180); }
inline float cosd(float randian) { return cosf(randian * PI / 180); }

struct sSpaceObject
{
    float x, y, vx, vy, angle, size;
};

int main()
{
    srand(time(0));
    // =======FONT AND TEXT========
    sf::Font font;
    font.loadFromFile("consola.ttf");
    sf::Text score_text;
    score_text.setString("SCORE: ");
    score_text.setCharacterSize(16);
    score_text.setFillColor(sf::Color::White);

    // =======ASTEROID SETUP=======
    std::vector<sSpaceObject> vecAsteroids{};
    vecAsteroids.push_back({50.f, 50.f, 100.f, 100.f, 0.0f, 2.0f});
    float fAsteroidRad = 70.f;
    sf::CircleShape asteroidModelShape(fAsteroidRad);
    asteroidModelShape.setFillColor(sf::Color::Transparent);
    asteroidModelShape.setOutlineColor(sf::Color(250, 150, 100));
    asteroidModelShape.setOutlineThickness(1.0f);
    asteroidModelShape.setOrigin(fAsteroidRad, fAsteroidRad);

    // =======SHIP SETUP=======
    sSpaceObject ship{float(nScreenWidth) / 2.0f, float(nScreenHeight) / 2.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    sf::ConvexShape shipModelShape;
    float ship_box_h = 40.0f, ship_box_w = 22.0f;
    shipModelShape.setPointCount(3);
    shipModelShape.setPoint(0, sf::Vector2f(0.0, -ship_box_h * 2.0f / 3.0f));
    shipModelShape.setPoint(1, sf::Vector2f(-ship_box_w / 2.0f, ship_box_h / 3.0f));
    shipModelShape.setPoint(2, sf::Vector2f(ship_box_w / 2.0f, ship_box_h / 3.0f));
    shipModelShape.setFillColor(sf::Color(255, 255, 255));
    float shipAngVel = 200.0f; // Degree per second
    float shipAcc = 150.0f;    // px per second

    // ======BULLET SETUP======
    std::vector<sSpaceObject> vecBullets{};
    sf::RectangleShape bulletModelShape(sf::Vector2f(3.0f, 5.0f));
    float bulletVec = 500.f; // px per sec
    bulletModelShape.setFillColor(sf::Color::White);

    // ======CREATE WINDOW======
    sf::RenderWindow window(sf::VideoMode(nScreenWidth, nScreenHeight), "");
    window.setKeyRepeatEnabled(false);

    sf::Clock triggerClock;
    sf::Clock clock;
    bool bDead = false;
    int nScore = 0;
    auto ResetGame = [&]() {
        vecAsteroids.clear();
        vecBullets.clear();
        vecAsteroids.push_back({50.f, 50.f, 100.f, 60.f, 0.0f, 2.0f});
        vecAsteroids.push_back({600.f, 100.f, -50.f, 20.f, 0.0f, 2.8f});
        vecAsteroids.push_back({50.f, 0.f, 150.f, -10.f, 0.0f, 1.5f});
        vecAsteroids.push_back({600., 500.f, 5.f, -30.f, 0.0f, 2.5f});
        ship = {float(nScreenWidth) / 2.0f, float(nScreenHeight) / 2.0f, 0.0f, 0.0f, 0.0f, 0.0f};
        bDead = false;
    };
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

        if (bDead == true)
            ResetGame();

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
                ship.x +
                shipModelShape.getPoint(0).x * cosf(ship.angle * PI / 180) -
                shipModelShape.getPoint(0).y * sinf(ship.angle * PI / 180);

            float bulletTriggerPos_y =
                ship.y +
                shipModelShape.getPoint(0).y * cosf(ship.angle * PI / 180) +
                shipModelShape.getPoint(0).x * sinf(ship.angle * PI / 180);

            sSpaceObject bullet{
                bulletTriggerPos_x,
                bulletTriggerPos_y,
                ship.vx + bulletVec * sinf(ship.angle * PI / 180.f), // add initial vel
                ship.vy + bulletVec * -cosf(ship.angle * PI / 180.f),
                ship.angle,
                1.0f};
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

        // draw and update bullets and check bullet collision
        std::vector<sSpaceObject> vecNewAsteroids{};
        for (auto &b : vecBullets)
        {
            b.x += b.vx * fElapsedTime;
            b.y += b.vy * fElapsedTime;
            bulletModelShape.setPosition(b.x, b.y);
            window.draw(bulletModelShape);

            // Check for asteroid collision
            for (auto &a : vecAsteroids)
            {
                if (isPointInsideCircle(a.x, a.y, a.size * fAsteroidRad, b.x, b.y))
                {
                    b.x -= 1000.0f;
                    nScore += 100;

                    // split the asteroid if size > 4 else delete it
                    if (a.size > 0.5f)
                    {
                        float angle1 = float(rand()) / float(RAND_MAX) * 360.f; //Degree not radian
                        float angle2 = float(rand()) / float(RAND_MAX) * 360.f;
                        float splitVec = 50.f;
                        vecNewAsteroids.push_back({a.x, a.y, splitVec * sind(angle1), splitVec * -cosd(angle1), angle1, a.size / 2.0f});
                        vecNewAsteroids.push_back({a.x, a.y, splitVec * sind(angle2), splitVec * -cosd(angle2), angle2, a.size / 2.0f});
                    }
                    a.x = -1000.0f; // remove off the screen so the func underneath can delete it
                }
            }
        }

        // append new asteroid in original storage
        for (auto a : vecNewAsteroids)
            vecAsteroids.push_back(a);

        // remove bullet if it is off the screen
        if (!vecBullets.empty())
        {
            auto iter = std::remove_if(
                vecBullets.begin(),
                vecBullets.end(),
                [&](sSpaceObject o) { return (o.x < 1.0f || o.y < 1.0f || o.x >= nScreenWidth - 1 || o.y >= nScreenHeight - 1); });
            if (iter != vecBullets.end())
                vecBullets.erase(iter);
        }

        // remove asteroid if it is off the screen
        if (!vecAsteroids.empty())
        {
            auto iter = std::remove_if(
                vecAsteroids.begin(),
                vecAsteroids.end(),
                [&](sSpaceObject o) { return (o.x < 0.0f); });
            if (iter != vecAsteroids.end())
                vecAsteroids.erase(iter);
        }

        // draw and update asteroids
        for (auto &a : vecAsteroids)
        {
            a.x += a.vx * fElapsedTime;
            a.y += a.vy * fElapsedTime;
            wrapCoordinate(a.x, a.y);

            asteroidModelShape.setPosition(a.x, a.y);
            asteroidModelShape.setScale(a.size, a.size);
            //asteroidModelShape.rotate(a.angle);
            window.draw(asteroidModelShape);

            //debug
            sf::RectangleShape originRec(sf::Vector2f(3.f, 3.f));
            originRec.setFillColor(sf::Color::Red);
            originRec.setPosition(a.x, a.y);
            window.draw(originRec);
        }

        for (auto a : vecAsteroids)
            if (isPointInsideCircle(a.x, a.y, fAsteroidRad * a.size, ship.x, ship.y))
                bDead = true;
        
        if(vecAsteroids.empty())
        {
            nScore += 1000;
            ResetGame();
        }
        //score_text.setString(score_text.getString() + std::to_string(nScore));
        window.draw(score_text);
        window.display();
    }

    return 0;
}

//----------------FUCTION DEFINITIONS------------------
void wrapCoordinate(float &x, float &y)
{
    if (x <= 0.0f)
        x += nScreenWidth;
    if (x >= nScreenWidth)
        x -= nScreenWidth;
    if (y <= 0.0f)
        y += nScreenHeight;
    if (y >= nScreenHeight)
        y -= nScreenHeight;
}

bool isPointInsideCircle(float cx, float cy, float r, float x, float y)
{
    float dis = sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));
    return (dis < r) ? true : false;
}
