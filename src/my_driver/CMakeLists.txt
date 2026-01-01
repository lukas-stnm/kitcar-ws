cmake_minimum_required(VERSION 3.8)
project(my_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(turtlesim REQUIRED)

# make a program out of the code
add_executable(simple_driver_node src/simple_driver.cpp)

# link libraries
ament_target_dependencies(simple_driver_node
  rclcpp
  geometry_msgs
  nav_msgs
  turtlesim
)

install(TARGETS
  simple_driver_node
  DESTINATION lib/${PROJECT_NAME}
)

ament_package()
