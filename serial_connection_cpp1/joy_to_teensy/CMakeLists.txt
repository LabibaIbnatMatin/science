cmake_minimum_required(VERSION 3.8)
project(joy_to_teensy)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 1. Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

# 2. Add the executable (Include BOTH .cpp files here)
# Note: Do not call add_executable twice for the same target name
add_executable(joy_node 
    src/joy_node.cpp 
    src/teensy_serial_backend.cpp
)

# 3. Specify C++ standard
target_compile_features(joy_node PUBLIC cxx_std_17)

# 4. Include directories (Tells the compiler where your .hpp files are)
target_include_directories(joy_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

# 5. Link ROS 2 dependencies
ament_target_dependencies(
  joy_node
  "rclcpp"
  "sensor_msgs"
)

# 6. Install the executable so 'ros2 run' can find it
install(TARGETS joy_node
  DESTINATION lib/${PROJECT_NAME})

# 7. Testing (Optional)
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

# 8. Export the package
ament_package()