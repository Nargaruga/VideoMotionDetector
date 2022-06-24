#ifndef THREADED_VMD
#define THREADED_VMD

#include "blur_worker.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>

class ThreadedVMD{
public:
	ThreadedVMD(int nw = 1);

	~ThreadedVMD();

    void run(std::string videoPath);
    void benchmarkRun(std::string videoPath, int tries);

private:
    float m_threshold = 0.1;
	int m_nw;
	std::vector<BlurWorker> m_workers;

    void toGrayScale(cv::Mat& img);
    void smooth(cv::Mat& img);
	void smoothTask(const cv::Mat& img, cv::Mat& smoothed);
    bool compareFrame(const cv::Mat& background, const cv::Mat& frame);

};

#endif
