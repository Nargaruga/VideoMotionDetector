#include "vmd_frame.h"

void VMDFrame::toGrayScale() {
    cv::Mat grey(m_contents.rows, m_contents.cols, CV_8UC1);

    for(int r = 0; r < m_contents.rows; r++) {
        for(int c = 0; c < m_contents.cols; c++) {
            cv::Vec3b intensity = m_contents.at<cv::Vec3b>(r, c);
            uchar red = intensity.val[2];
            uchar green	= intensity.val[1];
            uchar blue = intensity.val[0];

            uchar value = std::floor((red + green + blue) / 3.0);
            grey.at<uchar>(r, c) = value;
        }
    }

    m_contents = grey.clone();
}

void VMDFrame::smooth() {
    int border = 1;
    cv::Mat padded;
    cv::Mat smoothed(m_contents.rows, m_contents.cols, CV_8UC1);

    // Pad original image to simplify border handling
    cv::copyMakeBorder(m_contents, padded, border, border, border, border, cv::BORDER_REPLICATE);

    // Vertical pass
    for(int r = 1; r < padded.rows - 2; r++) {
        for(int c = 1; c < padded.cols - 2; c++) {
            int count = 0;
            for(int i = - 1; i < 2; i++) {
                count += padded.at<uchar>(r + i, c);
            }
            smoothed.at<uchar>(r, c) = std::floor(count / 3.0);
        }
    }

    // Horizontal pass
    for(int r = 1; r < padded.rows - 2; r++) {
        for(int c = 1; c < padded.cols - 2; c++) {
            int count = 0;
            for(int i = - 1; i < 2; i++) {
                count += padded.at<uchar>(r, c + i);
            }
            smoothed.at<uchar>(r, c) = std::floor(count / 3.0);
        }
    }

    smoothed.copyTo(m_contents);
}

bool VMDFrame::compareTo(const VMDFrame& other) const {
    //TODO throw an error
    if(m_contents.rows != other.getRows() || m_contents.cols != other.getCols())
        return false;

    int count = 0;
    int pixelThreshold = std::round(0.1 * m_contents.rows * m_contents.cols);

    for(int r = 0; r < m_contents.rows; r++) {
        for(int c = 0; c < m_contents.cols; c++) {
            uchar a = m_contents.at<uchar>(r, c);
            uchar b = other.getContents().at<uchar>(r, c);

            if(abs(a - b) >= 5)
                count++;

            if(count > pixelThreshold)
                return true;
        }
    }

    return false;
}
