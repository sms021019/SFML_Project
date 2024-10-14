#include "Game.h"
#include <iostream>

// Private Functions
void Game::initVariables()
{
    this->window = nullptr;
    this->movementSpeed = 5.f; 
}

void Game::initWindow()
{
    this->videoMode.height = 600;
    this->videoMode.width = 800;

    this->window = new sf::RenderWindow(this->videoMode, "Game 1", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
    this->window->setFramerateLimit(144);
}

void Game::initEnemies()
{
    this->enemy.setPosition(10.f, 10.f);
    this->enemy.setSize(sf::Vector2f(50.f, 50.f));
    this->enemy.setFillColor(sf::Color::Cyan);
}

Game::Game()
{
    this->initVariables();
    this->initWindow();
    this->initEnemies();

    this->connectToServer(); 
}

Game::~Game()
{
    delete this->window;
}

const bool Game::running() const
{
    return this->window->isOpen();
}

void Game::pollEvents()
{
    while (this->window->pollEvent(this->ev))
    {
        switch (this->ev.type)
        {
        case sf::Event::Closed:
            this->window->close();
            break;
        case sf::Event::KeyPressed:
            if (this->ev.key.code == sf::Keyboard::Escape)
                this->window->close();
            break;
        }
    }
}

void Game::updateMovement()
{
    // Player Movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        this->enemy.move(0.f, -this->movementSpeed);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        this->enemy.move(0.f, this->movementSpeed);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        this->enemy.move(-this->movementSpeed, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        this->enemy.move(this->movementSpeed, 0.f);

    // Boundary checking
    if (this->enemy.getPosition().x < 0.f)
        this->enemy.setPosition(0.f, this->enemy.getPosition().y);
    if (this->enemy.getPosition().y < 0.f)
        this->enemy.setPosition(this->enemy.getPosition().x, 0.f);
    if (this->enemy.getPosition().x + this->enemy.getSize().x > this->window->getSize().x)
        this->enemy.setPosition(this->window->getSize().x - this->enemy.getSize().x, this->enemy.getPosition().y);
    if (this->enemy.getPosition().y + this->enemy.getSize().y > this->window->getSize().y)
        this->enemy.setPosition(this->enemy.getPosition().x, this->window->getSize().y - this->enemy.getSize().y);

    // Update player position
    this->playerPosition = this->enemy.getPosition();
}

void Game::update()
{
    this->pollEvents();
    this->updateMovement();   
    this->sendPositionToServer();  
    this->receivePositionsFromServer();  

    std::cout << "Updating game state" << std::endl;
}

void Game::render()
{
    this->window->clear(sf::Color::Black);

    this->window->draw(this->enemy);

    // Draw other players
    sf::RectangleShape otherPlayerShape(this->enemy.getSize());
    otherPlayerShape.setFillColor(sf::Color::Red);
    for (auto& position : this->otherPlayers)
    {
        otherPlayerShape.setPosition(position);
        this->window->draw(otherPlayerShape);
    }

    this->window->display();
}

// Networking

void Game::connectToServer()
{
    if (this->socket.connect("127.0.0.1", 53000) != sf::Socket::Done)
    {
        std::cout << "Error connecting to server\n";
    }
    else
    {
        std::cout << "Connected to server\n";

        this->socket.setBlocking(false);
    }
}

void Game::sendPositionToServer()
{
    std::cout << "Sending position to server..." << std::endl; 

    sf::Socket::Status status = this->socket.send(&this->playerPosition, sizeof(this->playerPosition));

    if (status == sf::Socket::Partial)
    {
        std::cout << "Partial data sent. Need to send remaining data..." << std::endl;
    }
    else if (status == sf::Socket::NotReady)
    {
        // Socket isn't ready yet, skip this frame and try again
    }
    else if (status == sf::Socket::Done)
    {
        std::cout << "Position successfully sent to server." << std::endl;
    }
    else
    {
        std::cout << "Error sending position to server" << std::endl;
    }
}

void Game::receivePositionsFromServer()
{
    std::cout << "Receiving positions from server..." << std::endl; 

    sf::Vector2f receivedPosition;
    std::size_t received;
    this->otherPlayers.clear(); 

    sf::Socket::Status status = this->socket.receive(&receivedPosition, sizeof(receivedPosition), received);

    if (status == sf::Socket::Done)
    {
        if (received == sizeof(receivedPosition))
        {
            this->otherPlayers.push_back(receivedPosition);
            std::cout << "Position received from server: (" << receivedPosition.x << ", " << receivedPosition.y << ")\n";
        }
    }
    else if (status == sf::Socket::NotReady)
    {
        // No data received this frame, just continue
    }
    else if (status == sf::Socket::Disconnected)
    {
        std::cout << "Server disconnected" << std::endl;
    }
    else
    {
        std::cout << "Error receiving position from server" << std::endl;
    }
}


