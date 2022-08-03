#ifndef THREADED_VMD_H
#define THREADED_VMD_H

#include "../vmd.h"
#include "../vmd_frame.h"
#include "thread_pool.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <thread>

/*
 * Class providing a threaded implementation of a
 * video motion detection algorithm.
 */
class ThreadedVMD : public VMD {
public:
	/*
	 *	Constructs the threaded VMD with the specified number of workers.	
	 */
	ThreadedVMD(int nw = 1) : m_nw(nw) {}

	/*
	 * Default destructor.
	 */
	~ThreadedVMD() {}

	/*
	 * Runs the video motion detection algorithm on the video
	 * located at the specified path.
	 */
    void run(std::string videoPath);

	/*
	 * Runs the video motion detection algorithm on the video
	 * located at the specified path a number of times equal to
	 * 'tries'. Time measurements are outputted to outfilepath.
	 */
    void benchmarkRun(std::string videoPath, int tries, std::string outFilePath);
	
private:
	int m_nw;	// Number of workers in the thread pool

	/*
	 *	Completely processes `frame`.	
	 */
	bool processFrame(VMDFrame frame, const VMDFrame& backround);
};

#endif
