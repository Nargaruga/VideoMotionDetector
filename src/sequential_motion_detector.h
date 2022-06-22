#ifndef SEQUENTIAL_MOTION_DETECTOR
#define SEQUENTIAL_MOTION_DETECTOR

#include <opencv2/opencv.hpp>
#include <string>

class SequentialMotionDetector {
public:
    void run(std::string videoPath);
    void benchmarkRun(std::string videoPath);

private:
    float threshold = 0.1;

    void toGrayScale(cv::Mat& img);
    void smooth(cv::Mat& img);
    bool checkFrame(cv::Mat& background, cv::Mat& frame);

};

#endif
