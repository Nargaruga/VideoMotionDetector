#ifndef VMD_H
#define VMD_H

#include <opencv2/opencv.hpp>
#include <string>

class VMD {
public:
	VMD() {};
	~VMD() {};

    virtual void run(std::string videoPath);
    virtual void benchmarkRun(std::string videoPath, int tries);

protected:
    float threshold = 0.1;

    void toGrayScale(cv::Mat& img);
    void smooth(cv::Mat& img);
    bool compareFrame(const cv::Mat& background, const cv::Mat& frame);

};
#endif 
