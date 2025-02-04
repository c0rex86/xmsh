#pragma once

#include "xmsh.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <map>

namespace xmsh {

class Server {
public:
    Server(int port = DEFAULT_PORT);
    ~Server();

    void start();
    void stop();

private:
    void handle_client(int client_socket);
    void init_ssl();
    
    int port_;
    int server_socket_;
    bool running_;
    std::map<int, std::thread> client_threads_;
    Connection connection_;
};

} 