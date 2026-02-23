#pragma once
#include <string>

class TeensyComms {
public:
    TeensyComms();
    ~TeensyComms();

    bool setup(const std::string& port, int baudrate);
    std::string send_msg(const std::string& msg);

private:
    int serial_fd_;
    bool is_open_;
};