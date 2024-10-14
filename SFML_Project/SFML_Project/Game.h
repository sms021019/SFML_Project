#pragma once

#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

/*
	Class that acts as the game engine
	Wrapper class.
*/

class Game
{
private:
	// Variables
	// Windows
	sf::RenderWindow* window;
	sf::VideoMode videoMode;
	sf::Event ev;

	// Game objects
	sf::RectangleShape enemy;
	float movementSpeed; // New: Movement speed for character

	// Networking
	sf::TcpSocket socket;  // Client's connection to server
	sf::Vector2f playerPosition;
	std::vector<sf::Vector2f> otherPlayers; // Track other players' positions


	// Private functions
	void initVariables();
	void initWindow();
	void initEnemies();

public:
	// Constructors / Destructors
	Game();
	virtual ~Game();

	// Accessors
	const bool running() const;

	// Functions
	void pollEvents();
	void updateMovement();
	void update();
	void render();
	void connectToServer();
	void sendPositionToServer();
	void receivePositionsFromServer();
};

