#include "utils.h"
#include "manipulator.h"

#include <filesystem>


using namespace std;
using namespace cv;
ImageProcessingPipeline pipeline;
NoiseReducer noiseReducer;
MorphologyProcessor morphologyProcessor;
ColorManipulator colorManipulator;
cv::Point nonBackground_point; // For storing non-background point in blob detection


cv::Mat field_coloredshirt(cv::Mat& image, const Config& cfg, int closed_connectivity) {
    /**
    Takes in an image, do a series of processing steps to detect red shirts in a field setting.
    First adjusts saturation, brightness/contrast, enhances red channel, applies median filtering,
    performs Canny edge detection, morphological operations, and blob detection. Finally, draws circles around detected blobs on the original image.
    */
    int radius = 15;
    int kernel_size = 1;
    int saturationScale = 2;
    int connectivity = 4;

    cv::Mat saturatedImage = colorManipulator.saturation(image, saturationScale);
    cv::Mat brightness_contrast_image = colorManipulator.brightnees_contrast(saturatedImage, cfg.brightness_contrast.contrast, cfg.brightness_contrast.brightness);
    cv::Mat RedEnhanced = colorManipulator.BGR_channel_changer(brightness_contrast_image, 0, 0);
    //cv::imshow("Red Enhanced Image", RedEnhanced);
    cv::Mat medianFiltered = noiseReducer.median_filter(RedEnhanced, cfg.median_filter.kernel_size);
    //cv::imshow("Median Filtered Image", medianFiltered);
    cv::Mat edges = canny_edge_detection(medianFiltered, cfg.canny_parameters.threshold.low_threshold, cfg.canny_parameters.threshold.max_threshold);
    //cv::imshow("Edges", edges);
    cv::Mat closedImage = morphologyProcessor.closing_morphology(edges, closed_connectivity);
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

void sky_sorted_coloredshirt(cv::Mat& image, const Config& cfg) {
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
 
    cout << "Number of blobs detected: " << blobs.numLabels -1 << endl;
    cv::imshow("Final Labeled Image", image);
    cv::waitKey(0);
}

//cv::Mat HOG(const cv::Mat& image) { //!! WORK IN PROGRESS
    /**
    Computes the Histogram of Oriented Gradients (HOG) features for the input image.
    */
    //cv::Mat grayImage;
    //cv::Size size = image.size();
    //std::cout << "Image size: " << size << std::endl;
    //cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
    //float Gradient_magnitude, Gradient_direction;
    //cv::Mat Gx, Gy;
    //int cell_size = 8;
    //cv::Sobel(grayImage, Gx, CV_32F, 1, 0, 3);
    //cv::Sobel(grayImage, Gy, CV_32F, 0, 1, 3);
    //for (int y = 0; y < grayImage.rows; y++) {
    //    for (int x = 0; x < grayImage.cols; x++) {
    //        float gx = Gx.at<float>(y, x);
    //        float gy = Gy.at<float>(y, x);
    //        float magnitude = sqrt(gx * gx + gy * gy);
    //        float direction = atan2(gy, gx) * (180.0f / CV_PI); // Convert to degrees
    //        if (direction < 0){
    //             direction += 180.0f;
    //        }
    //        int cellsX = grayImage.cols / cell_size;
    //        int cellsY = grayImage.rows / cell_size;
    //        
    //    }
   // }
//    return grayImage;
//}





int main() {
    double target_intensity = 200.0;
    // Load configuration file 
    std::string configPath = CONFIG_PATH;
    Config cfg = pipeline.loadConfig(configPath);
    // Load image
    string filename = "jesper3.jpeg";
    cv::Mat image = pipeline.fetch_image(filename);
    int radius = 15;

    int connectivity = 4;
    int saturationScale = 2;
    double scaleFactor = 0.204;
    //cv::Mat resized_image;
    //cv::resize(image, resized_image, cv::Size(), scaleFactor, scaleFactor, cv::INTER_LINEAR);
    //std::vector<uchar> buf;
    //std::vector<int> params = {
    //cv::IMWRITE_JPEG_QUALITY, 30   // try 20–40
    //};
//
    //cv::imencode(".jpg", resized_image, buf, params);
    //cv::Mat compressed = cv::imdecode(buf, cv::IMREAD_COLOR);
    //cv::GaussianBlur(compressed, compressed, cv::Size(3,3), 0.8);
    //cv::fastNlMeansDenoisingColored(resized_image, resized_image, 3, 3 , 7, 21);
    //cv::imshow("Original Image", resized_image);
    //cv::waitKey(0);

    //cv::Mat gammaCorrectedImage = noiseReducer.gamma_correction(image,find_gamma(image, target_intensity, determine_intensity(image)));
    //cv::Mat resized_image = noiseReducer.resize_image(gammaCorrectedImage, 0.3);
    //cv::imshow("Gamma corrected Image", resized_image);
    //cv::waitKey(0);
    // Process image
    //double average_intensity = determine_intensity(image);
    //cv:: Mat gammaCorrectedImage = noiseReducer.gamma_correction(image, find_gamma(image, target_intensity, average_intensity));
    //cv::imshow("Gamma Corrected Image", gammaCorrectedImage);
    //cv::waitKey(0);

    //image = sky_sorted_coloredshirt(image, cfg);
    //image = field_coloredshirt(compressed, cfg, 7);

    //!! HERE
    cv::Mat saturatedImage = colorManipulator.saturation(image, 8);
    cv::Mat gammaCorrectedImage = noiseReducer.gamma_correction(saturatedImage,find_gamma(saturatedImage, target_intensity, determine_intensity(saturatedImage)));
    cv::Mat medianFiltered = noiseReducer.median_filter(gammaCorrectedImage, 15);
    cv::Mat bilateralFiltered = noiseReducer.bilateral_filter(medianFiltered, 7, 150, 150);

    cv::Mat gray;
    cv::cvtColor(bilateralFiltered, gray, cv::COLOR_BGR2GRAY);

    cv::Mat edges = canny_edge_detection(gray, 200, 250);
    cv::Mat closedImage = morphologyProcessor.closing_morphology(edges, cfg.blob_detection.connectivity);
    cv::Mat openingImage = morphologyProcessor.opening_morphology(closedImage, 1);
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
    cv::imshow("satruration", saturatedImage);
    cv::imshow("saturated", gammaCorrectedImage);
    cv::imshow("median", medianFiltered);
    cv::imshow("edges", edges);
    cv::imshow("closed", closedImage);
    cv::imshow("opened", openingImage);
    cv::imshow("final", image);
    cv::waitKey(0);
    ////cv::imshow("WTF", edges);


    
    //pipeline.save_image(image, "output.jpg");
    ////cv::imshow("Saturation Adjusted Image", saturatedImage);
    


    //!! Tuning test
    //tuning(image, 3);
    return 0;

}


