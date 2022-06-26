#ifndef VMD_H
#define VMD_H

#include <opencv2/opencv.hpp>
#include <string>

class VMD {
public:
    void run(std::string videoPath);
    void benchmarkRun(std::string videoPath, int tries);

protected:
    float threshold = 0.1;

    void toGrayScale(cv::Mat& img);
    void smooth(const cv::Mat& kernel, cv::Mat& img);
    bool compareFrame(const cv::Mat& background, const cv::Mat& frame);

};

#endif
