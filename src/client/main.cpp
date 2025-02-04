#include "client.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <host> [port]" << std::endl;
        return 1;
    }

    try {
        std::string host = argv[1];
        int port = argc > 2 ? std::stoi(argv[2]) : xmsh::DEFAULT_PORT;

        xmsh::Client client;
        client.connect(host, port);
        std::cout << "Connected to " << host << ":" << port << std::endl;

        std::string command;
        while (std::getline(std::cin, command)) {
            if (command == "exit" || command == "quit") {
                break;
            }

            try {
                client.send_command(command);
                std::string response = client.receive_response();
                std::cout << response;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                break;
            }
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 