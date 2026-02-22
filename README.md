ls /dev/input/js*
—>/dev/input/js0

This is neede to be checked while creating symlink
udevadm info -a -n /dev/input/js0 | grep -E "idVendor|idProduct"
udevadm info -a -n /dev/input/js0 | grep -E "idVendor|idProduct"


Creating symlink

sudo nano /etc/udev/rules.d/99-joystick.rules
This is needed to write out

SUBSYSTEM=="input", KERNEL=="js*", ATTRS{idVendor}=="046d", ATTRS{idProduct}=="c215", SYMLINK+="input/science"


Trigger the rule

sudo udevadm control --reload-rules
sudo udevadm trigger




How to run joystick 
ros2 run joy joy_node --ros-args -p dev:=/dev/input/science
for seeing the topic ros topic echo /joy

science :
ros2 run joy_to_udp joy_udp
serial:
ros2 run joy_to_teensy joy_node
