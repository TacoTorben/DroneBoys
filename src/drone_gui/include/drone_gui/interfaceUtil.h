#pragma once

#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include "imgui_internal.h"
#include <filesystem>
#include <opencv2/opencv.hpp>

using namespace cv;
namespace fs = std::filesystem;

namespace windowVar {
    extern int monitor_w;
    extern int monitor_h;
    extern int display_w;
    extern int display_h;
    extern ImVec4 BackgroundColor;
}

class WindowInitializer {
public:

    void GetPrimaryMonitorResolution(int& width, int& height);
    void Setup();
    void UpdateWindowSize();

    void Render();
    void DrawMultiColor();
    void loadFonts();     // load all fonts once
    ImFont* getFont(int size);

private:
    ImFont* font18 = nullptr;
    ImFont* font24 = nullptr;
    ImFont* font28 = nullptr;
    ImFont* font40 = nullptr;
};
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
ImU32 DarkenColor(ImU32 col, float factor);
class Widgets {
public:
    bool costum_square_button(const char* id, ImVec2 pos, ImVec2 size, ImFont* font, float font_size, ImU32 color);
    bool costum_round_button(ImVec2 center, float radius, int segments, ImU32 color);
    bool DrawCircleGradientButton(ImDrawList* draw_list, ImFont* font, float scale, ImVec2 center, float radius, const char* id, float font_size);
};

bool LoadTextureFromMemory(const void* data, size_t data_size, GLuint* out_texture, int* out_width, int* out_height);
bool LoadTextureFromFile(const char* file_name, GLuint* out_texture, int* out_width, int* out_height);

cv::Mat fetch_image(const std::string& filename);

cv::Mat fetch_image_output(const std::string& filename);