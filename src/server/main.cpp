#include "server.hpp"
#include <csignal>

xmsh::Server* g_server = nullptr;

void signal_handler(int signal) {
    if (g_server) {
        g_server->stop();
    }
}

int main(int argc, char* argv[]) {
    try {
        int port = xmsh::DEFAULT_PORT;
        if (argc > 1) {
            port = std::stoi(argv[1]);
        }

        signal(SIGINT, signal_handler);
        signal(SIGTERM, signal_handler);

        xmsh::Server server(port);
        g_server = &server;
        server.start();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 