#include "sequential_motion_detector.h"
#include <bits/chrono.h>
#include <chrono>
#include <fstream>

void SequentialMotionDetector::run(std::string videoPath) {
    cv::VideoCapture cap(videoPath);
    cv::Mat background;

    int movementFrames = 0;
    int totalFrames = 0;

    while(true) {
        cv::Mat frame;
        cap >> frame;
        if(frame.empty())
            break;

        toGrayScale(frame);
        smooth(frame);

        if(background.empty()) {
            background = frame;
        } else if(checkFrame(background, frame)) {
            movementFrames++;
        }

        totalFrames++;
    }

    cap.release();

    std::cout << "Movement was detected in " << movementFrames << "out of " << totalFrames << " frames." << std::endl;
}

void SequentialMotionDetector::benchmarkRun(std::string videoPath, int tries) {
	std::ofstream out;
	out.open("benchmark.csv");

	out << "Gray;Smooth;Check;Total\n";

    for(int i = 0; i < tries; i++) {
        cv::VideoCapture cap(videoPath);
        cv::Mat background;

        int movementFrames = 0;
        int totalFrames = 0;

        int grayElapsed = 0;
        int smoothElapsed = 0;
        int checkElapsed = 0;

        while(true) {
            cv::Mat frame;
            cap >> frame;
            if(frame.empty())
                break;

            auto grayStart =  std::chrono::system_clock::now();
            toGrayScale(frame);
            auto grayEnd = std::chrono::system_clock::now();
            grayElapsed = std::chrono::duration_cast<std::chrono::milliseconds> (grayEnd - grayStart).count();

            auto smoothStart =  std::chrono::system_clock::now();
            smooth(frame);
            auto smoothEnd = std::chrono::system_clock::now();
            smoothElapsed = std::chrono::duration_cast<std::chrono::milliseconds> (smoothEnd - smoothStart).count();

            if(background.empty()) {
                background = frame;
            } else {
                auto checkStart =  std::chrono::system_clock::now();
                if(checkFrame(background, frame)) {
                    movementFrames++;
                }
                auto checkEnd = std::chrono::system_clock::now();
                checkElapsed = std::chrono::duration_cast<std::chrono::milliseconds> (checkEnd - checkStart).count();
            }

            totalFrames++;
        }

        cap.release();

        std::cout << "Movement was detected in " << movementFrames << "out of " << totalFrames << " frames." << std::endl;

	out << grayElapsed << ";"
		<< smoothElapsed << ";"
		<< checkElapsed << ";"
		<< (grayElapsed + smoothElapsed + checkElapsed) << "\n";
    }


	out.close();
}

void SequentialMotionDetector::toGrayScale(cv::Mat& img) {
    cv::Mat grey(img.rows, img.cols, CV_8UC1);

    for(int r = 0; r < img.rows; r++) {
        for(int c = 0; c < img.cols; c++) {
            cv::Vec3b intensity = img.at<cv::Vec3b>(r, c);
            uchar red = intensity.val[2];
            uchar green	= intensity.val[1];
            uchar blue = intensity.val[0];

            uchar value = std::floor((red + green + blue) / 3.0);
            grey.at<uchar>(r, c) = value;
        }
    }

    img = grey.clone();
}

void SequentialMotionDetector::smooth(cv::Mat& img) {
    // Gaussian blur kernel
    uchar kernelData[9] = {1, 2, 1,
                           2, 4, 2,
                           1, 2, 1,
                          };

    cv::Mat kernel(3, 3, CV_8UC1, kernelData);
    cv::Mat smoothed(img.rows, img.cols, CV_8UC1);

    for(int r = 1; r < img.rows - 1; r++) {
        for(int c = 1; c < img.cols - 1; c++) {
            cv::Mat section = img(cv::Range(r-1, r+2), cv::Range(c-1, c+2));
            cv::Mat tmp = section.mul(kernel);

            int count = 0;
            tmp.forEach<uchar>([&count](uchar res, const int* pos) -> void {
                count += res;
            });

            smoothed.at<uchar>(r, c) = std::floor(count / 16.0);
        }
    }

    img = smoothed.clone();
}

bool SequentialMotionDetector::checkFrame(cv::Mat& background, cv::Mat& frame) {
    //TODO throw an error
    if(background.rows != frame.rows || background.cols != frame.cols)
        return false;

    int count = 0;
    int pixelThreshold = std::round(threshold * background.rows * background.cols);

    for(int r = 0; r < background.rows; r++) {
        for(int c = 0; c < background.cols; c++) {
            uchar a = background.at<uchar>(r, c);
            uchar b = frame.at<uchar>(r, c);

            if(abs(a - b) >= 5)
                count++;

            if(count > pixelThreshold)
                return true;
        }
    }

    return false;
}
