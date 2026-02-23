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

    def joy_callback(self, msg):
        try:
            # Note: Joy msg uses .buttons (list) and .axes (list)
            # Ensure your controller is connected or these indices will throw an IndexError
            
            # S-Buttons
            s1 = 2000 if msg.buttons[0] else 1500
            s2 = 2000 if msg.buttons[1] else 1500 

            # D-Buttons
            d11 = 2000 if msg.buttons[2] else 1500
            d12 = 1000 if msg.buttons[3] else 1500   

            # D2-Buttons
            d21 = 2000 if msg.buttons[11] else 1500
            d22 = 1000 if msg.buttons[5] else 1500
            relay = 2000 if msg.buttons[7] else 1500
            relay1 = 2000 if msg.buttons[6] else 1500
            btn_8= 2000 if msg.buttons[8] else 1500

            # Steering/Axis Logic
            # Check if axis index 4 and button 9 exist on your specific controller
            axis_val = msg.axes[3] if len(msg.axes) > 4 else 0.0
            btn_9 = msg.buttons[8] if len(msg.buttons) > 9 else 0
            btn_10= msg.buttons[9] if len(msg.buttons) > 10 else 0
    
            btn_11= msg.buttons[10] if len(msg.buttons) > 11 else 0
            btn_12 = msg.buttons[11] if len(msg.buttons) > 12 else 0
            


        # st_pos_1 = 2000 if axis_val == 1.0 and btn_9 else 1500


            st_pos_1 = 1500
            st_pos_2 = 1500
            st_pos_3 = 1500
            st_pos_4 = 1500

            
            if btn_9:
                if axis_val == 1.0:
                    st_pos_1 = 2000
                elif axis_val == -1.0:
                    st_pos_1 = 1000 
                else:
                    st_pos_1 = 1500
              

            
            if btn_10:
                if axis_val == 1.0:
                    st_pos_2 = 2000
                elif axis_val == -1.0:
                    st_pos_2 = 1000 
                else:
                    st_pos_2 = 1500
              
            if btn_11:
                if axis_val == 1.0:
                    st_pos_3 = 2000
                elif axis_val == -1.0:
                    st_pos_3 = 1000 
                else:
                    st_pos_3 = 1500
            
            if btn_12:
                if axis_val == 1.0:
                    st_pos_4 = 2000
                elif axis_val == -1.0:
                    st_pos_4 = 1000 
                else:
                    st_pos_4 = 1500
              


          


            
            # Assigning to your specific variables
            
            
            # Relay
          

            # Constructing the packet
            # Format: s[val1,val2,...]
            packet_data = [
                s1, s2, d11, d12, d21, d22,
                st_pos_1, st_pos_2, st_pos_3, st_pos_4,
                relay,relay1
            ]
            
            # Convert list to comma-separated string inside s[]
            packet = f"s[{','.join(map(str, packet_data))}]"

            self.get_logger().info(f"Published: {packet}")
            self.sock.sendto(packet.encode(), (self.target_ip, self.target_port))

        except IndexError as e:
            self.get_logger().warn(f"Index Error: Check if your controller has enough buttons/axes. {e}")
        except Exception as e:
            self.get_logger().error(f"UDP Error: {e}")

def main(args=None):
    rclpy.init(args=args)
    node = JoyUdpBridge()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
