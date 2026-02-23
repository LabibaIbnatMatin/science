#include "rclcpp/rclcpp.hpp"
#include "joy_to_teensy/teensy_serial_backend.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

// --- DEFINITIONS (Edit these here) ---
#define RECEIVE_PORT 5005
#define RECEIVE_IP "0.0.0.0"  // Listen on all network interfaces
#define SERIAL_PORT "/dev/teensy"
#define SERIAL_BAUDRATE 115200

class Joy : public rclcpp::Node {
public:
    Joy() : rclcpp::Node("joy_node") {
        
        // 1. UDP Socket Setup
        sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd_ < 0) {
            RCLCPP_ERROR(this->get_logger(), "Socket creation failed!");
            return;
        }

        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(RECEIVE_PORT);
        servaddr.sin_addr.s_addr = inet_addr(RECEIVE_IP);

        if (bind(sockfd_, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            RCLCPP_ERROR(this->get_logger(), "UDP Bind failed on %s:%d", RECEIVE_IP, RECEIVE_PORT);
            return;
        }

        // 2. Serial Setup with Teensy
        teensy_.setup(SERIAL_PORT, SERIAL_BAUDRATE);

        // 3. Timer to check for UDP packets every 5ms
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(5), 
            std::bind(&Joy::udp_to_serial_loop, this)
        );

        RCLCPP_INFO(this->get_logger(), "C++ Bridge started! Listening on %s:%d", RECEIVE_IP, RECEIVE_PORT);
    }

    ~Joy() {
        if (sockfd_ >= 0) close(sockfd_);
    }

private:
    int sockfd_;
    rclcpp::TimerBase::SharedPtr timer_;
    TeensyComms teensy_;

    void udp_to_serial_loop() {
        char buffer[1024];
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);

        // Non-blocking receive
        int n = recvfrom(sockfd_, (char *)buffer, 1024, MSG_DONTWAIT, (struct sockaddr *)&cliaddr, &len);

        if (n > 0) {
            buffer[n] = '\0';
            std::string raw_cmd(buffer); 

            teensy_.send_msg(raw_cmd);
            RCLCPP_INFO(this->get_logger(), "Sent: %s", raw_cmd.c_str());
        }
    }
};

int main(int argc, char * argv[]) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<Joy>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
