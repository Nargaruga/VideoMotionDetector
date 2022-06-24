#ifndef SEQ_VMD
#define SEQ_VMD

#include <opencv2/opencv.hpp>
#include <string>

class SequentialVMD{
public:
    void run(std::string videoPath);
    void benchmarkRun(std::string videoPath, int tries);

private:
    float threshold = 0.1;

    void toGrayScale(cv::Mat& img);
    void smooth(cv::Mat& img);
    bool compareFrame(const cv::Mat& background, const cv::Mat& frame);

};

#endif