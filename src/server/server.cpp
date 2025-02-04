#include "server.hpp"
#include <cstring>
#include <arpa/inet.h>

namespace xmsh {

Server::Server(int port) : port_(port), server_socket_(-1), running_(false) {
    init_ssl();
}

Server::~Server() {
    stop();
}

void Server::init_ssl() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    connection_.ctx.reset(SSL_CTX_new(TLS_server_method()));
    if (!connection_.ctx) {
        throw XMSHException("Failed to create SSL context");
    }

    if (SSL_CTX_use_certificate_file(connection_.ctx.get(), "server.crt", SSL_FILETYPE_PEM) <= 0) {
        throw XMSHException("Failed to load certificate");
    }

    if (SSL_CTX_use_PrivateKey_file(connection_.ctx.get(), "server.key", SSL_FILETYPE_PEM) <= 0) {
        throw XMSHException("Failed to load private key");
    }
}

void Server::start() {
    server_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_ < 0) {
        throw XMSHException("Failed to create socket");
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port_);

    if (bind(server_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw XMSHException("Failed to bind socket");
    }

    if (listen(server_socket_, 5) < 0) {
        throw XMSHException("Failed to listen on socket");
    }

    running_ = true;
    std::cout << "Server started on port " << port_ << std::endl;

    while (running_) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket_, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            if (running_) {
                std::cerr << "Failed to accept connection" << std::endl;
            }
            continue;
        }

        std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << std::endl;
        client_threads_[client_socket] = std::thread(&Server::handle_client, this, client_socket);
    }
}

void Server::handle_client(int client_socket) {
    Connection client_conn;
    client_conn.socket = client_socket;
    client_conn.ssl.reset(SSL_new(connection_.ctx.get()));
    
    SSL_set_fd(client_conn.ssl.get(), client_socket);
    
    if (SSL_accept(client_conn.ssl.get()) <= 0) {
        std::cerr << "SSL accept failed" << std::endl;
        close(client_socket);
        return;
    }

    char buffer[BUFFER_SIZE];
    while (running_) {
        int bytes = SSL_read(client_conn.ssl.get(), buffer, sizeof(buffer));
        if (bytes <= 0) {
            break;
        }
        
        buffer[bytes] = '\0';
        std::cout << "Received: " << buffer << std::endl;
        
        SSL_write(client_conn.ssl.get(), buffer, bytes);
    }

    close(client_socket);
    client_threads_.erase(client_socket);
}

void Server::stop() {
    running_ = false;
    if (server_socket_ >= 0) {
        close(server_socket_);
        server_socket_ = -1;
    }

    for (auto& [socket, thread] : client_threads_) {
        close(socket);
        if (thread.joinable()) {
            thread.join();
        }
    }
    client_threads_.clear();
}

} 