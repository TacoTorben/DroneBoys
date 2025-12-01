#include "drone_core/utils.h"
#include "drone_core/manipulator.h"
#include <ament_index_cpp/get_package_share_directory.hpp>
#include "rclcpp/rclcpp.hpp"
#include <rclcpp_action/rclcpp_action.hpp>
#include "drone_core/action/finder_action.hpp"


#include <filesystem>
#include <iostream>
#include <string>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;
using DroneCommand = drone_core::action::FinderAction;
using GoalHandleDroneCommand = rclcpp_action::ServerGoalHandle<DroneCommand>;
ImageProcessingPipeline pipeline;
NoiseReducer noiseReducer;
MorphologyProcessor morphologyProcessor;
ColorManipulator colorManipulator;

cv::Point nonBackground_point; // For storing non-background point in blob detection

cv::Mat FAST_detector(cv::Mat& image, const Config& cfg, int fast_blackshirt) {
    int saturationScale = 8;
    float fast_gamma_value = 0.5;
    image = noiseReducer.gausian_filter(image, 3, 0.8, 0.8);
    if (fast_blackshirt)
    {
        cv::Mat saturatedImage = colorManipulator.saturation(image, saturationScale);
        cv::Mat gammaCorrectedImage = noiseReducer.gamma_correction(saturatedImage, fast_gamma_value);
        cv::Mat image = noiseReducer.bilateral_filter(gammaCorrectedImage, 9, 75, 75);
        cout << "FAST Blackshirt processing" << endl;
    }
    else {
        image = noiseReducer.gausian_filter(image, 5, 0, 0);
    }

    // Step 2: FAST detector setup
    auto detector = cv::FastFeatureDetector::create(
        cfg.fast_parameters.threshold,
        cfg.fast_parameters.nonmaxSuppression
    );

    // Step 3: Detect keypoints
    std::vector<cv::KeyPoint> keypoints;
    detector->detect(image, keypoints);

    // Step 4: Draw keypoints
    cv::Mat output;
    cv::drawKeypoints(image, keypoints, output, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);

    // Step 5: If we have keypoints, compute minimal enclosing circle
    if (!keypoints.empty()) {

        // Extract points into a vector<cv::Point2f>
        std::vector<cv::Point2f> pts;
        pts.reserve(keypoints.size());
        for (const auto &kp : keypoints) {
            pts.push_back(kp.pt);
        }

        // Compute minimal enclosing circle
        cv::Point2f center_f;
        float radius_f;
        cv::minEnclosingCircle(pts, center_f, radius_f);

        // Convert to int for drawing
        cv::Point center((int)center_f.x, (int)center_f.y);
        int radius = (int)radius_f;

        // Draw the circle (green)
        cv::circle(output, center, radius, cv::Scalar(0, 255, 0), 2);

        // Draw center point (blue)
        cv::circle(output, center, 3, cv::Scalar(255, 0, 0), -1);
    }

    
    return output;
}




cv::Mat field_coloredshirt(cv::Mat& image, const Config& cfg) {
    /**
    Takes in an image, do a series of processing steps to detect red shirts in a field setting.
    First adjusts saturation, brightness/contrast, enhances
     red channel, applies median filtering,
    performs Canny edge detection, morphological operations, and blob detection. Finally, draws circles around detected blobs on the original image.
    */
    int radius = 15;
    int kernel_size = 1;
    int connectivity = 4; // 4 for 30m 5 for 5m
    int saturationScale = 2;
    //int height = image.rows;
    //int width = image.cols;
    int resize_height = 1120;
    int resize_width = 746;
    cv::resize(image, image, cv::Size(resize_height, resize_width), cv::INTER_NEAREST);

    cv::Mat saturatedImage = colorManipulator.saturation(image, saturationScale);
    cv::Mat brightness_contrast_image = colorManipulator.brightnees_contrast(saturatedImage, cfg.brightness_contrast.contrast, cfg.brightness_contrast.brightness);
    cv::Mat RedEnhanced = colorManipulator.BGR_channel_changer(brightness_contrast_image, 0, 0);
    //cv::imshow("Red Enhanced Image", RedEnhanced);
    cv::Mat medianFiltered = noiseReducer.median_filter(RedEnhanced, cfg.median_filter.kernel_size);
    //cv::imshow("Median Filtered Image", medianFiltered);
    cv::Mat edges = canny_edge_detection(medianFiltered, cfg.canny_parameters.threshold.low_threshold, cfg.canny_parameters.threshold.max_threshold);
    //cv::imshow("Edges", edges);
    cv::Mat closedImage = morphologyProcessor.closing_morphology(edges, 5);
    //cv::imshow("Labeled Blobs", closedImage);
    cv::Mat openingImage = morphologyProcessor.opening_morphology(closedImage, kernel_size);
    //cv::imshow("Opening Morphology Image", openingImage);
    BlobData blobs = pipeline.blob_detection(openingImage, connectivity);

    
    //Draw circles around detected blobs (excluding background aka label 0)
    for(int i = 1; i < blobs.numLabels; ++i) {
        cv::Point2d centroid(
            blobs.centroids.at<double>(i, 0),
            blobs.centroids.at<double>(i, 1)
        );
        cv::Point center(static_cast<int>(centroid.x), static_cast<int>(centroid.y));
        //std::cout << "centroid y" << centroid.y << std::endl;
        image = pipeline.draw_circles(image, center, radius, i);
    }

    cout << "Number of blobs detected: " << blobs.numLabels -1 << endl;

    return image;
}

cv::Mat sky_sorted_coloredshirt(cv::Mat& image, const Config& cfg) { 
    /**
    Takes in an image, do a series of processing steps to detect red shirts in a field setting.
    First adjusts saturation, brightness/contrast, enhances red channel, applies median filtering,
    performs Canny edge detection, morphological operations, and blob detection then sorts blobs above a certain y-coordinate threshold. 
    Finally, draws circles around detected blobs on the original image.
    */
    int radius = 15;
    int kernel_size = 1;
    int connectivity = 4; 
    int saturationScale = 2;

    cv::Mat saturatedImage = colorManipulator.saturation(image, saturationScale);
    cv::Mat brightness_contrast_image = colorManipulator.brightnees_contrast(saturatedImage, cfg.brightness_contrast.contrast, cfg.brightness_contrast.brightness);
    cv::Mat RedEnhanced = colorManipulator.BGR_channel_changer(brightness_contrast_image, 0, 0);
    
    cv::Mat medianFiltered = noiseReducer.median_filter(RedEnhanced, cfg.median_filter.kernel_size);
    
    cv::Mat edges = canny_edge_detection(medianFiltered, cfg.canny_parameters.threshold.low_threshold, cfg.canny_parameters.threshold.max_threshold);
    
    cv::Mat closedImage = morphologyProcessor.closing_morphology(edges, cfg.blob_detection.connectivity);
   
    cv::Mat openingImage = morphologyProcessor.opening_morphology(closedImage, kernel_size);
    
    BlobData blobs = pipeline.blob_detection(openingImage, connectivity);
    BlobData skyBlobs = pipeline.sky_sorting(openingImage, blobs, 0.45);
    
    //Draw circles around detected blobs (excluding background aka label 0)
    for(int i = 1; i < skyBlobs.numLabels; ++i) {
        cv::Point2d centroid(
            skyBlobs.centroids.at<double>(i, 0),
            skyBlobs.centroids.at<double>(i, 1)
        );
        cv::Point center(static_cast<int>(centroid.x), static_cast<int>(centroid.y));
    
        image = pipeline.draw_circles(image, center, radius, i);
    }

    cout << "Number of blobs detected: " << skyBlobs.numLabels -1 << endl;
    return image;
}

cv::Mat dark_colorshirt(cv::Mat& image, const Config& cfg){ 
    int radius = 15;
    double target_intensity = 200.0;
    int connectivity = 4;
    int saturationScale = 8;
    cv::Mat saturatedImage = colorManipulator.saturation(image, saturationScale);
    cv::Mat gammaCorrectedImage = noiseReducer.gamma_correction(saturatedImage,find_gamma(saturatedImage, target_intensity, determine_intensity(saturatedImage)));
    cv::Mat medianFiltered = noiseReducer.median_filter(gammaCorrectedImage, cfg.median_filter.kernel_size_dark);
    
    cv::Mat bilateralFiltered = noiseReducer.bilateral_filter(medianFiltered, cfg.bilateral_filter.d_dark, cfg.bilateral_filter.sigmaColor_dark, cfg.bilateral_filter.sigmaSpace_dark);
    
    cv::Mat gray;
    cv::cvtColor(bilateralFiltered, gray, cv::COLOR_BGR2GRAY);

    cv::Mat edges = canny_edge_detection(gray, cfg.canny_parameters.threshold.low_threshold_dark, cfg.canny_parameters.threshold.max_threshold_dark);
 
    cv::Mat closedImage = morphologyProcessor.closing_morphology(edges, cfg.blob_detection.connectivity);
    cv::Mat openingImage = morphologyProcessor.opening_morphology(closedImage, cfg.blob_detection.kernel_size_dark);
    BlobData blobs = pipeline.blob_detection(openingImage, connectivity);

    
    //Draw circles around detected blobs (excluding background aka label 0)
    for(int i = 1; i < blobs.numLabels; ++i) {
        cv::Point2d centroid(
            blobs.centroids.at<double>(i, 0),
            blobs.centroids.at<double>(i, 1)
        );
        cv::Point center(static_cast<int>(centroid.x), static_cast<int>(centroid.y));
        //std::cout << "centroid y" << centroid.y << std::endl;
        image = pipeline.draw_circles(image, center, radius, i);
    }

    cout << "Number of blobs detected: " << blobs.numLabels -1 << endl;

    return image;


}

//-----------------ROS2 Action Server and Client Implementation-----------------
class CommandActionServer : public rclcpp::Node {
public:
    CommandActionServer() : Node("command_action_server") {
        action_server_ = rclcpp_action::create_server<DroneCommand>(
            this,
            "drone_command",
            std::bind(&CommandActionServer::handleDroneCommand, this, std::placeholders::_1, std::placeholders::_2),
            std::bind(&CommandActionServer::handleCancel, this, std::placeholders::_1),
            std::bind(&CommandActionServer::handleAccepted, this, std::placeholders::_1)
        );
    }

private:
    
   
    
    std::string config_path = ament_index_cpp::get_package_share_directory("drone_core") + "/config/params.yaml";
    Config cfg = pipeline.loadConfig(config_path);

    rclcpp_action::Server<DroneCommand>::SharedPtr action_server_;

    rclcpp_action::GoalResponse handleDroneCommand(
        const rclcpp_action::GoalUUID &,
        std::shared_ptr<const DroneCommand::Goal> goal)
    {
        static const std::vector<std::string> allowed_commands = {"field", "sky", "dark", "fast"};
        RCLCPP_INFO(get_logger(), "Received goal request: %s", goal->command_type.c_str());
        if (std::find(allowed_commands.begin(), allowed_commands.end(), goal->command_type) == allowed_commands.end()) {
            RCLCPP_WARN(get_logger(), "Rejected invalid command: %s", goal->command_type.c_str());
            return rclcpp_action::GoalResponse::REJECT;
        }
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
    }

    rclcpp_action::CancelResponse handleCancel(const std::shared_ptr<GoalHandleDroneCommand> goal_handle) {
        (void)goal_handle;
        RCLCPP_INFO(get_logger(), "Received cancel request");
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    void handleAccepted(const std::shared_ptr<GoalHandleDroneCommand> goal_handle) {
        std::thread([this, goal_handle]() { execute(goal_handle); }).detach();
    }

    void execute(const std::shared_ptr<GoalHandleDroneCommand> goal_handle) {
        const auto goal = goal_handle->get_goal();
        auto result = std::make_shared<DroneCommand::Result>();

        try {
            if (goal->method == "test") {
                if (goal->command_type == "field") {
                    int filename_number = 0;
                    for(int i = 0; i < 5; i++) {
                        for(int j = 1; j <= 10; j++) {

                            try {
                                fs::path current = fs::current_path();

                                fs::path inputPath = current.parent_path() / "drone_boys_images" / "test" / std::to_string(i) / (std::to_string(j) + ".JPG");
                                cv::Mat image = cv::imread(inputPath.string(), cv::IMREAD_COLOR);

                                if (image.empty()) {
                                    continue;
                                }

                                RCLCPP_INFO(get_logger(), "Processing image: %s", inputPath.string().c_str());

                                image = pipeline.compression(image);
                                cv::Mat processed_image =dark_colorshirt(image, cfg);
                                fs::path outputPath = current.parent_path() / "drone_boys_images" / "test" / "output";
                                fs::path filePath = outputPath / (std::to_string(filename_number) + ".JPG");
                                cv::imwrite(filePath.string(), image);
                                filename_number++;
                            }
                            catch(const std::exception &e) {
                                continue;
                            }

                        } 
                    }     

                } 
                 if (goal->command_type == "dark") {
                    int filename_number = 0;
                    for(int i = 0; i < 5; i++) {
                        for(int j = 1; j <= 10; j++) {

                            try {
                                fs::path current = fs::current_path();

                                fs::path inputPath = current.parent_path() / "drone_boys_images" / "test" / std::to_string(i) / (std::to_string(j) + ".JPG");
                                cv::Mat image = cv::imread(inputPath.string(), cv::IMREAD_COLOR);

                                if (image.empty()) {
                                    continue;
                                }

                                RCLCPP_INFO(get_logger(), "Processing image: %s", inputPath.string().c_str());

                                image = pipeline.compression(image);
                                cv::Mat processed_image = field_coloredshirt(image, cfg);
                                fs::path outputPath = current.parent_path() / "drone_boys_images" / "test" / "output";
                                fs::path filePath = outputPath / (std::to_string(filename_number) + ".JPG");
                                cv::imwrite(filePath.string(), image);
                                filename_number++;
                            }
                            catch(const std::exception &e) {
                                continue;
                            }

                        } 
                    }     

                } 
            } 

            
            if (goal->method == "training") {
                if (goal->command_type == "field") {
                    int img_numb = goal->image_info[0]; 
                    int output_numb = goal->image_info[1];
                    RCLCPP_INFO(get_logger(), "Processing image number: %d", img_numb);
                    cv::Mat image = pipeline.fetch_image(std::to_string(img_numb) + ".JPG");
                    image = pipeline.compression(image);
                    image = field_coloredshirt(image, cfg);
                    pipeline.save_image(image, std::to_string(output_numb) + ".JPG");
                    result->success = true;
                }
                if (goal->command_type == "sky") {
                    int img_numb = goal->image_info[0]; 
                    int output_numb = goal->image_info[1];
                    RCLCPP_INFO(get_logger(), "Processing image number: %d", img_numb);
                    cv::Mat image = pipeline.fetch_image(std::to_string(img_numb) + ".JPG");
                    image = pipeline.compression(image);
                    image = sky_sorted_coloredshirt(image, cfg);
                    pipeline.save_image(image, std::to_string(output_numb) + ".JPG");
                    result->success = true;
                }
                if (goal->command_type == "dark") {
                    int img_numb = goal->image_info[0]; 
                    int output_numb = goal->image_info[1];
                    RCLCPP_INFO(get_logger(), "Processing image number: %d", img_numb);
                    cv::Mat image = pipeline.fetch_image(std::to_string(img_numb) + ".JPG");
                    image = pipeline.compression(image);
                    image =dark_colorshirt(image, cfg);
                    pipeline.save_image(image, std::to_string(output_numb) + ".JPG");
                    result->success = true;
                }
                if (goal->command_type == "fast") {
                    int img_numb = goal->image_info[0]; 
                    int output_numb = goal->image_info[1];
                    int fast_blackshirt = goal->image_info[2];
                    RCLCPP_INFO(get_logger(), "Processing image number: %d", img_numb);
                    cv::Mat image = pipeline.fetch_image(std::to_string(img_numb) + ".JPG");
                    image = pipeline.compression(image);
                    image =FAST_detector(image, cfg, fast_blackshirt);
                    pipeline.save_image(image, std::to_string(output_numb) + ".JPG");
                    result->success = true;
                }
            }
            
            //if (goal->command_type = "test"){
            //    int map_numb = goal->image_info[0];
            //    case map_numb:
            //    //while loop around here woooooW
            //    cv::Mat image = pipeline.fetch_image(std::to_string(img_numb) + ".JPG"); //!!! NEED TO CHANGE FECTH FUNCTION
            //    image = pipeline.compression(image);
            //    image = field_coloredshirt(image, cfg);
            //    pipeline.save_image(image, std::to_string(output_numb) + ".JPG");
            //    result->success = true;
            //}
        } catch (const std::exception &e) {
            RCLCPP_ERROR(get_logger(), "Exception: %s", e.what());
            result->success = false;
        }

        goal_handle->succeed(result);
    }
};

int main(int argc, char **argv) {
    
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CommandActionServer>());
    rclcpp::shutdown();
    return 0;

}


