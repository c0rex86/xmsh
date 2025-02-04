#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <openssl/ssl.h>
#include <openssl/err.h>

namespace xmsh {

constexpr int DEFAULT_PORT = 2222;
constexpr int BUFFER_SIZE = 4096;

struct Connection {
    std::unique_ptr<SSL_CTX, void(*)(SSL_CTX*)> ctx;
    std::unique_ptr<SSL, void(*)(SSL*)> ssl;
    int socket;

    Connection() : ctx(nullptr, SSL_CTX_free), 
                  ssl(nullptr, SSL_free), 
                  socket(-1) {}
};

class XMSHException : public std::runtime_error {
public:
    explicit XMSHException(const std::string& msg) : std::runtime_error(msg) {}
};

} 