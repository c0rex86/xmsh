#pragma once

#include "xmsh.hpp"
#include <string>

namespace xmsh {

class Client {
public:
    Client();
    ~Client();

    void connect(const std::string& host, int port = DEFAULT_PORT);
    void disconnect();
    void send_command(const std::string& command);
    std::string receive_response();

private:
    void init_ssl();
    
    Connection connection_;
    bool connected_;
};

} 