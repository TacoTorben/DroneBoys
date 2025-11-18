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


cv::Mat field_coloredshirt(cv::Mat& image, const Config& cfg) {
    /**
    Takes in an image, do a series of processing steps to detect red shirts in a field setting.
    First adjusts saturation, brightness/contrast, enhances red channel, applies median filtering,
    performs Canny edge detection, morphological operations, and blob detection. Finally, draws circles around detected blobs on the original image.
    */
    int radius = 15;
    int kernel_size = 1;
    int connectivity = 4;
    int saturationScale = 2;

    cv::Mat saturatedImage = colorManipulator.saturation(image, saturationScale);
    cv::Mat brightness_contrast_image = colorManipulator.brightnees_contrast(saturatedImage, cfg.brightness_contrast.contrast, cfg.brightness_contrast.brightness);
    cv::Mat RedEnhanced = colorManipulator.BGR_channel_changer(brightness_contrast_image, 0, 0);
    //cv::imshow("Red Enhanced Image", RedEnhanced);
    cv::Mat medianFiltered = noiseReducer.median_filter(RedEnhanced, cfg.median_filter.kernel_size);
    //cv::imshow("Median Filtered Image", medianFiltered);
    cv::Mat edges = canny_edge_detection(medianFiltered, cfg.canny_parameters.threshold.low_threshold, cfg.canny_parameters.threshold.max_threshold);
    //cv::imshow("Edges", edges);
    cv::Mat closedImage = morphologyProcessor.closing_morphology(edges, cfg.blob_detection.connectivity);
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

    cout << "Number of blobs detected: " << skyBlobs.numLabels -1 << endl;
    cv::imshow("Final Labeled Image", image);
    cv::waitKey(0);
}



int main() {
    
    // Load configuration file 
    std::string configPath = CONFIG_PATH;
    Config cfg = pipeline.loadConfig(configPath);
    // Load image
    string filename = "jesper2.jpeg";
    cv::Mat image = pipeline.fetch_image(filename);
   

    // Process image

    
    
   
    
    image = field_coloredshirt(image, cfg);
    cv::imshow("WTF", image);
    cv::waitKey(0);
    //sky_sorted_coloredshirt(image, cfg);
    
    pipeline.save_image(image, "output.jpg");
    ////cv::imshow("Saturation Adjusted Image", saturatedImage);
    


    //!! Tuning test
    //tuning(image, 4);
    return 0;

}


