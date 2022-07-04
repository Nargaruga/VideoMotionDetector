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
    VMDFrame background;
    VMDFrame frame;
    cv::Mat frameContents;

    int movementFrames = 0;
    int totalFrames = 0;

    while(true) {
        cap >> frameContents;
        if(frameContents.empty())
            break;

        frame.setContents(frameContents);
        frame.toGrayScale();
        frame.smooth();

        if(background.isEmpty()) {
            background = frame;
        } else {
            if(frame.compareTo(background))
                movementFrames++;

            totalFrames++;
        }
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
        VMDFrame background;
        VMDFrame frame;
        cv::Mat frameContents;

        movementFrames = 0;
        totalFrames = 0;
        totalElapsed = 0;

        auto start = std::chrono::steady_clock::now();
        while(true) {
            cap >> frameContents;
            if(frameContents.empty())
                break;

            frame.setContents(frameContents);

            if(background.isEmpty()) {
                background = frame;
                background.toGrayScale();
                background.smooth();
            } else {
                std::packaged_task<void()> task(std::bind(&ThreadedVMD::processFrame, this, frame, background));
                m_pool.insertTask(std::move(task));

                totalFrames++;
            }
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

void ThreadedVMD::processFrame(VMDFrame& frame, const VMDFrame& background) {
    frame.toGrayScale();
    frame.smooth();

    if(frame.compareTo(background)) {
        movementFrames++;
    }
}
