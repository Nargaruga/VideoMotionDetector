#ifndef VMD_FRAME_H
#define VMD_FRAME_H

#include <opencv2/opencv.hpp>

/*
 *	Wrapper class for a CV frame that implements VMD-related behaviour.
 */
class VMDFrame {
public:
    /*
     *	Default constructor.
     */
    VMDFrame() {}

    /*
     *	Creates the frame and set its contents.
     */
    VMDFrame(cv::Mat contents) : m_contents(contents) {}

    /*
     *	Turns the frame to gray scale.
     */
    void toGrayScale();

    /*
     *	Blurs the frame.
     */
    void blur();

    /*
     * 	Returns true if movement is detected when comparing the two frames,
     * 	otherwise returns false.
     */
    bool checkForMovement(const VMDFrame& background) const;

    /*
     *	Returns true if the frame is empty, false otherwise.
     */
    bool isEmpty() const {
        return m_contents.empty();
    }

    /*
     *	Sets the frame's contents.
     */
    void setContents(cv::Mat contents) {
        m_contents = contents;
    }

    /*
     *	Returns the matrix with the frame's contents.
     */
    const cv::Mat& getContents()  const {
        return m_contents;
    }

    /*
     *	Returns the frame's width.
     */
    const int getCols() const {
        return m_contents.cols;
    }

    /*
     *	Returns the frame's heigth.
     */
    const int getRows() const {
        return m_contents.rows;
    }

private:
    cv::Mat m_contents; // Matrix containing the frame's contents.
};

#endif
