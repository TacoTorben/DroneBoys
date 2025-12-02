#include "utils.h"
#include "manipulator.h"

#include <filesystem>

using namespace std;
using namespace cv;
namespace fs = std::filesystem;
extern ImageProcessingPipeline pipeline;
extern NoiseReducer noiseReducer;
extern MorphologyProcessor morphologyProcessor;
extern ColorManipulator colorManipulator;


    cv::Mat ImageProcessingPipeline::fetch_image(const std::string& filename) {

        fs::path imagePath = fs::current_path() / "../images" / filename;

        cv::Mat img = cv::imread(imagePath.string());

        if (img.empty()) {
            cerr << "Error: Could not load image: " << filename << endl;
        }


        return img;
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

        fs["blob_detection"]["kernel_size"] >> cfg.blob_detection.kernel_size;
        fs["blob_detection"]["connectivity"] >> cfg.blob_detection.connectivity;

        fs["bilateral_filter"]["d"] >> cfg.bilateral_filter.d;
        fs["bilateral_filter"]["sigmaColor"] >> cfg.bilateral_filter.sigmaColor;
        fs["bilateral_filter"]["sigmaSpace"] >> cfg.bilateral_filter.sigmaSpace;

        fs["brightness_contrast"]["brightness"] >> cfg.brightness_contrast.brightness;
        fs["brightness_contrast"]["contrast"] >> cfg.brightness_contrast.contrast;

        fs["median_filter"]["kernel_size"] >> cfg.median_filter.kernel_size;

        fs.release();
        return cfg;
    }



    BlobData ImageProcessingPipeline::blob_detection(const cv::Mat& inputImage, int connectivity) {

        cv::Mat binary;
        // Threshold to ensure binary
        cv::threshold(inputImage, binary, 128, 255, cv::THRESH_BINARY);


        cv::Mat labels, stats, centroids;
        int numLabels = cv::connectedComponentsWithStats(binary, labels, stats, centroids, connectivity, CV_32S);

        return {labels, stats, centroids, numLabels};
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

    void ImageProcessingPipeline::save_image(const cv::Mat& image, const std::string& filename) {
        fs::path outputPath = fs::current_path() / "../output" / filename;
        if (!cv::imwrite(outputPath.string(), image)) {
            cerr << "Error: Could not save image to: " << outputPath.string() << endl;
        }
    }
//!! Just for testing

void tuning(const cv::Mat& inputImage, int mode) {
    if (inputImage.empty()) {
        cerr << "Error: Input image is empty!" << endl;
        return;
    }

    // Clone original to avoid modification
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
       
        case 2: {
            Mat image = inputImage.clone();  // Local copy for this case
            Mat bilateral;
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
            Mat image = inputImage.clone();  // Local copy for this case

            const string windowName = "Redshirt altitude test";
            namedWindow(windowName, WINDOW_AUTOSIZE);

            // Trackbars (modern, safe)
            createTrackbar("Canny Low",      windowName, NULL, 350);
            createTrackbar("Canny High",     windowName, NULL, 350);
            createTrackbar("Connectivity",   windowName, NULL, 8);

            int kernel_size = 1;   // opening kernel
            int radius = 15;       // preserved but unused
            int saturationScale = 2; 
            Mat edges, closedImage, openingImage;

            while (true)
            {
                // --- Get trackbar values ---
                int lowThreshold    = getTrackbarPos("Canny Low",    windowName);
                int highThreshold   = getTrackbarPos("Canny High",   windowName);
                int connectivity    = getTrackbarPos("Connectivity", windowName);
               
                if (connectivity < 1) connectivity = 1;
                Mat resized = noiseReducer.resize_image(image, 0.204);
                // --- Your processing pipeline (unchanged) ---
                cv::Mat saturatedImage =
                    colorManipulator.saturation(resized, saturationScale);  // FIXED to "2" permanently

                cv::Mat brightness_contrast_image =
                    colorManipulator.brightnees_contrast(
                        saturatedImage,
                        126,
                        200);

                cv::Mat RedEnhanced =
                    colorManipulator.BGR_channel_changer(
                        brightness_contrast_image, 0, 0);

                cv::Mat medianFiltered =
                    noiseReducer.median_filter(
                        RedEnhanced,
                        7);

                // --- Canny (option B: returns Mat) ---
                edges = canny_edge_detection(
                            medianFiltered,
                            lowThreshold,
                            highThreshold);

                if (edges.empty()) {
                    std::cerr << "Edges empty — skipping morphology.\n";
                    continue;
                }

                // --- Morphology ---
                closedImage =
                    morphologyProcessor.closing_morphology(edges, connectivity);

                openingImage =
                    morphologyProcessor.opening_morphology(closedImage, 1);

                // Display final tuned blob-mask (resized)
                
                imshow(windowName, openingImage);

                // Exit on ESC
                int key = waitKey(30);
                if (key == 27)
                    break;
                }
                destroyWindow(windowName);
                break;
            }
             
       
        case 5: {
            Mat image = inputImage.clone();  // Local copy for this case
            double target_intensity = 200.0;
             int kernel_size = 1;
            int connectivity = 4;
            int saturationScale = 2;
            const string windowName = "Redshirt altitude test";
            namedWindow(windowName, WINDOW_AUTOSIZE);

            // Trackbars (modern style)
            createTrackbar("Canny Low",  windowName, NULL, 350);
            createTrackbar("Canny High", windowName, NULL, 350);
            createTrackbar("Morph kernel (d)", windowName, NULL, 20);

            Mat edges, closedImage, openingImage;

            while (true)
            {
                // --- Read trackbar values every frame ---
                int lowThreshold  = getTrackbarPos("Canny Low",  windowName);
                int highThreshold = getTrackbarPos("Canny High", windowName);
                int d             = getTrackbarPos("Morph kernel (d)", windowName);

                // Avoid invalid morphology kernel sizes
                if (d < 1) d = 1;

                // --- Your image processing chain ---r3
                Mat saturated     = colorManipulator.saturation(image, saturationScale);
                Mat gammaCorrected = noiseReducer.gamma_correction(
                        saturated,
                        find_gamma(saturated, target_intensity, determine_intensity(saturated))
                    );

                Mat medianFiltered     = noiseReducer.median_filter(gammaCorrected, 15);
                Mat bilateralFiltered  = noiseReducer.bilateral_filter(medianFiltered, 7, 150, 150);

                Mat gray;
                cv::cvtColor(bilateralFiltered, gray, cv::COLOR_BGR2GRAY);

                // --- FIXED: canny writes into edges ---
                edges = canny_edge_detection(gray, lowThreshold, highThreshold);

                // Safety check
                if (edges.empty())
                {
                    std::cerr << "Edges image empty — skipping morphology.\n";
                    continue;
                }

                // --- Morphology pipeline on edges only ---
                closedImage  = morphologyProcessor.closing_morphology(edges, d);
                openingImage = morphologyProcessor.opening_morphology(closedImage, 1);

                // Resize final blob image
                Mat resized = noiseReducer.resize_image(openingImage, 1);

                imshow(windowName, resized);

                // Exit on ESC
                int key = waitKey(30);
                if (key == 27)
                    break;
            
                    }
                    destroyWindow(windowName);
                    break;
                }
            }
        }

bool sky_edge_found(const cv::Mat& image,  cv::Point2d centroid, int searchRadius) { //!! Not happy about how works
    int startX = std::max(0, static_cast<int>(centroid.x) - searchRadius);
    int endX = std::min(image.cols - 1, static_cast<int>(centroid.x) + searchRadius);
    int y = static_cast<int>(centroid.y);
    int x = static_cast<int>(centroid.x);
    cv::Mat hsvImage;
    cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);
    int averageHue = 0;
    int count = 0;
    int averageSaturation = 0;
    int averageValue = 0;
    //for (int x = startX; x <= endX; ++x) {
    //     cv::Vec3b hsvPixel = hsvImage.at<cv::Vec3b>(y, x);
    //    
    //    // Assuming sky is predominantly blue; adjust thresholds as needed
//
    //    int H = hsvPixel[0];
    //    int S = hsvPixel[1];
    //    int V = hsvPixel[2];
//
    // 
    //   if (H > 70 && H < 160 && S > 50 && V > 20) { // Example thresholds
    //        return true; // Sky edge found
    //    }
    //}
    for (int dx = -searchRadius; dx <= searchRadius; ++dx) {
        for (int dy = -searchRadius; dy <= searchRadius; ++dy) {
            
            // Circle condition
            if (dx*dx + dy*dy > searchRadius*searchRadius)
                continue; // outside circle

            int cx = x + dx;
            int cy = y + dy;

            if (cx < 0 || cx >= image.cols || cy < 0 || cy >= image.rows)
                continue;

            cv::Vec3b hsvPixel = hsvImage.at<cv::Vec3b>(cy, cx);
            int H = hsvPixel[0];
            int S = hsvPixel[1];
            int V = hsvPixel[2];
            averageHue += H;
            count++;
            averageSaturation += S;
            averageValue += V;

        }
    }
        int avgH = averageHue / count;
        int avgS = averageSaturation / count;
        int avgV = averageValue / count;
        if (avgH > 50 && avgH < 180 && avgS > 90 && avgV > 50) {
            return true; // Sky edge found
        }
       
    return false; // No edge found

}