#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <map>

// Structure to represent a player and their position
struct Player {
    sf::Vector2f position;
};

// Server class to manage clients and their player positions
class Server
{
private:
    sf::TcpListener listener;
    std::vector<sf::TcpSocket*> clients;
    sf::SocketSelector selector;
    std::map<sf::TcpSocket*, Player> players; 

    bool running;

    void acceptNewClient()
    {
        auto* newClient = new sf::TcpSocket;
        if (this->listener.accept(*newClient) == sf::Socket::Done)
        {
            std::cout << "New client connected!\n";
            this->clients.push_back(newClient);
            this->selector.add(*newClient);

            // Initialize new player for the client
            players[newClient] = Player{ sf::Vector2f(100.f, 100.f) };
        }
        else
        {
            delete newClient;
        }
    }

    void handleClientMessage(sf::TcpSocket* client)
    {
        // Buffer to receive player position
        sf::Vector2f newPos;
        std::size_t received;

        // Attempt to receive data from the client
        sf::Socket::Status status = client->receive(&newPos, sizeof(newPos), received);

        if (status == sf::Socket::Done)
        {
            if (received == sizeof(newPos))
            {
                players[client].position = newPos;
                std::cout << "Updated player position to (" << newPos.x << ", " << newPos.y << ")\n";

                broadcastPlayerPositions();
            }
            else
            {
                std::cout << "Received incorrect data size\n";
            }
        }
        else if (status == sf::Socket::Disconnected)
        {
            std::cout << "Client disconnected\n";

            this->selector.remove(*client); 
            this->clients.erase(std::remove(this->clients.begin(), this->clients.end(), client), this->clients.end());
            this->players.erase(client); 

            delete client; 
        }
        else
        {
            std::cout << "Error receiving data from client\n";
        }
    }



    void broadcastPlayerPositions()
    {
        for (auto* client : this->clients)  
        {
            // Send the position of all players to every client
            for (auto& pair : this->players) 
            {
                sf::TcpSocket* clientSocket = pair.first; 
                Player& player = pair.second;      

                // Send the player's position to the current client
                if (client->send(&player.position, sizeof(player.position)) != sf::Socket::Done)
                {
                    std::cout << "Error sending player position to a client\n";
                }
            }
        }
    }


public:
    Server() : running(true) {}

    void run()
    {
        // Start listening for incoming connections
        if (this->listener.listen(53000) != sf::Socket::Done)
        {
            std::cout << "Error starting the server\n";
            return;
        }
        std::cout << "Server is listening on port 53000...\n";

        this->selector.add(this->listener);

        while (this->running)
        {
            if (this->selector.wait())  // Wait until data is available
            {
                if (this->selector.isReady(this->listener))
                {
                    acceptNewClient();
                }
                else
                {
                    for (auto* client : this->clients)
                    {
                        if (this->selector.isReady(*client))
                        {
                            handleClientMessage(client);
                        }
                    }
                }
            }
        }
    }
};

int main()
{
    Server server;
    server.run();

    return 0;
}
