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
	 *	The threadpool is initialized with `nw` workers.
	 */
	ThreadedVMD(int nw = 1) : m_nw(nw), m_pool(nw) {};

	/*
	 * Default destructor.
	 */
	~ThreadedVMD();

	/*
	 * Run the video motion detection algorithm on the video
	 * located at the specified path.
	 */
    void run(std::string videoPath);

	/*
	 * Run the video motion detection algorithm on the video
	 * located at the specified path a number of times equal to
	 * 'tries'. Time measurements are outputted to /benchmark/benchmark.cvs.
	 */
    void benchmarkRun(std::string videoPath, int tries);
	
private:
	int m_nw;							// Number of workers in the thread pool
	ThreadPool<void> m_pool;			// Thread pool for handling tasks
	std::atomic_int movementFrames = 0; // Number of frames in which movement was detected
	int totalFrames = 0;				// Total number of frames
	int totalElapsed = 0; 				// Total elapsed time

	/*
	 *	Perform a complete processing of the frame `frame`.
	 */
	void processFrame(VMDFrame& frame, const VMDFrame& backround);
};

#endif
