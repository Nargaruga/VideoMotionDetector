#include "vmd_frame.h"

void VMDFrame::toGrayScale() {
    cv::Mat grey(m_contents.rows, m_contents.cols, CV_8UC1);

    for(int r = 0; r < m_contents.rows; r++) {
        for(int c = 0; c < m_contents.cols; c++) {
            cv::Vec3b intensity = m_contents.at<cv::Vec3b>(r, c);
            uchar red = intensity.val[2];
            uchar green	= intensity.val[1];
            uchar blue = intensity.val[0];

			// Compte the pixel's intensity
            uchar value = std::floor((red + green + blue) / 3.0);
            grey.at<uchar>(r, c) = value;
        }
    }

	m_contents = grey;
}

void VMDFrame::blur() {
    int border = 1;
    cv::Mat padded;
    cv::Mat blurred(m_contents.rows, m_contents.cols, CV_8UC1);

    // Pad original image to simplify border handling
    cv::copyMakeBorder(m_contents, padded, border, border, border, border, cv::BORDER_REPLICATE);

	// Blurring is done by applying a 3x3 convolution matrix of 1s.
	// We Split the convolution in two passes in order to reduce the total number of
	// per-pixel operations from 9 to 6.

    // Vertical pass
    for(int r = 1; r < padded.rows - 2; r++) {
        for(int c = 1; c < padded.cols - 2; c++) {
            int count = 0;
            for(int i = - 1; i < 2; i++) {
                count += padded.at<uchar>(r + i, c);
            }
            blurred.at<uchar>(r, c) = std::floor(count / 3.0);
        }
    }

    // Horizontal pass
    for(int r = 1; r < padded.rows - 2; r++) {
        for(int c = 1; c < padded.cols - 2; c++) {
            int count = 0;
            for(int i = - 1; i < 2; i++) {
                count += padded.at<uchar>(r, c + i);
            }
            blurred.at<uchar>(r, c) = std::floor(count / 3.0);
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
