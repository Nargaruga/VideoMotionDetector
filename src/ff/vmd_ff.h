#ifndef VMD_FF_H
#define VMD_FF_H

#include "../vmd.h"
#include "../vmd_frame.h"
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include <ff/node.hpp>
#include <future>
#include <opencv2/videoio.hpp>

/*
 * Class providing a FastFlow-based farm implementation of a
 * video motion detection algorithm.
 */
class FastFlowVMD : public VMD {
public:
    /*
     *	POD to wrap a frame and the background to compare it to.
     */
    struct ToCompare {
        VMDFrame frame;
        const VMDFrame& background;

        ToCompare(VMDFrame f, const VMDFrame& b) : frame(f), background(b) {};
    };

    /*
     *	Constructs the FF VMD with the desired number of workers.
     */
    FastFlowVMD(int nWorkers = 1) : m_nWorkers(nWorkers) {}

    /*
     * Run the video motion detection algorithm on the video
     * located at the specified path.
     */
    void run(std::string videoPath);

    /*
     * Run the video motion detection algorithm on the video
     * located at the specified path a number of times equal to
     * 'tries'. Time measurements are outputted to `outFilePath`.
     */
    void benchmarkRun(std::string videoPath, int tries, std::string outFilePath);

private:
    int m_nWorkers;
    std::vector<std::unique_ptr<ff::ff_node>> m_workers;	// Farm workers

    /*
     *	Emitter class responsible for reading frames and
     *	passing them to workers, as well as getting back
	 *	results from workers and counting movement frames.
     */
    class Emitter : public ff::ff_node_t<bool, VMDFrame> {
    public:
        Emitter(std::string videoPath) : cap(videoPath) {}

    private:
        cv::VideoCapture cap;		// Video stream source
        VMDFrame background;		// Background to compare frames against
        int movementFrames = 0;		// Number of frames in which movement was detected
        int totalFrames = 0;		// Total number of frame processed
        int resultsReceived = 0;	// Number of results received by the emitter

        /*
         *	Reads frames and sends them out to workers.
         */
        VMDFrame* svc(bool* movement) {
            if(movement == nullptr) {
				// First run, we distribute tasks among workers
                while(true) {
                    cv::Mat frameContents;
                    cap >> frameContents;
                    if(frameContents.empty())
                        break;

                    totalFrames++;

                    if(background.isEmpty()) {
                        // Before processing any other frame, the background must be prepared
                        background.setContents(frameContents);
                        background.toGrayScale();
                        background.blur();
                    } else {
                        VMDFrame frame;
                        frame.setContents(frameContents);
                        ff_send_out(new ToCompare(frame, background));
                    }
                }

                cap.release();
                return GO_ON;
            } else {
				// Second run, we process the workers' results
                bool done = false;
                resultsReceived++;

				// -1 due to not counting the background
                if(resultsReceived == totalFrames - 1)
                    done = true;

                if(*movement)
                    movementFrames++;

                delete movement;

				if(done)
					return EOS;
				else
                	return GO_ON;

            }
        }

        /*
         * Prints the results.
         */
        void svc_end() {
            std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;
        }
    };

    /*
     *	Worker class responsible for processing frames.
     */
    class Worker : public ff::ff_node_t<ToCompare, bool> {

        /*
         * 	Processes a frame and passes it to the collector.
         */
        bool* svc(ToCompare* framesPtr) {
            if(framesPtr == nullptr)
                return EOS;

            bool res = false;

            VMDFrame& frame = framesPtr->frame;
            const VMDFrame& background = framesPtr->background;

            frame.toGrayScale();
            frame.blur();
            res = frame.checkForMovement(background);

            delete framesPtr;

            return new bool(res);
        }
    };
};

#endif
