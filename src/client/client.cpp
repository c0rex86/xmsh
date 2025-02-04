#include "client.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

namespace xmsh {

Client::Client() : connected_(false) {
    init_ssl();
}

Client::~Client() {
    disconnect();
}

void Client::init_ssl() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    connection_.ctx.reset(SSL_CTX_new(TLS_client_method()));
    if (!connection_.ctx) {
        throw XMSHException("Failed to create SSL context");
    }
}

void Client::connect(const std::string& host, int port) {
    if (connected_) {
        disconnect();
    }

    connection_.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connection_.socket < 0) {
        throw XMSHException("Failed to create socket");
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
        throw XMSHException("Invalid address");
    }

    if (::connect(connection_.socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw XMSHException("Connection failed");
    }

    connection_.ssl.reset(SSL_new(connection_.ctx.get()));
    if (!connection_.ssl) {
        throw XMSHException("Failed to create SSL object");
    }

    SSL_set_fd(connection_.ssl.get(), connection_.socket);

    if (SSL_connect(connection_.ssl.get()) != 1) {
        throw XMSHException("SSL connection failed");
    }

    connected_ = true;
}

void Client::disconnect() {
    if (connected_) {
        if (connection_.ssl) {
            SSL_shutdown(connection_.ssl.get());
        }
        if (connection_.socket >= 0) {
            close(connection_.socket);
            connection_.socket = -1;
        }
        connected_ = false;
    }
}

void Client::send_command(const std::string& command) {
    if (!connected_) {
        throw XMSHException("Not connected");
    }

    if (SSL_write(connection_.ssl.get(), command.c_str(), command.length()) <= 0) {
        throw XMSHException("Failed to send command");
    }
}

std::string Client::receive_response() {
    if (!connected_) {
        throw XMSHException("Not connected");
    }

    char buffer[BUFFER_SIZE];
    int bytes = SSL_read(connection_.ssl.get(), buffer, sizeof(buffer) - 1);
    
    if (bytes <= 0) {
        throw XMSHException("Failed to receive response");
    }

    buffer[bytes] = '\0';
    return std::string(buffer);
}

} 