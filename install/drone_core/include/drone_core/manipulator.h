#pragma once
#include <opencv2/opencv.hpp>

class NoiseReducer {
    public:
        cv::Mat denoise_algorithm(const cv::Mat& inputImage, int h = 10, int hColor = 10, int templateWindowSize = 7, int searchWindowSize = 21);

        cv::Mat median_filter(const cv::Mat& inputImage, int kernelSize = 3);

        cv::Mat bilateralFilter(const cv::Mat& inputImage, int d = 9, double sigmaColor = 75, double sigmaSpace = 75);

};

class MorphologyProcessor {
    public:
        cv::Mat erosion_morphology(const cv::Mat& inputImage, int kernel_size);

        cv::Mat closing_morphology(const cv::Mat& inputImage, int kernelSize = 5);

        cv::Mat opening_morphology(const cv::Mat& inputImage, int kernel_size);
};

class ColorManipulator {
    public:
        cv::Mat brightnees_contrast(const cv::Mat& inputImage, double contrast, int brightness);

        cv::Mat saturation(const cv::Mat& inputImage, double saturationScale);

        cv::Mat BGR_channel_changer(const cv::Mat& inputImage, int channelIndex, double scale);
       
};

cv::Mat canny_edge_detection(const cv::Mat& inputImage, double lowThreshold = 100, double highThreshold = 200);











