#include "threaded_vmd.h"
#include <chrono>
#include <fstream>
#include <future>
#include <functional>
#include <opencv2/core/base.hpp>

ThreadedVMD::~ThreadedVMD() {
	m_pool.conclude();
}

//TODO ancora da modificare
void ThreadedVMD::run(std::string videoPath) {
    cv::VideoCapture cap(videoPath);
    cv::Mat background;
    cv::Mat frame;

    int movementFrames = 0;
    int totalFrames = 0;

    while(true) {
        cap >> frame;
        if(frame.empty())
            break;

        toGrayScale(frame);
        smooth(frame);

        if(background.empty()) {
            background = frame;
        } else if(compareFrame(background, frame)) {
            movementFrames++;
        }
        totalFrames++;
    }

    cap.release();

    std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;
}

void ThreadedVMD::benchmarkRun(std::string videoPath, int tries) {
    std::ofstream out;
    out.open("benchmark/benchmark.csv");

    out << "Total\n";

    for(int i = 0; i < tries; i++) {
        cv::VideoCapture cap(videoPath);
        cv::Mat background;
        cv::Mat frame;

		movementFrames = 0;
		totalFrames = 0;
		totalElapsed = 0;

        auto start = std::chrono::steady_clock::now();
        cap >> background;
        if(background.empty())
            break;
        toGrayScale(background);
        smooth(background);
    	totalFrames++;

        while(true) {
            cap >> frame;
            if(frame.empty())
                break;

            std::packaged_task<void()> task(std::bind(&ThreadedVMD::processFrame, this, frame.clone(), background));
            m_pool.insertTask(std::move(task));
			totalFrames++;
        }
        cap.release();

        m_pool.awaitCompletion();
        auto end = std::chrono::steady_clock::now();
        totalElapsed = std::chrono::duration_cast<std::chrono::microseconds> (end - start).count();


        std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;

        out << totalElapsed << "\n";
    }


    out.close();
}

void ThreadedVMD::processFrame(cv::Mat frame, const cv::Mat background) {
    toGrayScale(frame);
    smooth(frame);

    if(compareFrame(background, frame)) {
        movementFrames++;
    }
}
