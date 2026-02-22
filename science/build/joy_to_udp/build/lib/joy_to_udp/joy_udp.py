import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
import socket

class JoyUdpBridge(Node):
    def __init__(self):
        super().__init__('joy_to_udp_bridge')
        
        # UDP Configuration
        self.target_ip = "127.0.0.1"  
        self.target_port = 5005
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        
        self.subscription = self.create_subscription(
            Joy, 
            'joy', 
            self.joy_callback, 
            10)
        
        self.get_logger().info("Bridge Node Started. Direct Button Direction Logic Active.")

    def get_pair_val(self, pos_btn, neg_btn):
        """
        Logic: 
        Pressed Positive Button -> 2000
        Pressed Negative Button -> 1000
        None or Both -> 1500
        """
        if pos_btn and not neg_btn:
            return 2000
        elif neg_btn and not pos_btn:
            return 1000
        return 1500

    def joy_callback(self, msg):
       
        s1 = self.get_pair_val(msg.buttons[0], msg.buttons[1])
        s2 = s1 

       
        d1 = self.get_pair_val(msg.buttons[2], msg.buttons[3])

       
        d2 = self.get_pair_val(msg.buttons[4], msg.buttons[5])

      
        st1 = self.get_pair_val(msg.buttons[8], msg.buttons[9])
        st2 = st1 
        
      
        st3 = self.get_pair_val(msg.buttons[10], msg.buttons[11])
        st4 = st3

       
        relay = 2000 if msg.buttons[6] else 1500

        
        packet = f"s[{s1},{s2},{d1},{d2},{st1},{st2},{st3},{st4},{relay}]"

     
        self.get_logger().info(f"Published: {packet}")

       
        try:
            self.sock.sendto(packet.encode(), (self.target_ip, self.target_port))
        except Exception as e:
            self.get_logger().error(f"UDP Error: {e}")

def main(args=None):
    rclpy.init(args=args)
    node = JoyUdpBridge()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()