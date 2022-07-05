#ifndef VMD_FF_H
#define VMD_FF_H

#include "../vmd.h"
#include "../vmd_frame.h"
#include <ff/ff.hpp>
#include <ff/farm.hpp>
#include <ff/node.hpp>
#include <future>
#include <memory>
#include <opencv2/videoio.hpp>

/*
 * Class providing a FastFlow-based farm implementation of a
 * video motion detection algorithm.
 */
class FastFlowVMD : public VMD {
public:
    /*
     *	POD to pass frames to workers.
     */
    struct ToCompare {
        VMDFrame frame;
        const VMDFrame& background;

        ToCompare(VMDFrame f, const VMDFrame& b) : frame(f), background(b) {};
    };

    /*
     *	Constructs the FF VMD with the desired number of workers.
     */
    FastFlowVMD(int nWorkers = 1)  {
        for(int i = 0; i < nWorkers; i++) {
            m_workers.push_back(ff::make_unique<Worker>());
        }
    }

    /*
     * Run the video motion detection algorithm on the video
     * located at the specified path.
     */
    void run(std::string videoPath);

    /*
     * Run the video motion detection algorithm on the video
     * located at the specified path a number of times equal to
     * 'tries'. Time measurements are outputted to /benchmark/benchmark.cvs.
     */
    void benchmarkRun(std::string videoPath, int tries);

private:
	std::vector<std::unique_ptr<ff::ff_node>> m_workers;	// Farm workers

    /*
     *	Emitter class responsible for reading frames and
     *	passing them to workers.
     */
    class Emitter : public ff::ff_node_t<VMDFrame> {
    public:
        Emitter(std::string videoPath) : cap(videoPath) {}

    private:
        cv::VideoCapture cap;	// Video stream source
        VMDFrame background;	// Background to compare frames against

        /*
         *	Reads frames and sends them out to workers.
         */
        VMDFrame* svc(VMDFrame*) {
            VMDFrame frame;

            while(true) {
                cv::Mat frameContents;
                cap >> frameContents;
                if(frameContents.empty())
                    break;

                frame.setContents(frameContents);

                if(background.isEmpty()) {
                    // Before processing any other frame, the background must be prepared
                    background = frame;
                    background.toGrayScale();
                    background.blur();
                } else {
                    ff_send_out(new ToCompare(frame, background));
                }
            }

            cap.release();

            return EOS;
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

            VMDFrame frame = framesPtr->frame;
            VMDFrame background = framesPtr->background;

            frame.toGrayScale();
            frame.blur();
            res = frame.checkForMovement(background);

            free(framesPtr);

            return new bool(res);
        }
    };

    /*
     * Class responsible for collecting results and counting frames.
     */
    class Collector : public ff::ff_node_t<bool, void> {
        int movementFrames = 0;
        int totalFrames = 0;

        /*
         * Counts the number of frames where movement was detected.
         */
        void* svc(bool* movementDetectedPtr) {
            if(movementDetectedPtr == nullptr)
                return EOS;

            totalFrames++;
            if(*movementDetectedPtr) {
                movementFrames++;
            }

            free(movementDetectedPtr);

            return ff::FF_GO_ON;
        }

        /*
         * Prints the results.
         */
        void svc_end() {
            std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;
        }
    };

};

#endif
