#include "drone_core/utils.h"
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <queue>
#include <cstddef>
#include <vector>
#include <cmath>



using namespace std;
using namespace cv;
namespace fs = std::filesystem;
using Box = std::vector<std::vector<int>>; 

    
cv::Mat ImageProcessingPipeline::fetch_image(const std::string& filename) {
    fs::path current = fs::current_path();

    fs::path inputPath = current.parent_path() / "drone_boys_images" / "input" / filename;

    cv::Mat image = cv::imread(inputPath.string(), cv::IMREAD_COLOR);
    if (image.empty()) {
        std::cerr << "Error: Could not load image from: " << inputPath.string() << std::endl;
    } else {
        std::cout << "Loaded image: " << inputPath.string() << std::endl;
    }
    return image;
}

    Config ImageProcessingPipeline::loadConfig(const std::string& path) {
        Config cfg{};
        cv::FileStorage fs(path, cv::FileStorage::READ);
        if (!fs.isOpened()) {
            throw std::runtime_error("Cannot open config file: " + path);
        }


        fs["denoise"]["h"] >> cfg.denoise.h;
        fs["denoise"]["hColor"] >> cfg.denoise.hColor;
        fs["denoise"]["templateWindowSize"] >> cfg.denoise.templateWindowSize;
        fs["denoise"]["searchWindowSize"] >> cfg.denoise.searchWindowSize;

        fs["canny_parameters"]["threshold_low"] >> cfg.canny_parameters.threshold.low_threshold;
        fs["canny_parameters"]["threshold_max"] >> cfg.canny_parameters.threshold.max_threshold;
        fs["canny_parameters"]["threshold_low_dark"] >> cfg.canny_parameters.threshold.low_threshold_dark;
        fs["canny_parameters"]["threshold_max_dark"] >> cfg.canny_parameters.threshold.max_threshold_dark;

        fs["blob_detection"]["kernel_size"] >> cfg.blob_detection.kernel_size;
        fs["blob_detection"]["connectivity"] >> cfg.blob_detection.connectivity;
        fs["blob_detection"]["kernel_size_dark"] >> cfg.blob_detection.kernel_size_dark;

        fs["bilateral_filter"]["d"] >> cfg.bilateral_filter.d;
        fs["bilateral_filter"]["sigmaColor"] >> cfg.bilateral_filter.sigmaColor;
        fs["bilateral_filter"]["sigmaSpace"] >> cfg.bilateral_filter.sigmaSpace;
        fs["bilateral_filter"]["d_dark"] >> cfg.bilateral_filter.d_dark;
        fs["bilateral_filter"]["sigmaColor_dark"] >> cfg.bilateral_filter.sigmaColor_dark;
        fs["bilateral_filter"]["sigmaSpace_dark"] >> cfg.bilateral_filter.sigmaSpace_dark;

        fs["brightness_contrast"]["brightness"] >> cfg.brightness_contrast.brightness;
        fs["brightness_contrast"]["contrast"] >> cfg.brightness_contrast.contrast;

        fs["median_filter"]["kernel_size"] >> cfg.median_filter.kernel_size;
        fs["median_filter"]["kernel_size_dark"] >> cfg.median_filter.kernel_size_dark;

        fs["fast_parameters"]["threshold"] >> cfg.fast_parameters.threshold;
        fs["fast_parameters"]["nonmaxSuppression"] >> cfg.fast_parameters.nonmaxSuppression;

        fs.release();
        return cfg;
    }


BlobData ImageProcessingPipeline::blob_detection(const cv::Mat& inputImage, int connectivity)
{
    cv::Mat binary;
    // 1. Threshold image first
    cv::threshold(inputImage, binary, 128, 255, cv::THRESH_BINARY);

    // 2. Connected components
    cv::Mat labels, stats, centroids;
    int numLabels = cv::connectedComponentsWithStats(
        binary, labels, stats, centroids, connectivity, CV_32S);

    // Create feature vectors
    std::vector<double> areas(numLabels);
    std::vector<double> perimeters(numLabels);
    std::vector<double> circularities(numLabels);
    std::vector<double> aspect_ratios(numLabels);
    std::vector<double> inertia(numLabels);
    std::vector<double> solidity(numLabels);
    std::vector<double> eccentricity(numLabels);

    // 3. LOOP OVER BLOBS
    for (int i = 1; i < numLabels; i++)  // skip background label 0
    {
        // Area from stats
        double area = stats.at<int>(i, cv::CC_STAT_AREA);
        areas[i] = area;

        // Create mask for this blob
        cv::Mat compMask = (labels == i);

        // Find contour
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(compMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

        // Perimeter
        double perimeter = cv::arcLength(contours[0], true);
        perimeters[i] = perimeter;

        // Circularity
        double circularity = (4 * CV_PI * area) / (perimeter * perimeter + 1e-5); //
        circularities[i] = circularity;

        // Aspect ratio
        double w = stats.at<int>(i, cv::CC_STAT_WIDTH);
        double h = stats.at<int>(i, cv::CC_STAT_HEIGHT);
        aspect_ratios[i] = w / h;

        //Inertia
        cv::Moments mu = cv::moments(contours[0]);
        inertia[i] = (mu.mu20 + mu.mu02) / (area + 1e-5);

        {
            std::vector<cv::Point> hull;
            cv::convexHull(contours[0], hull);
            double hullArea = cv::contourArea(hull);
            solidity[i] = area / (hullArea + 1e-5);
        }


        {
            cv::RotatedRect ellipse = cv::fitEllipse(contours[0]);
            double a = ellipse.size.width / 2.0;  // semi-major axis
            double b = ellipse.size.height / 2.0; // semi-minor axis

            if(b > a) std::swap(a, b); // ensure a >= b

            eccentricity[i] = sqrt(1 - (b * b) / (a * a + 1e-8)); 
        }


    }

    // 4. Return all data
    return {labels,
        stats,
        centroids,
        numLabels,
        areas,
        perimeters,
        circularities,
        aspect_ratios,
        inertia,
        solidity,
        eccentricity,
         };
    
}


    cv::Mat ImageProcessingPipeline::draw_circles(const cv::Mat& inputImage, const cv::Point& centroids, int radius, int i) {

        cv::Mat output;
        inputImage.copyTo(output);
        cv::circle(output, centroids, radius, cv::Scalar(0, 0, 255), 4); // Red circle
        return output;

    }

     BlobData ImageProcessingPipeline::sky_sorting(const cv::Mat& inputImage, BlobData blobs, double percentageThreshold) {
         double yThreshold = inputImage.rows * percentageThreshold;

        BlobData data = blobs;
        cv::Mat newCentroids;
        cv::Mat newStats;
        int newNumLabels = 0;
        for (int i = 0; i < data.centroids.rows; ++i) {
                double y = data.centroids.at<double>(i, 1);
                if (y >= yThreshold) { // keep only those above threshold
                    newCentroids.push_back(data.centroids.row(i));
                    newStats.push_back(data.stats.row(i));
                    newNumLabels++;
                }
            }
        
        data.centroids = newCentroids;
        data.stats = newStats;
        data.numLabels = newNumLabels;
        return data;
    }

    
cv::Mat ImageProcessingPipeline::compression(const cv::Mat& inputImage){
   
    cv::Mat resized_image;
    cv::resize(inputImage, resized_image, cv::Size(1120, 746), cv::INTER_LINEAR);
    std::vector<uchar> buf;
    std::vector<int> params = {
    cv::IMWRITE_JPEG_QUALITY, 30   // try 20–40
    };

    cv::imencode(".jpg", resized_image, buf, params);
    cv::Mat compressed = cv::imdecode(buf, cv::IMREAD_COLOR);
    cv::GaussianBlur(compressed, compressed, cv::Size(3,3), 0.8);
    return compressed;
}

void ImageProcessingPipeline::save_image(const cv::Mat& image, const std::string& filename) {
    // Get current working directory (where the node runs)
    fs::path current = fs::current_path();

    // Go one level up and then into images/output
    fs::path outputPath = current.parent_path() / "drone_boys_images" / "output";

    // Ensure the directory exists
    if (!fs::exists(outputPath)) {
        fs::create_directories(outputPath);
    }

    fs::path filePath = outputPath / filename;

    if (!cv::imwrite(filePath.string(), image)) {
        std::cerr << "Error: Could not save image to: " << filePath.string() << std::endl;
    } else {
        std::cout << "Image saved to: " << filePath.string() << std::endl;
    }
}

cv::Mat resize_scale(const cv::Mat& inputImage) {
    cv::Mat outputImage;
    int height = inputImage.rows;
    int width = inputImage.cols;
    int resize_height = 1120;
    int resize_width = 746;
    cv::resize(inputImage, outputImage, cv::Size(resize_width, resize_height));


    //cv::resize(inputImage, outputImage, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
    return outputImage;
}

std::vector<int> group_points(const BlobData& blobs, double radius)
{
    const std::size_t N = blobs.numLabels;
    std::vector<int> labels(N, -1);
    std::vector<char> visited(N, 0);

    if (N <= 1) return labels;

    const double r2 = radius * radius;
    int current_label = 0;
    visited[0] = 1; // mark background as visited
    labels[0] = -1; // background label
    for (std::size_t i = 1; i < N; ++i) {
        if (visited[i]) continue;

        std::queue<std::size_t> q;
        q.push(i);
        visited[i] = 1;
        labels[i] = current_label;

        while (!q.empty()) {
            std::size_t idx = q.front();
            q.pop();

            // read centroid coordinates as plain doubles
            const double px = blobs.centroids.at<double>(idx, 0);
            const double py = blobs.centroids.at<double>(idx, 1);

            for (std::size_t j = 0; j < N; ++j) {
                if (visited[j]) continue;

                const double qx = blobs.centroids.at<double>(j, 0);
                const double qy = blobs.centroids.at<double>(j, 1);

                const double dx = qx - px;
                const double dy = qy - py;
                const double dist2 = dx * dx + dy * dy;

                if (dist2 <= r2) {
                    visited[j] = 1;
                    labels[j] = current_label;
                    q.push(j);
                }
            }
        }

        ++current_label;
    }

    return labels;
}

DetectionResult draw_square(const cv::Mat& image,
                    const std::vector<int>& group_labels,
                    const BlobData& blobs)
{
    cv::Mat output;
    image.copyTo(output);
    std::vector<Box> boxes_size;
    // Determine number of groups
    int max_group = *std::max_element(group_labels.begin(), group_labels.end());

    // Prepare bounding boxes
    std::vector<cv::Rect2d> boxes(max_group + 1, cv::Rect2d(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX));

    // Accumulate min/max for each group
    for (size_t i = 1; i < blobs.numLabels; ++i) {
        int g = group_labels[i];

        double x = blobs.centroids.at<double>(i, 0);
        double y = blobs.centroids.at<double>(i, 1);

        boxes[g].x      = std::min(boxes[g].x, x);
        boxes[g].y      = std::min(boxes[g].y, y);
        boxes[g].width  = std::max(boxes[g].width,  x);
        boxes[g].height = std::max(boxes[g].height, y);
    }

    // Draw rectangles
    for (int g = 0; g <= max_group; ++g) {
        if (boxes[g].width < 0) continue; // skip empty groups

        double minX = boxes[g].x;
        double minY = boxes[g].y;
        double maxX = boxes[g].width;
        double maxY = boxes[g].height;

        // Compute box size
        double w = maxX - minX;
        double h = maxY - minY;

        // Centroid of the group
        double cx = (minX + maxX) / 2.0;
        double cy = (minY + maxY) / 2.0;

        // Enforce minimum size
        w = std::max(w, 50.0);
        h = std::max(h, 50.0);

        cv::Point p1(cx - w / 2.0, cy - h / 2.0);
        cv::Point p2(cx + w / 2.0, cy + h / 2.0);
        Box box = {{p1.x, p1.y}, {p2.x, p2.y}};
        boxes_size.push_back(box);
        cv::rectangle(output, p1, p2, cv::Scalar(255, 0, 0), 2);
    }

    return {output, boxes_size};
}

    //!! Just for testing

void tuning(const cv::Mat& inputImage, int mode) {
    if (inputImage.empty()) {
        cerr << "Error: Input image is empty!" << endl;
        return;
    }

    // Clone original to avoid modification
    Mat image;
    if (inputImage.channels() == 3)
        cvtColor(inputImage, image, COLOR_BGR2GRAY);
    else
        image = inputImage.clone();
    int d = 9;
    int sigmaColor = 33;
    int sigmaSpace = 33;
    int lowThreshold = 50;
    int highThreshold = 150;
    int medianKernelSize = 3;
    int brightness = 100; // Trackbar value from 0 to 200
    int contrast = 100;   // Trackbar value from 0 to 200
    double alpha = contrast / 100.0; // Contrast control
    int beta = brightness - 100; 
    switch(mode) {
        case 0: {
            const string windowName = "Canny + Bilateral Filter Tuning";
            namedWindow(windowName, WINDOW_AUTOSIZE);

            createTrackbar("d", windowName, &d, 20);
            createTrackbar("sigmaColor", windowName, &sigmaColor, 150);
            createTrackbar("sigmaSpace", windowName, &sigmaSpace, 150);
            createTrackbar("Canny Low", windowName, &lowThreshold, 255);
            createTrackbar("Canny High", windowName, &highThreshold, 255);

            Mat bilateral, edges, output;

            cout << "Adjust sliders. Press ESC to exit." << endl;

            while (true) {
                // --- Bilateral filter ---
                bilateralFilter(image, bilateral, d > 0 ? d : 1, sigmaColor, sigmaSpace);
            
                // --- Canny ---
                Canny(bilateral, edges, lowThreshold, highThreshold);
            
                // --- Combine for visualization ---
                cvtColor(edges, output, COLOR_GRAY2BGR);
                addWeighted(output, 0.7, inputImage, 0.3, 0, output);
            
                imshow(windowName, output);
            
                // Exit on ESC key
                int key = waitKey(30);
                if (key == 27)  // ESC
                    break;
            }
        
            destroyWindow(windowName);
            break;
        }
        case 1: {   
            const string windowName = "Canny + median Filter Tuning";
            namedWindow(windowName, WINDOW_AUTOSIZE);
            createTrackbar("Median Kernel Size", windowName, &d, 20);
            createTrackbar("Canny Low", windowName, &lowThreshold, 255);
            createTrackbar("Canny High", windowName, &highThreshold, 255);
            Mat medianFiltered, edges, output;
            cout << "Adjust sliders. Press ESC to exit." << endl;
            while (true) {
                // --- Median filter ---
                medianBlur(image, medianFiltered, d % 2 == 0 ? d + 1 : d);
                // --- Canny ---
                Canny(medianFiltered, edges, lowThreshold, highThreshold);
                // --- Combine for visualization ---
                cvtColor(edges, output, COLOR_GRAY2BGR);
                addWeighted(output, 0.7, inputImage, 0.3, 0, output);
                imshow(windowName, output);
                // Exit on ESC key
                int key = waitKey(30);
                if (key == 27)  // ESC
                    break;
            }
            destroyWindow(windowName);  
            break;
        }
        case 2: {
            const string windowName = "brightness and contrast tuning";
            namedWindow(windowName, WINDOW_AUTOSIZE);
      
            createTrackbar("Brightness", windowName, &brightness, 200);
            createTrackbar("Contrast", windowName, &contrast, 200);
            createTrackbar("Canny Low", windowName, &lowThreshold, 255);
            createTrackbar("Canny High", windowName, &highThreshold, 255);
            Mat adjusted, adjustedBGR, edges, output;
            cout << "Adjust sliders. Press ESC to exit." << endl;
            while (true) {
                // Adjust brightness and contrast
                // Recompute alpha and beta from trackbar positions
                alpha = contrast / 100.0;
                beta = brightness - 100;
                // Brightness control
                image.convertTo(adjusted, -1, alpha, beta);
                // Ensure same number of channels as inputImage for addWeighted
                if (adjusted.channels() == 1 && inputImage.channels() == 3) {
                    cvtColor(adjusted, adjustedBGR, COLOR_GRAY2BGR);
                } else {
                    adjustedBGR = adjusted;
                }
                // Combine for visualization
                addWeighted(adjustedBGR, 0.7, inputImage, 0.3, 0, output);
                Canny(adjusted, output, lowThreshold, highThreshold);
                imshow(windowName, output);
                // Exit on ESC key
                int key = waitKey(30);
                if (key == 27)  // ESC
                    break;
            }
            destroyWindow(windowName);
            break;
        } 
        case 3: {
            const string windowName = "Saturation and Canny Tuning";
            namedWindow(windowName, WINDOW_AUTOSIZE);
            createTrackbar("Saturation Scale x100", windowName, &contrast, 300);
            createTrackbar("Canny Low", windowName, &lowThreshold, 255);
            createTrackbar("Canny High", windowName, &highThreshold, 255);
            Mat hsvImage, saturatedImage, edges, output;
            cout << "Adjust sliders. Press ESC to exit." << endl;
            while (true) {
                // Convert to HSV
                cvtColor(inputImage, hsvImage, COLOR_BGR2HSV);
                // Split channels
                vector<Mat> hsvChannels;
                split(hsvImage, hsvChannels);
                // Adjust saturation
                double saturationScale = contrast / 100.0;
                hsvChannels[1] *= saturationScale;
                // Merge back
                merge(hsvChannels, hsvImage);
                // Convert back to BGR
                cvtColor(hsvImage, saturatedImage, COLOR_HSV2BGR);
                // Convert to grayscale for Canny
                Mat graySaturated;
                cvtColor(saturatedImage, graySaturated, COLOR_BGR2GRAY);
                // Apply Canny
                Canny(graySaturated, edges, lowThreshold, highThreshold);
                // Combine for visualization
                cvtColor(edges, output, COLOR_GRAY2BGR);
                addWeighted(output, 0.7, inputImage, 0.3, 0, output);
                imshow(windowName, output);
                // Exit on ESC key
                int key = waitKey(30);
                if (key == 27)  // ESC
                    break;

            
        }
            destroyWindow(windowName);
            break;
        } 
        case 4: {
            const string windowName = "BGR Channel Changer and Canny Tuning";
            int scale = 10;           // initial trackbar position (center)
            int scale_slider = 10;    // trackbar value (0–20)
            const int scale_max = 20;
            namedWindow(windowName, WINDOW_AUTOSIZE);
            createTrackbar("Channel Index (0-B,1-G,2-R)", windowName, &d, 2);
            createTrackbar("Scale", windowName, &scale_slider, scale_max);
            createTrackbar("Canny Low", windowName, &lowThreshold, 350);
            createTrackbar("Canny High", windowName, &highThreshold, 350);
            Mat changedChannelImage, edges, output;
            cout << "Adjust sliders. Press ESC to exit." << endl;
            while (true) {
                // Split BGR channels
                vector<Mat> bgrChannels;
                split(inputImage, bgrChannels);
                // Adjust specified channel
                double scale = scale_slider / 10.0; // Scale from 0.0 to 2.0
                if (d >= 0 && d < 3) {
                    bgrChannels[d] *= scale;
                }
                // Merge back
                merge(bgrChannels, changedChannelImage);
                // Convert to grayscale for Canny
                Mat grayChanged;
                cvtColor(changedChannelImage, grayChanged, COLOR_BGR2GRAY);
                // Apply Canny
                Canny(grayChanged, edges, lowThreshold, highThreshold);
                // Combine for visualization
                cvtColor(edges, output, COLOR_GRAY2BGR);
                addWeighted(output, 0.7, inputImage, 0.3, 0, output);
                imshow(windowName, output);
                // Exit on ESC key
                int key = waitKey(30);
                if (key == 27)  // ESC
                    break;
            }
            destroyWindow(windowName);
            break;
        } 
    }
}

