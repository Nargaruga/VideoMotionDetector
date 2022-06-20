#include <iostream>
#include <opencv2/opencv.hpp>

void toGrayScale(cv::Mat& img) {
	cv::Mat grey(img.rows, img.cols, CV_8UC1);
	
	for(int r = 0; r < img.rows; r++) {
		for(int c = 0; c < img.cols; c++) {
			cv::Vec3b intensity = img.at<cv::Vec3b>(r, c);
			uchar red = intensity.val[2];
			uchar green	= intensity.val[1];
			uchar blue = intensity.val[0];

			uchar value = std::floor((red + green + blue) / 3);
			grey.at<uchar>(r, c) = value;
		}
	}

	img = grey.clone();
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

        imshow("Prova", frame);
        cv::waitKey(25);
    }
    cap.release();
    return 0;
}
