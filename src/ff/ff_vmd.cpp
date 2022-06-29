#include "ff_vmd.h"
#include <bits/chrono.h>
#include <chrono>
#include <fstream>
#include <opencv2/core/base.hpp>

//TODO ancora da modificare
void FastFlowVMD::run(std::string videoPath) {
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

void FastFlowVMD::benchmarkRun(std::string videoPath, int tries) {
    std::ofstream out;
    out.open("/benchmark/benchmark.csv");

    out << "Total\n";

    for(int i = 0; i < tries; i++) {
        cv::VideoCapture cap(videoPath);
        cv::Mat background;
        cv::Mat frame;

        int movementFrames = 0;
        int totalFrames = 0;
		int totalElapsed = 0;

        auto start = std::chrono::steady_clock::now();
        while(true) {
            cap >> frame;
            if(frame.empty())
                break;
        }
        auto end = std::chrono::steady_clock::now();
        totalElapsed = std::chrono::duration_cast<std::chrono::microseconds> (end - start).count();

        cap.release();

        std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;

        out << totalElapsed << "\n";
    }


    out.close();
}
