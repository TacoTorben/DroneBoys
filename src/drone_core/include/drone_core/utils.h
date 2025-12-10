#pragma once
#include <string>
#include <opencv2/opencv.hpp>
#include <vector>
#include <map>
#include <cmath>

using namespace cv;

struct CannyParameters {
    struct threshold {
        double low_threshold;
        double max_threshold;
        double low_threshold_dark;
        double max_threshold_dark;
    } threshold;
};
struct DenoiseConfig {
    int h;
    int hColor;
    int templateWindowSize;
    int searchWindowSize;
};
struct BlobDetectionConfig {
    int kernel_size;
    int connectivity;
    int kernel_size_dark;
};

struct BilateralFilterConfig {
    int d;
    int sigmaColor;
    int sigmaSpace;
    int d_dark;
    int sigmaColor_dark;
    int sigmaSpace_dark;
};

struct BrightnessContrastConfig {
    int brightness;
    int contrast;
};

struct MedianFilterConfig {
    int kernel_size;
    int kernel_size_dark;
};

struct FastParameters {
    int threshold;
    bool nonmaxSuppression;
};


struct Config {
    DenoiseConfig denoise;
    CannyParameters canny_parameters;
    BlobDetectionConfig blob_detection;
    BilateralFilterConfig bilateral_filter;
    BrightnessContrastConfig brightness_contrast;
    MedianFilterConfig median_filter;
    FastParameters fast_parameters;
};






struct BlobData {
    cv::Mat labels;      // CV_32S matrix of labels
    cv::Mat stats;       // CV_32S matrix of stats
    cv::Mat centroids;   // CV_64F matrix of centroids
    int numLabels;
    std::vector<double> areas;
    std::vector<double> perimeters;
    std::vector<double> circularities;
    std::vector<double> aspect_ratios;
    std::vector<double> inertia;
    std::vector<double> solidity;
    std::vector<double> eccentricity;
};

class ImageProcessingPipeline {
    public:
        Config loadConfig(const std::string& path); 
        cv::Mat fetch_image(const std::string& filename);

        BlobData blob_detection(const cv::Mat& inputImage, int connectivity = 8);

        cv::Mat draw_circles(const cv::Mat& inputImage, const cv::Point& centroids, int radius, int i = 0);

        BlobData sky_sorting(const cv::Mat& inputImage, BlobData blobs, double yThreshold);

        void save_image(const cv::Mat& image, const std::string& filename);

        cv::Mat compression(const cv::Mat& inputImage);

};
void tuning(const cv::Mat& inputImage, int mode = 0);

