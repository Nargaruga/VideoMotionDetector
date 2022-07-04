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
};
#endif 
