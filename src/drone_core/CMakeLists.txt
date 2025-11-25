cmake_minimum_required(VERSION 3.16)
project(drone_core)

# ROS2
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ament_index_cpp REQUIRED)

# Actions
rosidl_generate_interfaces(${PROJECT_NAME}
  "action/FinderAction.action"
)

ament_export_dependencies(rosidl_default_runtime)

# Include directories
include_directories(
    include
    ${OpenCV_INCLUDE_DIRS}
)

# Core library
add_library(drone_core_lib
    src/utils.cpp
    src/manipulator.cpp
)
target_link_libraries(drone_core_lib PUBLIC ${OpenCV_LIBS})
target_compile_features(drone_core_lib PUBLIC cxx_std_20)

# Main finder node
add_executable(finder
    src/main.cpp
    src/manipulator.cpp
    src/utils.cpp
)

target_compile_features(finder PUBLIC cxx_std_20)

target_link_libraries(finder
    drone_core_lib
    ${OpenCV_LIBS}
    ament_index_cpp::ament_index_cpp
)

ament_target_dependencies(finder
    rclcpp
    rclcpp_action
    ament_index_cpp
)

rosidl_target_interfaces(finder
    ${PROJECT_NAME} "rosidl_typesupport_cpp"
)

# Install
install(
    DIRECTORY include/
    DESTINATION include
)

install(
    TARGETS drone_core_lib finder
    DESTINATION lib/${PROJECT_NAME}
)

install(
    DIRECTORY config
    DESTINATION share/${PROJECT_NAME}/
)


ament_package()
