cmake_minimum_required(VERSION 3.16)
project(drone)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(OpenCV REQUIRED)
find_package(ament_index_cpp REQUIRED)  
find_package(rosidl_default_generators REQUIRED)
if(NOT builtin_interfaces_FOUND)
message(FATAL_ERROR "Could not find builtin_interfaces package")
endif()

rosidl_generate_interfaces(${PROJECT_NAME}
  "action/FinderAction.action"
)


ament_export_dependencies(rosidl_default_runtime)
include_directories(include ${OpenCV_INCLUDE_DIRS})

add_executable(finder 
src/main.cpp 
src/interface.cpp 
src/manipulator.cpp 
src/utils.cpp
)

target_compile_features(finder PUBLIC cxx_std_20)

# Link OpenCV + ament_index_cpp
target_link_libraries(finder 
  ${OpenCV_LIBS}
  ament_index_cpp::ament_index_cpp
)

# Add rclcpp_action as dependency
ament_target_dependencies(finder 
  rclcpp
  rclcpp_action
  ament_index_cpp
)
rosidl_target_interfaces(finder ${PROJECT_NAME} "rosidl_typesupport_cpp")

install(TARGETS finder
    DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME}/
)

ament_package()
