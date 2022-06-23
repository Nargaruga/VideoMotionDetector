#include "sequential_motion_detector.h"
#include <bits/chrono.h>
#include <chrono>
#include <fstream>
#include <opencv2/core/base.hpp>

void SequentialMotionDetector::run(std::string videoPath) {
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
        imshow("img", frame);
        cv::waitKey(25);
        totalFrames++;
    }

    cap.release();

    std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;
}

void SequentialMotionDetector::benchmarkRun(std::string videoPath, int tries) {
    std::ofstream out;
    out.open("benchmark.csv");

    out << "Gray;Smooth;Check;Total\n";

    for(int i = 0; i < tries; i++) {
        cv::VideoCapture cap(videoPath);
        cv::Mat background;
        cv::Mat frame;

        int movementFrames = 0;
        int totalFrames = 0;

        int grayElapsed = 0;
        int smoothElapsed = 0;
        int compareElapsed = 0;
        int totalElapsed = 0;

        auto start = std::chrono::steady_clock::now();
        while(true) {
            cap >> frame;
            if(frame.empty())
                break;

            auto grayStart =  std::chrono::steady_clock::now();
            toGrayScale(frame);
            auto grayEnd = std::chrono::steady_clock::now();
            grayElapsed += std::chrono::duration_cast<std::chrono::microseconds> (grayEnd - grayStart).count();

            auto smoothStart =  std::chrono::steady_clock::now();
            smooth(frame);
            auto smoothEnd = std::chrono::steady_clock::now();
            smoothElapsed += std::chrono::duration_cast<std::chrono::microseconds> (smoothEnd - smoothStart).count();

            if(background.empty()) {
                background = frame;
            } else {
                auto compareStart =  std::chrono::steady_clock::now();
                if(compareFrame(background, frame)) {
                    movementFrames++;
                }
                auto compareEnd = std::chrono::steady_clock::now();
                compareElapsed += std::chrono::duration_cast<std::chrono::microseconds> (compareEnd - compareStart).count();
            }

            totalFrames++;
        }
        auto end = std::chrono::steady_clock::now();
        totalElapsed = std::chrono::duration_cast<std::chrono::microseconds> (end - start).count();

        cap.release();

        std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;

        out << round(grayElapsed / totalFrames) << ";"
            << round(smoothElapsed / totalFrames) << ";"
            << round(compareElapsed / totalFrames) << ";"
            << totalElapsed << "\n";
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

    int border = 1;
    cv::Mat padded;
    cv::Mat kernel(3, 3, CV_8UC1, kernelData);
    cv::Mat smoothed(img.rows, img.cols, CV_8UC1);

    // Pad original image to simplify border handling
    cv::copyMakeBorder(img, padded, border, border, border, border, cv::BORDER_REPLICATE);

    for(int r = 1; r < padded.rows - 2; r++) {
        for(int c = 1; c < padded.cols - 2; c++) {
            cv::Mat section = padded(cv::Range(r-1, r+2), cv::Range(c-1, c+2));
            cv::Mat tmp = section.mul(kernel);

            int count = 0;
            tmp.forEach<uchar>([&count](uchar res, const int* pos) -> void {
                count += res;
            });

            smoothed.at<uchar>(r, c) = std::floor(count / 16.0);
        }
    }

	smoothed.copyTo(img);
}

bool SequentialMotionDetector::compareFrame(const cv::Mat& background, const cv::Mat& frame) {
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
