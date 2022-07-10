#include "threaded_vmd.h"
#include <chrono>
#include <fstream>
#include <future>
#include <functional>
#include <iostream>
#include <opencv2/core/base.hpp>

void ThreadedVMD::run(std::string videoPath) {
	ThreadPool<void> pool(m_nw);	
    cv::VideoCapture cap(videoPath);
    VMDFrame background;
    VMDFrame frame;
    cv::Mat frameContents;

	int totalFrames = 0;

    // Read video frames until EOF
    while(true) {
        cv::Mat frameContents;
        cap >> frameContents;
        if(frameContents.empty())
            break;

        if(background.isEmpty()) {
            // The background must be processed first
            background.setContents(frameContents);
            background.toGrayScale();
            background.blur();
        } else {
            //Send out tasks to the pool
        	frame.setContents(frameContents);
            std::packaged_task<void()> task(std::bind(
                                                &ThreadedVMD::processFrame,
                                                this,
                                                frame,
                                                background));
            pool.insertTask(std::move(task));

            totalFrames++;
        }
    }
    cap.release();

    pool.awaitCompletion();

    std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;

}

void ThreadedVMD::benchmarkRun(std::string videoPath, int tries, std::string outFilePath) {
    // Prepare the benchmark output file
    std::ofstream out(outFilePath);
	if(!out.is_open()) {
		std::cerr << "Failed to open the benchmark file. Closing." << std::endl;
		return;
	}
    out << "Pool init;Total\n";

    for(int i = 0; i < tries; i++) {
        cv::VideoCapture cap(videoPath);
        VMDFrame background;
        VMDFrame frame;

        int totalFrames = 0;
		int setupElapsed = 0;
        int totalElapsed = 0;

        movementFrames = 0;

		// Initialize the threadpool while measuring the time necessary to do so 
        auto poolInitStart = std::chrono::steady_clock::now();
		ThreadPool<void> pool(m_nw);
        auto poolInitEnd = std::chrono::steady_clock::now();
        setupElapsed = std::chrono::duration_cast<std::chrono::microseconds> (poolInitEnd - poolInitStart).count();

        auto totalStart = std::chrono::steady_clock::now();
        // Read video frames until EOF
        while(true) {
            cv::Mat frameContents;
            cap >> frameContents;
            if(frameContents.empty())
                break;

            if(background.isEmpty()) {
                // The background must be processed first
                background.setContents(frameContents);
                background.toGrayScale();
                background.blur();
            } else {
                //Send out tasks to the pool
            	frame.setContents(frameContents);
                std::packaged_task<void()> task(std::bind(
                                                    &ThreadedVMD::processFrame,
                                                    this,
                                                    frame,
                                                    background));
                pool.insertTask(std::move(task));

                totalFrames++;
            }
        }
        cap.release();

        pool.awaitCompletion();
        auto totalEnd = std::chrono::steady_clock::now();
        totalElapsed = std::chrono::duration_cast<std::chrono::microseconds> (totalEnd - totalStart).count();

        std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;

		// Write benchmark results to a csv file
        out << setupElapsed << ";" 
			<< totalElapsed  << "\n";
    }


    out.close();
}

void ThreadedVMD::processFrame(VMDFrame& frame, const VMDFrame& background) {
    frame.toGrayScale();
    frame.blur();

    if(frame.checkForMovement(background)) {
        movementFrames++;
    }
}
