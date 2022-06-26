#ifndef FF_VMD_H
#define FF_VMD_H

#include "../vmd.h"

class FastFlowVMD : public VMD {
public:
    void run(std::string videoPath);
    void benchmarkRun(std::string videoPath, int tries);
	
private:
	void smooth(const cv::Mat& kernel, cv::Mat& img);
	void smoothTask(const cv::Mat& kernel, const cv::Mat& source, cv::Mat& dest, int row);

};

#endif
