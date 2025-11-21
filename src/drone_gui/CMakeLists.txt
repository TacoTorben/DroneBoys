cmake_minimum_required(VERSION 3.16)
project(drone_gui)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

include(FetchContent)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG master
)
FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.4
)
FetchContent_MakeAvailable(glfw)

add_library(ImGuiLib
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(ImGuiLib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(ImGuiLib PUBLIC glfw OpenGL::GL)

add_library(InterfaceLib 
src/interfaceUtil.cpp)

target_include_directories(InterfaceLib PUBLIC
    include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${glfw_SOURCE_DIR}/include
)
target_link_libraries(InterfaceLib PUBLIC ImGuiLib glfw OpenGL::GL)

add_executable(gui_app src/interface.cpp)
target_compile_features(gui_app PUBLIC cxx_std_20)
target_include_directories(gui_app PRIVATE ${OpenCV_INCLUDE_DIRS})

ament_target_dependencies(gui_app rclcpp std_msgs)

target_link_libraries(gui_app
    InterfaceLib
    ImGuiLib
    glfw
    OpenGL::GL
    ${OpenCV_LIBS}
)


install(TARGETS gui_app DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY config DESTINATION share/${PROJECT_NAME}/)

ament_export_dependencies(rclcpp std_msgs)
ament_package()
