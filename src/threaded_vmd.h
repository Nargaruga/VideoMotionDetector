#ifndef THREADED_VMD_H
#define THREADED_VMD_H

#include "vmd.h"
#include "thread_pool.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>

class ThreadedVMD : public VMD {
public:
	ThreadedVMD(int nw = 1) : m_nw(nw), m_pool(nw) {};

    void run(std::string videoPath);
    void benchmarkRun(std::string videoPath, int tries);
	
private:
	int m_nw;
	ThreadPool<void> m_pool;
};

#endif
