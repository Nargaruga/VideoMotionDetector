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

class FastFlowVMD : public VMD {
public:
	struct ToCompare {
		VMDFrame frame;
		const VMDFrame& background;

		ToCompare(VMDFrame f, const VMDFrame& b) : frame(f), background(b) {};
	};
	

    FastFlowVMD(int nWorkers = 1)  {
        for(int i = 0; i < nWorkers; i++) {
            m_workers.push_back(ff::make_unique<Worker>());
        }
    }

    void run(std::string videoPath);

    class Emitter : public ff::ff_node_t<VMDFrame> {
    public:
        Emitter(std::string videoPath) : cap(videoPath) {}

    private:
        cv::VideoCapture cap;
        VMDFrame background;

        VMDFrame* svc(VMDFrame* dummy) {
            VMDFrame frame;

            while(true) {
				cv::Mat frameContents;
                cap >> frameContents;
                if(frameContents.empty())
                    break;

				frame.setContents(frameContents);

                if(background.isEmpty()) {
                    background = frame;
					background.toGrayScale();
					background.smooth();
                } else {
                    ff_send_out(new ToCompare(frame, background));
                }
            }

            cap.release();

            return EOS;
        }
    };


    class Worker : public ff::ff_node_t<ToCompare, bool> {
        bool* svc(ToCompare* framesPtr) {
            if(framesPtr == nullptr)
                return EOS;

            bool res = false;

			VMDFrame frame = framesPtr->frame;
			VMDFrame background = framesPtr->background;

			frame.toGrayScale();
			frame.smooth();
			res = frame.compareTo(background);

			free(framesPtr);

            return new bool(res);
        }
    };

    class Collector : public ff::ff_node_t<bool, void> {
        int movementFrames = 0;
        int totalFrames = 0;

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

        void svc_end() {
            std::cout << "Movement was detected in " << movementFrames << " out of " << totalFrames << " frames." << std::endl;
        }
    };

    std::vector<std::unique_ptr<ff::ff_node>> m_workers;
};

#endif
