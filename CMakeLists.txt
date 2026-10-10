cmake_minimum_required(VERSION 3.8)
project(ben_d91_autonom)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

set(ament_dependencies
  rclcpp
  std_msgs
  geometry_msgs
  sensor_msgs
)

add_executable(distance_sim_node src/distance_sim_node.cpp)
ament_target_dependencies(distance_sim_node ${ament_dependencies})

add_executable(emergency_brake_node src/emergency_brake_node.cpp)
ament_target_dependencies(emergency_brake_node ${ament_dependencies})

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

install(TARGETS
  distance_sim_node
  emergency_brake_node
  DESTINATION lib/${PROJECT_NAME})

ament_package()
