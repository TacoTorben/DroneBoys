#include "drone_gui/interfaceUtil.h"
#include "drone_gui/stb_image.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <filesystem>
#include <ament_index_cpp/get_package_share_directory.hpp>
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_action/rclcpp_action.hpp"
#include <thread>
#include <atomic>
#include "drone_core/action/finder_action.hpp" 
using FinderAction = drone_core::action::FinderAction;
using namespace std;


WindowInitializer winInit;
Widgets widget;

//setup
int current_image = 1;

int max_images = 10;
int button_width = 125;
int fb_w, fb_h;
//---------------- ROS2 Action Client --------------------
class GuiClient : public rclcpp::Node
{
public:
    using GoalHandleFinder = rclcpp_action::ClientGoalHandle<FinderAction>;

    GuiClient() : Node("gui_client")
    {
        client_ = rclcpp_action::create_client<FinderAction>(
            this,
            "/drone_command"   // Must match server name
        );
    }

    void send_command(const std::string &command, std::vector<int32_t> target_pose)
    {
        if (!client_->wait_for_action_server(std::chrono::seconds(2)))
        {
            RCLCPP_ERROR(get_logger(), "Action server not available");
            return;
        }

        auto goal_msg = FinderAction::Goal();
        goal_msg.command_type = command;
        goal_msg.target_pose = target_pose;

        RCLCPP_INFO(get_logger(), "Sending goal: %s", command.c_str());

        auto options = rclcpp_action::Client<FinderAction>::SendGoalOptions();

        options.feedback_callback =
            [this](GoalHandleFinder::SharedPtr,
                   const std::shared_ptr<const FinderAction::Feedback> feedback)
            {
                RCLCPP_INFO(get_logger(), "Feedback: numb_labels = %d", feedback->numb_labels);
            };

        options.result_callback =
            [this](const GoalHandleFinder::WrappedResult &result)
            {
                RCLCPP_INFO(get_logger(), "Result success = %d", result.result->success);
            };

        client_->async_send_goal(goal_msg, options);
    }

private:
    rclcpp_action::Client<FinderAction>::SharedPtr client_;
};

void ros_thread_function(std::shared_ptr<GuiClient> node, std::atomic<bool> & running)
{
    rclcpp::executors::MultiThreadedExecutor exec;
    exec.add_node(node);

    while (running) {
        exec.spin_some();        // non-blocking
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    exec.cancel();
}
//---------------- End ROS2 Action Client --------------------
int main() {
    fs::path current = fs::current_path();
    std::string package_share_dir = current.parent_path() / "drone_boys_images";
    ImVec2 button_size(110,20);
        // Load image


        //std::string base = std::filesystem::current_path().string();
        //std::cout << "Working directory: " << base << "\n";
        rclcpp::init(0, nullptr);
        auto node = std::make_shared<GuiClient>();
        std::atomic<bool> running(true);
        std::thread ros_thread(ros_thread_function, node, std::ref(running));
    
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
        
        static bool circle_state = false;
        int my_image_width = 0;
        int my_image_height = 0;
        GLuint my_image_texture = 0;
        int my_alt_height = 0;
        int my_alt_width = 0;
        GLuint my_alt_texture = 0;
        static bool textures_loaded = false;
        
                // MAIN IMAGE
                
        std::filesystem::path path_main =
            std::filesystem::path(package_share_dir) /
            "input" /
            (std::to_string(current_image) + ".JPG");


        LoadTextureFromFile(
            path_main.string().c_str(),
            &my_image_texture,
            &my_image_width,
            &my_image_height
        );

        // ALT IMAGE
        std::filesystem::path path_alt =
            std::filesystem::path(package_share_dir) /
            "output" /
            (std::to_string(current_image) + ".JPG");

        LoadTextureFromFile(
            path_alt.string().c_str(),
            &my_alt_texture,
            &my_alt_width,
            &my_alt_height
        );
        
        bool ok_alt = LoadTextureFromFile(
            path_alt.string().c_str(),
            &my_alt_texture,
            &my_alt_width,
            &my_alt_height
        );
        



        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            glfwGetFramebufferSize(window, &fb_w, &fb_h);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();     

            //Page Control Button
            ImGui::NewFrame();
            ImGui::SetNextWindowSize(ImVec2(button_width ,200));
            ImGui::SetNextWindowPos(ImVec2(5, 5));
            ImGui::Begin("Image Control");
            if (ImGui::Button("next image",button_size)) {;
                if (current_image <  max_images) {
                    current_image++;
                    std::filesystem::path path_main =
                    std::filesystem::path(package_share_dir) / "input" / (std::to_string(current_image) + ".JPG");
                     LoadTextureFromFile(path_main.string().c_str(), &my_image_texture, &my_image_width, &my_image_height);
                                            // reload alternate image
                    std::filesystem::path path_alt =
                    std::filesystem::path(package_share_dir) / "output" / (std::to_string(current_image) + ".JPG");
                    LoadTextureFromFile(path_alt.string().c_str(), &my_alt_texture, &my_alt_width, &my_alt_height);
                }   
            }   
            if (ImGui::Button("previous image", button_size))  {;
                if (current_image > 1) {
                    current_image--;
                    std::filesystem::path path_main =
                    std::filesystem::path(package_share_dir) / "input" / (std::to_string(current_image) + ".JPG");
                    LoadTextureFromFile(path_main.string().c_str(), &my_image_texture, &my_image_width, &my_image_height);
                    // reload alternate image
                    std::filesystem::path path_alt =
                    std::filesystem::path(package_share_dir) / "output" / (std::to_string(current_image) + ".JPG");
                    LoadTextureFromFile(path_alt.string().c_str(), &my_alt_texture, &my_alt_width, &my_alt_height);}

            }
             if (ImGui::Button("F", button_size)) {;
                node->send_command("field", {1, 2, 3});

            }   
            if (ImGui::Button("Dark", button_size))  {;
                node->send_command("dark", {1, 2, 3});


            }
            ImGui::SetNextItemWidth(20.0f);
            ImGui::Checkbox("Circle", &circle_state);

        
            // Pick texture based on toggle state
            GLuint tex  = circle_state ? my_alt_texture : my_image_texture;

            ImGui::End(); // End control window

            // Image Window
            ImGui::SetNextWindowSize(ImVec2((float)fb_w * 0.98f - button_width, (float)fb_h * 0.7f));
            ImGui::SetNextWindowPos(ImVec2(button_width + 15, 5));
            ImGui::Begin("Image Goes Here");

            ImGui::Image(
                (ImTextureID)(intptr_t)tex,
                ImVec2((float)fb_w * 0.96f - button_width, (float)fb_h * 0.68f)
            );

            ImGui::End();



            //description window
            ImGui::SetNextWindowSize(ImVec2((float)fb_w * 0.98 -button_width, (float)fb_h * 0.3 - 15));
            ImGui::SetNextWindowPos(ImVec2(button_width+15, (float)fb_h * 0.7+10));
            ImGui::Begin("Description");
            ImGui::Text("This is image %d", current_image);
            ImGui::Text("Circle State: %d", circle_state);
            std::string current_path = circle_state ? path_alt.string() : path_main.string();
            ImGui::Text("Current Image Path: %s", current_path.c_str());
            ImGui::End();


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
        rclcpp::shutdown();
    


    
    return 0;   
    
}