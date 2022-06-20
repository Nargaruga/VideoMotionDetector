#include <iostream>
#include <opencv2/opencv.hpp>

// Converts the provided RGB image to grayscale
void toGrayScale(cv::Mat& img) {
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

// Smooths the provided grayscale image
void smooth(cv::Mat& img) {
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

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: ./test VIDEO_PATH" << std::endl;
        return 0;
    }

    cv::VideoCapture cap(argv[1]);
    cv::Mat background;

    while(true) {
        cv::Mat frame;
        cap >> frame;
        if(frame.empty())
            break;

        toGrayScale(frame);
        smooth(frame);

        imshow("Test", frame);
        cv::waitKey(25);
    }
    cap.release();
    return 0;
}

