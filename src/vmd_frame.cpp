#include "vmd_frame.h"
#include "opencv2/core/matx.hpp"

void VMDFrame::toGrayScale() {
    cv::Mat grey(m_contents.rows, m_contents.cols, CV_8UC1);

    for(int r = 0; r < m_contents.rows; r++) {
		const cv::Vec3b* rowA = m_contents.ptr<cv::Vec3b>(r);
		uchar* rowB = grey.ptr<uchar>(r);

        for(int c = 0; c < m_contents.cols; c++) {
            cv::Vec3b intensity = rowA[c];

            uchar red = intensity.val[2];
            uchar green	= intensity.val[1];
            uchar blue = intensity.val[0];

            // Compte the pixel's intensity
            uchar value = std::floor((red + green + blue) / 3.0);
            rowB[c] = value;
        }
    }

    m_contents = grey;
}

void VMDFrame::blur() {
    int border = 3;
    cv::Mat padded;
    cv::Mat blurred(m_contents.rows, m_contents.cols, CV_8UC1);
	
    // Pad original image to simplify border handling
    cv::copyMakeBorder(m_contents, padded, border, border, border, border, cv::BORDER_REPLICATE);

	// Convolution with 7x7 kernel of 1s
    for(int r = 0; r < blurred.rows; r++) {
        for(int c = 0; c < blurred.cols; c++) {

            int count = 0;
            for(int x = -border; x <= border; x++) {
                for(int y = -border; y <= border; y++) {
                    count += padded.at<uchar>(r+x, c+y);
                }
            }

            blurred.at<uchar>(r, c) = std::floor(count / 49.0);
        }
    }

    m_contents = blurred;
}

bool VMDFrame::checkForMovement(const VMDFrame& other) const {
    if(m_contents.rows != other.getRows() || m_contents.cols != other.getCols())
        return false;

    int count = 0;
    // We detect movement if more than 10% of the pixels differ
    int pixelThreshold = std::round(0.1 * m_contents.rows * m_contents.cols);

    for(int r = 0; r < m_contents.rows; r++) {
        for(int c = 0; c < m_contents.cols; c++) {
            uchar a = m_contents.at<uchar>(r, c);
            uchar b = other.getContents().at<uchar>(r, c);

            // We consider two pixels to be different if their value differs by at least 5 points
            if(abs(a - b) >= 5)
                count++;

            if(count > pixelThreshold)
                return true;
        }
    }

    return false;
}
