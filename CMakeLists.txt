cmake_minimum_required(VERSION 3.16)
project(Visual)

# -------------------------------------------------------------
#                   FIND PACKAGES
# -------------------------------------------------------------
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# -------------------------------------------------------------
#                      VISUALLIB
# -------------------------------------------------------------
add_library(VisualLib 
    src/utils.cpp
    src/manipulator.cpp
)

target_include_directories(VisualLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(VisualLib PUBLIC cxx_std_20)
target_link_libraries(VisualLib PUBLIC ${OpenCV_LIBS})
add_definitions(-DCONFIG_PATH="${CMAKE_CURRENT_SOURCE_DIR}/config/params.yaml")

# -------------------------------------------------------------
#                      VISUAL APP
# -------------------------------------------------------------
add_executable(VisualApp src/main.cpp)

target_compile_features(VisualApp PUBLIC cxx_std_20)
target_link_libraries(VisualApp PRIVATE VisualLib ${OpenCV_LIBS})

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

    # Backends
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(ImGuiLib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(ImGuiLib PUBLIC glfw)


add_library(InterfaceLib
    src/interfaceUtil.cpp
)

target_include_directories(InterfaceLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
    ${glfw_SOURCE_DIR}/include
)

target_link_libraries(InterfaceLib PUBLIC
    ImGuiLib
    glfw
    OpenGL::GL
)

add_executable(InterfaceApp src/interface.cpp)

target_compile_features(InterfaceApp PUBLIC cxx_std_20)

target_link_libraries(InterfaceApp PRIVATE
    InterfaceLib
    ImGuiLib
    glfw
    OpenGL::GL
    ${OpenCV_LIBS}
)
