#include "interfaceUtil.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <filesystem>
#include "stb_image.h"
using namespace std;


WindowInitializer winInit;
Widgets widget;

//setup
int current_image = 1;
int circle_state = 0;
int max_images = 10;
int button_width = 125;


int main() {
    // Load image
    

    //std::string base = std::filesystem::current_path().string();
    //std::cout << "Working directory: " << base << "\n";


    if (!glfwInit())
        return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    

    // Find screen resolution
    winInit.GetPrimaryMonitorResolution(windowVar::monitor_w, windowVar::monitor_h);
    windowVar::display_w = windowVar::monitor_w;
    windowVar::display_h = windowVar::monitor_h;
    

    

    winInit.Setup();

    GLFWwindow* window = glfwCreateWindow(windowVar::display_w, windowVar::display_h, "UI or something", nullptr, nullptr);
    if (window == nullptr)
        return 1;


   

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); 

    int my_image_width = 0;
    int my_image_height =    0;
    GLuint my_image_texture = 0;        
    std::string path = "/home/valdemar-linus/DroneBoys/images/jesper" + std::to_string(current_image) + ".jpeg";
    

    bool ret = LoadTextureFromFile(path.c_str(), &my_image_texture, &my_image_width, &my_image_height);
    IM_ASSERT(ret);


    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        int fb_w, fb_h;
        glfwGetFramebufferSize(window, &fb_w, &fb_h);
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();     
        
        //Page Control Button
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2(button_width ,200));
        ImGui::SetNextWindowPos(ImVec2(5, 5));
        ImGui::Begin("Image Control");
        if (ImGui::Button("next image", ImVec2(button_width -15, 20))) {;
            if (current_image <  max_images)
                current_image++;
                std::string path = "/home/valdemar-linus/DroneBoys/images/jesper" + std::to_string(current_image) + ".jpeg";
                bool ret = LoadTextureFromFile(path.c_str(), &my_image_texture, &my_image_width, &my_image_height);

        }
        if (ImGui::Button("previous image", ImVec2(button_width -15, 20)))  {;
            if (circle_state == 0){
                if (current_image > 1)
                    current_image--;
                    std::string path = "/home/valdemar-linus/DroneBoys/images/jesper" + std::to_string(current_image) + ".jpeg";
                    bool ret = LoadTextureFromFile(path.c_str(), &my_image_texture, &my_image_width, &my_image_height);
            }
            else if (circle_state == 1){
                if (current_image > 1)
                    current_image--;
                    printf("wow this sure is a temporary print fuction");
                    std::string path2 = "/home/valdemar-linus/DroneBoys/images/Jesper" + std::to_string(current_image) + ".jpeg";
                    bool ret = LoadTextureFromFile(path2.c_str(), &my_image_texture, &my_image_width, &my_image_height);
            }
        }
    
        if (ImGui::Button("method 2", ImVec2(button_width -15, 20)))  {;
            printf("wow this sure is a temporary print fuction");


        }
        ImGui::SetNextItemWidth(20.0f);
        ImGui::CheckboxFlags("Circle", (unsigned int*)&circle_state, 1);
        ImGui::End();

        //Image Window
        ImGui::SetNextWindowSize(ImVec2((float)fb_w * 0.98 -button_width, (float)fb_h * 0.7));
        ImGui::SetNextWindowPos(ImVec2(button_width+15, 5));
        ImGui::Begin("Image Goes Here");
        ImGui::Image((ImTextureID)(intptr_t)my_image_texture, ImVec2((float)fb_w * 0.96 -button_width, (float)fb_h * 0.68));
        ImGui::End();
        

        //description window
        ImGui::SetNextWindowSize(ImVec2((float)fb_w * 0.98 -button_width, (float)fb_h * 0.3 - 15));
        ImGui::SetNextWindowPos(ImVec2(button_width+15, (float)fb_h * 0.7+10));
        ImGui::Begin("Description");
        ImGui::Text("This is image %d", current_image);
        ImGui::Text("Current circle state is: %d", circle_state);
        ImGui::End();

        //printf(path.c_str());


        ImGui::Render();
        glViewport(0, 0, fb_w, fb_h);
        glClearColor(windowVar::BackgroundColor.x, windowVar::BackgroundColor.y, windowVar::BackgroundColor.z, windowVar::BackgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
   
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        

        glfwSwapBuffers(window);
        
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();    
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();





    return 0;   
} 