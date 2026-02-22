#include "joy_to_teensy/teensy_serial_backend.hpp"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdexcept>
#include <cstring>

TeensyComms::TeensyComms() : serial_fd_(-1), timeout_ms_(500), is_open_(false) {}

TeensyComms::~TeensyComms() {
    if (is_open_ && serial_fd_ >= 0) {
        close(serial_fd_);
    }
}

bool TeensyComms::setup(const std::string& port, int baudrate, int timeout_ms) {
    timeout_ms_ = timeout_ms;
    
    // Open serial port
    serial_fd_ = open(port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (serial_fd_ < 0) {
        return false;
    }

    struct termios tty;
    if (tcgetattr(serial_fd_, &tty) != 0) {
        close(serial_fd_);
        serial_fd_ = -1;
        return false;
    }

    // Set baud rate
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

    // Configure serial port settings
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;  // 8-bit chars
    tty.c_iflag &= ~IGNBRK;                       // disable break processing
    tty.c_lflag = 0;                              // no signaling chars, no echo, no canonical processing
    tty.c_oflag = 0;                              // no remapping, no delays
    tty.c_cc[VMIN]  = 0;                          // read doesn't block
    tty.c_cc[VTIME] = timeout_ms / 100;           // timeout in 0.1s units

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);       // shut off xon/xoff ctrl
    tty.c_cflag |= (CLOCAL | CREAD);              // ignore modem controls, enable reading
    tty.c_cflag &= ~(PARENB | PARODD);            // no parity
    tty.c_cflag &= ~CSTOPB;                       // 1 stop bit
    tty.c_cflag &= ~CRTSCTS;                      // no hardware flowcontrol

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

    // Send message
    std::string msg_with_newline = msg + "\n";
    ssize_t bytes_written = write(serial_fd_, msg_with_newline.c_str(), msg_with_newline.size());
    if (bytes_written < 0) {
        return "";
    }

    // Read response
    char buf[256] = {0};
    ssize_t n = read(serial_fd_, buf, sizeof(buf) - 1);
    
    if (n > 0) {
        return std::string(buf, n);
    }
    
    return "";
}
