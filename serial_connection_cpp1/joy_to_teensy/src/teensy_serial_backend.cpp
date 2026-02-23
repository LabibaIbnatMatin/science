#include "joy_to_teensy/teensy_serial_backend.hpp"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>

TeensyComms::TeensyComms() : serial_fd_(-1), is_open_(false) {}

TeensyComms::~TeensyComms() {
    if (is_open_ && serial_fd_ >= 0) {
        close(serial_fd_);
    }
}

bool TeensyComms::setup(const std::string& port, int baudrate) {
    serial_fd_ = open(port.c_str(), O_RDWR | O_NOCTTY);
    if (serial_fd_ < 0) {
        return false;
    }

    struct termios tty;
    if (tcgetattr(serial_fd_, &tty) != 0) {
        close(serial_fd_);
        serial_fd_ = -1;
        return false;
    }

    speed_t speed;
    switch (baudrate) {
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
        case 57600: speed = B57600; break;
        case 115200: speed = B115200; break;
        default:
            close(serial_fd_);
            serial_fd_ = -1;
            return false;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_iflag &= ~IGNBRK;
    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_cc[VMIN]  = 0;   // non-blocking read
    tty.c_cc[VTIME] = 1;   // 0.1s read timeout (minimal)

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(serial_fd_, TCSANOW, &tty) != 0) {
        close(serial_fd_);
        serial_fd_ = -1;
        return false;
    }

    is_open_ = true;
    return true;
}

std::string TeensyComms::send_msg(const std::string& msg) {
    if (!is_open_ || serial_fd_ < 0) {
        return "";
    }

    std::string msg_with_newline = msg + "\n";
    write(serial_fd_, msg_with_newline.c_str(), msg_with_newline.size());

    return "ok";
}
