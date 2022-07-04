#ifndef VMD_FRAME_H
#define VMD_FRAME_H

#include <opencv2/opencv.hpp>

class VMDFrame {
public:
	VMDFrame() {}
	VMDFrame(cv::Mat contents) : m_contents(contents) {}

    void toGrayScale();
    void smooth();
    bool compareTo(const VMDFrame& frame) const;

	bool isEmpty() const {
		return m_contents.empty();
	}

	void setContents(cv::Mat contents) {
		m_contents = contents;
	}

	const cv::Mat& getContents()  const {
		return m_contents;
	}

	const int getCols() const {
		return m_contents.cols;
	}

	const int getRows() const {
		return m_contents.rows;
	}

private:
	cv::Mat m_contents;
};

#endif
