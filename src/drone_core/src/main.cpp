#include "drone_core/utils.h"
#include "drone_core/manipulator.h"
#include <ament_index_cpp/get_package_share_directory.hpp>
#include "rclcpp/rclcpp.hpp"
#include <rclcpp_action/rclcpp_action.hpp>
#include "drone_core/action/finder_action.hpp"


#include <filesystem>
#include <fstream>
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

cv::Point nonBackground_point;


class seach_algorithm{

    public:
    
    cv::Mat preprocess_for_detection(const cv::Mat& input_image, int mode) {
        if (input_image.empty()) {
            throw std::invalid_argument("Input image is empty");
        }

        cv::Mat img = input_image.clone();

        // Apply different processing based on mode
        switch(mode) {
            case 0: {
                // Mode 0: Colored shirt - Only Gaussian Blur
                cv::GaussianBlur(img, img, cv::Size(5, 5), 0);
                break;
            }

            case 1: {
                // Mode 1: Black shirt - Full processing pipeline

                // Step 2: Saturation increase (scale = 8)
                cv::Mat hsv_image;
                cv::cvtColor(img, hsv_image, cv::COLOR_BGR2HSV);

                std::vector<cv::Mat> hsv_channels;
                cv::split(hsv_image, hsv_channels);

                hsv_channels[1].convertTo(hsv_channels[1], CV_32F);
                hsv_channels[1] *= 8.0;
                cv::threshold(hsv_channels[1], hsv_channels[1], 255, 255, cv::THRESH_TRUNC);
                hsv_channels[1].convertTo(hsv_channels[1], CV_8U);

                cv::merge(hsv_channels, hsv_image);
                cv::cvtColor(hsv_image, img, cv::COLOR_HSV2BGR);

                // Step 3: Gamma correction (gamma = 0.5)
                cv::Mat lut(1, 256, CV_8U);
                uchar* p = lut.ptr();
                for (int i = 0; i < 256; ++i) {
                    p[i] = cv::saturate_cast<uchar>(std::pow(i / 255.0, 0.5) * 255.0);
                }
                cv::LUT(img, lut, img);

                // Step 4: Bilateral filter
                cv::Mat output;
                cv::bilateralFilter(img, output, 9, 75.0, 75.0);
                img = output;
                break;
            }

            default:
                throw std::invalid_argument("Invalid mode. Use 0 for colored shirt or 1 for black shirt");
        }

        return img;
    }
    std::vector<cv::KeyPoint> fast_detector_with_circle(
        const cv::Mat& image, 
        cv::Point2f& center, 
        float& radius,
        int threshold = 60, 
        bool nonmax_suppression = true) 
    {
        if (image.empty()) {
            throw std::invalid_argument("Input image is empty");
        }

        // Initialize the FAST detector
        cv::Ptr<cv::FastFeatureDetector> fast = cv::FastFeatureDetector::create(threshold, nonmax_suppression);

        // Detect keypoints
        std::vector<cv::KeyPoint> keypoints;
        fast->detect(image, keypoints);

        // Calculate enclosing circle if keypoints found
        if (!keypoints.empty()) {
            // Extract (x, y) positions of keypoints
            std::vector<cv::Point2f> pts;
            pts.reserve(keypoints.size());
            for (const auto& kp : keypoints) {
                pts.push_back(kp.pt);
            }

            // Get minimal enclosing circle
            cv::minEnclosingCircle(pts, center, radius);
        } else {
            center = cv::Point2f(0, 0);
            radius = 0;
        }

        return keypoints;
    }

    cv::Mat fast_detector_visualize(const cv::Mat& image,const cv::Mat& original_image, int threshold = 60, bool nonmax_suppression = true)
    {
        cv::Point2f center;
        float radius;

        // Detect keypoints
        std::vector<cv::KeyPoint> keypoints = fast_detector_with_circle(
            image, center, radius, threshold, nonmax_suppression
        );
    
        // Draw keypoints on the image
        cv::Mat output_image;
        cv::drawKeypoints(original_image, keypoints, output_image, cv::Scalar(0, 0, 255));

        // Draw enclosing circle if keypoints found
        if (!keypoints.empty()) {
            cv::circle(output_image, 
                       cv::Point(static_cast<int>(center.x), static_cast<int>(center.y)), 
                       static_cast<int>(radius), 
                       cv::Scalar(0, 255, 0), 2);
            cv::circle(output_image, 
                       cv::Point(static_cast<int>(center.x), static_cast<int>(center.y)), 
                       3, 
                       cv::Scalar(255, 0, 0), -1);
        }
        cv::putText(output_image, //target image
                std::to_string(keypoints.size()), //text
                cv::Point(10, output_image.rows / 10), //top-left position
                cv::FONT_HERSHEY_DUPLEX,
                1.0,
                CV_RGB(255, 0, 0), //font color
                2);
        return output_image;
    }

    cv::Rect2f bounding_box(const std::vector<cv::KeyPoint>& keypoints) {
    if (keypoints.empty()) {
        return cv::Rect2f();
    }
    float min_x = keypoints[0].pt.x;
    float min_y = keypoints[0].pt.y;
    float max_x = keypoints[0].pt.x;
    float max_y = keypoints[0].pt.y;
    for (const auto& kp : keypoints) {
        float x = kp.pt.x;
        float y = kp.pt.y;
        if (x < min_x) min_x = x;
        if (y < min_y) min_y = y;
        if (x > max_x) max_x = x;
        if (y > max_y) max_y = y;
    }
    return cv::Rect2f(min_x, min_y, max_x - min_x, max_y - min_y);
}

    cv::Mat FAST_detector(cv::Mat& image, const Config& cfg, int fast_blackshirt)
        {
            int saturationScale = 8;
            float fast_gamma_value = 0.5;

            image = noiseReducer.gausian_filter(image, 3, 0.8, 0.8);

            //if (fast_blackshirt) {
            //    cv::Mat saturatedImage = colorManipulator.saturation(image, saturationScale);
            //    cv::Mat gammaCorrectedImage = noiseReducer.gamma_correction(saturatedImage, fast_gamma_value);
            //    image = noiseReducer.bilateral_filter(gammaCorrectedImage, 9, 75, 75);
//
            //    std::cout << "FAST Blackshirt processing" << std::endl;
            //} else {
            //    image = noiseReducer.gausian_filter(image, 5, 0, 0);
            //}

            // FAST detector setup
            auto detector = cv::FastFeatureDetector::create(
                cfg.fast_parameters.threshold,
                cfg.fast_parameters.nonmaxSuppression
            );

            // Detect keypoints
            std::vector<cv::KeyPoint> keypoints;
            detector->detect(image, keypoints);
            std::cout << "FAST keypoints: " << keypoints.size() << std::endl;


            // Draw keypoints into an output image
            cv::Mat output;
            cv::drawKeypoints(image, keypoints, output, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);

            // Draw bounding box on the same output image
            if (!keypoints.empty()) {
                cout << "dwadawd" << endl;
                cv::Rect2f box = bounding_box(keypoints);
                if (box.area() > 0) {
                    cv::rectangle(output, box, cv::Scalar(0, 0, 255), 2);
                }
            }
            return output;
        }


    cv::Mat field_coloredshirt(cv::Mat& image, const Config& cfg, int folder, int file){ 
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
        float threshold = 20.0f;
        
        fs::path current = fs::current_path();
        fs::path inputPath = current.parent_path() / "drone_boys_images";

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
        const fs::path csvPath = inputPath / ("data" + std::to_string(folder) + ".csv");
        bool needHeader = !fs::exists(csvPath) || fs::file_size(csvPath) == 0;
        std::ofstream out(csvPath.string(), std::ios::app);
        
        cout << "writing to: " << csvPath.string() << endl;
        if (!out.is_open()) {
            std::cerr << "Failed to open file for writing." << std::endl;
            return image;
        }
        
        if (needHeader) {
            out << "Folder,Image,Label,Area,Perimeter,Circularity,Aspect_Ratio,Inertia,Solidity,Eccentricity\n";
        }
        
        ////Draw circles around detected blobs (excluding background aka label 0)
        for (int i = 1; i < blobs.numLabels; ++i)
        {
            // centroid
            cv::Point2d c(
                blobs.centroids.at<double>(i, 0),
                blobs.centroids.at<double>(i, 1));
            
            cv::Point pos((int)c.x + 2, (int)c.y + 2);  // slight offset
            
            // Prepare the 3 lines
            std::string line1 = std::to_string(i);
            
            double scale = 3;   // very small text
            int thickness = 1;
      
            // Draw each line separately
            cv::putText(openingImage, line1, pos,
                        cv::FONT_HERSHEY_PLAIN, scale, cv::Scalar(255,255,255), thickness);
            
            //out << folder << "," 
            //    << file << ","
            //     << i << "," 
            //    << blobs.areas[i] << "," 
            //    << blobs.perimeters[i] << "," 
            //    << blobs.circularities[i] << "," 
            //    << blobs.aspect_ratios[i] << "," 
            //    << blobs.inertia[i] << "," 
            //    << blobs.solidity[i] << "," 
            //    << blobs.eccentricity[i] << "\n";
            //    << blobs.centroids.at<double>(i, 0) << ","
            //    << blobs.centroids.at<double>(i, 1) << "\n";
  
        }
        cv::putText(openingImage, "f = " + std::to_string(folder) + " i = " + std::to_string(file),cv::Point(10, openingImage.rows / 10),
                        cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255,255,255), 2);
        auto groups = group_blobs_by_vector_length(blobs, threshold);
        
        return openingImage;
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

        cv::putText(image, //target image
                std::to_string(blobs.numLabels -1), //text
                cv::Point(10, image.rows / 10), //top-left position
                cv::FONT_HERSHEY_DUPLEX,
                1.0,
                CV_RGB(255, 0, 0), //font color
                2);

        return image;
    }
};  

seach_algorithm sa;
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
                    for(int i = 0; i < 12; i++) {
                        for(int j = 1; j <= 81; j++) {

                            try {
                                fs::path current = fs::current_path();

                                fs::path inputPath = current.parent_path() / "drone_boys_images" / "test" / std::to_string(i) / (std::to_string(j) + ".JPG");
                                cv::Mat image = cv::imread(inputPath.string(), cv::IMREAD_COLOR);

                                if (image.empty()) {
                                    continue;
                                }

                                RCLCPP_INFO(get_logger(), "Processing image: %s", inputPath.string().c_str());

                                image = pipeline.compression(image);
                                cv::Mat processed_image = sa.field_coloredshirt(image, cfg, i, j);
                                fs::path outputPath = current.parent_path() / "drone_boys_images" / "test" / "output";
                                fs::path filePath = outputPath / (std::to_string(filename_number) + ".JPG");
                                cv::imwrite(filePath.string(), processed_image);
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
                    for(int i = 0; i < 12; i++) {
                        for(int j = 1; j <= 81; j++) {

                            try {
                                fs::path current = fs::current_path();

                                fs::path inputPath = current.parent_path() / "drone_boys_images" / "test" / std::to_string(i) / (std::to_string(j) + ".JPG");
                                cv::Mat image = cv::imread(inputPath.string(), cv::IMREAD_COLOR);

                                if (image.empty()) {
                                    continue;
                                }

                                RCLCPP_INFO(get_logger(), "Processing image: %s", inputPath.string().c_str());

                                image = pipeline.compression(image);
                                cv::Mat processed_image = sa.dark_colorshirt(image, cfg);
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
                if (goal->command_type == "fast") {
                    int filename_number = 0;
                    int mode = goal->image_info[2];
                    for(int i = 0; i < 12; i++) {
                        for(int j = 1; j <= 81; j++) {

                            try {
                                fs::path current = fs::current_path();

                                fs::path inputPath = current.parent_path() / "drone_boys_images" / "test" / std::to_string(i) / (std::to_string(j) + ".JPG");
                                cv::Mat image = cv::imread(inputPath.string(), cv::IMREAD_COLOR);
                                

                                if (image.empty()) {
                                    continue;
                                }

                                RCLCPP_INFO(get_logger(), "Processing image: %s", inputPath.string().c_str());

                                image = pipeline.compression(image);
                                cv::Mat original_image = image.clone();
                                image = sa.preprocess_for_detection(image, mode);
                                cv::Mat visualization = sa.FAST_detector(image, cfg, mode);
                                fs::path outputPath = current.parent_path() / "drone_boys_images" / "test" / "output";
                                fs::path filePath = outputPath / (std::to_string(filename_number) + ".JPG");
                                cv::imwrite(filePath.string(), visualization);
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
                    image = sa.field_coloredshirt(image, cfg, 0, img_numb);
                    pipeline.save_image(image, std::to_string(output_numb) + ".JPG");
                    result->success = true;
                }
                if (goal->command_type == "sky") {
                    int img_numb = goal->image_info[0]; 
                    int output_numb = goal->image_info[1];
                    RCLCPP_INFO(get_logger(), "Processing image number: %d", img_numb);
                    cv::Mat image = pipeline.fetch_image(std::to_string(img_numb) + ".JPG");
                    image = pipeline.compression(image);
                    image = sa.sky_sorted_coloredshirt(image, cfg);
                    pipeline.save_image(image, std::to_string(output_numb) + ".JPG");
                    result->success = true;
                }
                if (goal->command_type == "dark") {
                    int img_numb = goal->image_info[0]; 
                    int output_numb = goal->image_info[1];
                    RCLCPP_INFO(get_logger(), "Processing image number: %d", img_numb);
                    cv::Mat image = pipeline.fetch_image(std::to_string(img_numb) + ".JPG");
                    image = pipeline.compression(image);
                    image = sa.dark_colorshirt(image, cfg);
                    pipeline.save_image(image, std::to_string(output_numb) + ".JPG");
                    result->success = true;
                }
                if (goal->command_type == "fast") {
                    int img_numb = goal->image_info[0]; 
                    int output_numb = goal->image_info[1];
                    int mode = goal->image_info[2];
                    RCLCPP_INFO(get_logger(), "Processing image number: %d", img_numb);
                    cv::Mat image = pipeline.fetch_image(std::to_string(img_numb) + ".JPG");
                    image = pipeline.compression(image);
                    cv::Mat original_image = image.clone();
                    image = sa.preprocess_for_detection(image, mode);
                    cv::Mat visualization = sa.FAST_detector(image, cfg, mode);
                    pipeline.save_image(visualization, std::to_string(output_numb) + ".JPG");
                    result->success = true;
                }
            }
       
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


