#include <iostream>
#include <string>
#include "vmd.h"
#include "threads/threaded_vmd.h"
#include "ff/vmd_ff.h"

std::string usageString(
    "Usage: ./test VIDEO_PATH -m MODE -t TRIES -w WORKERS -b\n\
		At least VIDEO_PATH must be specified.");

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << usageString << std::endl;
        return -1;
    }

    if(argc > 9) {
        std::cout << usageString << std::endl;
        return -1;
    }

    std::string videoPath = argv[1];	// Path of the video to analyze
    int mode = 0;						//	Whether to run in sequential, threaded or FF mode
    int tries = 1;						// Number of benchmark tests
    int workers = 1;					// Parallelism degree
    bool benchmark = false; 			// Whether to measure execution times or not

    // Parse command line arguments
    for(int i = 2; i < argc; i++) {
        std::string arg(argv[i]);

        if(arg == "-m") {
            // Mode
            if(i == argc - 1) {
                std::cout << usageString << std::endl;
                return -1;
            } else {
                mode = atoi(argv[i+1]);
            }
            if(mode < 0 || mode > 2) {
                std::cout << "MODE must be one of 0 (sequential), 1 (threaded) or 2 (FastFlow)." << std::endl;
                return -1;
            }

        } else if(arg == "-t") {
            // Number of tries
            if(i == argc - 1) {
                std::cout << usageString << std::endl;
                return -1;
            } else {
                tries = atoi(argv[i+1]);
            }
            if(tries < 1) {
                std::cout << "TRIES must be at least 1." << std::endl;
                return -1;
            }

        } else if(arg == "-w") {
            // Parallelism degree
            if(i == argc - 1) {
                std::cout << usageString << std::endl;
                return -1;
            } else {
                workers = atoi(argv[i+1]);
            }
            if(workers < 1 || workers > 64) {
                std::cout << "WORKERS must be in [1, 64]." << std::endl;
                return -1;
            }
        } else if(arg == "-b") {
            // Enable time measurements
            benchmark = true;
        }
    }

//    for(int i = 1; i < 9; i++) {
//        workers = i; //TODO temporary...
        std::string modeStr;
        std::unique_ptr<VMD> detector;
        switch(mode) {
        case 0:
            modeStr = "seq";
            detector = std::unique_ptr<VMD>(new VMD());
            break;
        case 1:
            modeStr = "thread";
            detector = std::unique_ptr<ThreadedVMD>(new ThreadedVMD(workers));
            break;
        case 2:
            modeStr = "ff";
            detector = std::unique_ptr<FastFlowVMD>(new FastFlowVMD(workers));
            break;

        default:
            return -1;
        }


        if(benchmark) {
            std::string outFilePath("benchmark/"
                                    + modeStr
                                    + "/benchmark_"
                                    + modeStr + "_"
                                    + (workers < 10 ? "0" + std::to_string(workers) : std::to_string(workers))
                                    + ".csv");
            detector->benchmarkRun(videoPath, tries, outFilePath);
        }
        else {
            detector->run(videoPath);
        }
//    }

    return 0;
}
