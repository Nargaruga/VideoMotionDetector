#include "sequential_motion_detector.h"

void SequentialMotionDetector::run(std::string videoPath) {
    cv::VideoCapture cap(videoPath);
    cv::Mat background;

	int movementFrames = 0, totalFrames = 0;

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

void SequentialMotionDetector::benchmarkRun(std::string videoPath) {
    cv::VideoCapture cap(videoPath);
    cv::Mat background;
	
	int movementFrames = 0, totalFrames = 0;

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
