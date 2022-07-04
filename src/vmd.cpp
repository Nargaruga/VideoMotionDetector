#include "vmd.h"
#include "opencv2/core/matx.hpp"
#include "vmd_frame.h"
#include <chrono>
#include <fstream>
#include <opencv2/core/base.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/videoio.hpp>

void VMD::run(std::string videoPath) {
    cv::VideoCapture cap(videoPath);
    VMDFrame background;
    VMDFrame frame;

    int movementFrames = 0;
    int totalFrames = 0;

	// Read video frames until EOF
    while(true) {
        cv::Mat frameContents;
        cap >> frameContents;
        if(frameContents.empty())
            break;

        frame.setContents(frameContents);
        frame.toGrayScale();
        frame.blur();

        if(background.isEmpty()) {
            background = frame;
        } else {
            if(frame.checkForMovement(background))
                movementFrames++;

            totalFrames++;
        }
    }

    cap.release();

    std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;
}

void VMD::benchmarkRun(std::string videoPath, int tries) {
	// Prepare the benchmark output file
    std::ofstream out;
    out.open("benchmark/benchmark.csv");
    out << "Gray;Smooth;Check;Total\n";

    for(int i = 0; i < tries; i++) {
        cv::VideoCapture cap(videoPath);
        VMDFrame background;
        VMDFrame frame;
        cv::Mat frameContents;

        int movementFrames = 0;
        int totalFrames = 0;

		// Time measurements for the various phases
        int grayElapsed = 0;
        int blurElapsed = 0;
        int compareElapsed = 0;
        int totalElapsed = 0;

        auto start = std::chrono::steady_clock::now();
		// Read video frames until EOF
        while(true) {
            cap >> frameContents;
            if(frameContents.empty())
                break;

            frame.setContents(frameContents);

            auto grayStart =  std::chrono::steady_clock::now();
            frame.toGrayScale();
            auto grayEnd = std::chrono::steady_clock::now();
            grayElapsed += std::chrono::duration_cast<std::chrono::microseconds> (grayEnd - grayStart).count();

            auto smoothStart =  std::chrono::steady_clock::now();
            frame.blur();
            auto smoothEnd = std::chrono::steady_clock::now();
            blurElapsed += std::chrono::duration_cast<std::chrono::microseconds> (smoothEnd - smoothStart).count();

            if(background.isEmpty()) {
                background = frame;
            } else {
                auto compareStart =  std::chrono::steady_clock::now();
                if(frame.checkForMovement(background)) {
                    movementFrames++;
                }
                totalFrames++;
                auto compareEnd = std::chrono::steady_clock::now();
                compareElapsed += std::chrono::duration_cast<std::chrono::microseconds> (compareEnd - compareStart).count();
            }

        }
        auto end = std::chrono::steady_clock::now();
        totalElapsed = std::chrono::duration_cast<std::chrono::microseconds> (end - start).count();

        cap.release();

        std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;

        out << round(grayElapsed / totalFrames) << ";"
            << round(blurElapsed / totalFrames) << ";"
            << round(compareElapsed / totalFrames) << ";"
            << totalElapsed << "\n";
    }


    out.close();
}

