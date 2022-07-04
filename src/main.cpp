#include <iostream>
#include <string>
#include "vmd.h"
#include "threads/threaded_vmd.h"
#include "ff/vmd_ff.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: ./test VIDEO_PATH -m MODE -t TRIES -w WORKERS" << std::endl;
        std::cout << "At least VIDEO_PATH must be specified." << std::endl;
        return -1;
    }

    if(argc > 8) {
        std::cout << "Usage: ./test VIDEO_PATH -m MODE -t TRIES -w WORKERS" << std::endl;
        return -1;
    }



    std::string videoPath = argv[1];
    int mode = 0;
    int tries = 1;
    int workers = 1;

    for(int i = 2; i < argc; i++) {
        std::string arg(argv[i]);

        if(arg == "-m") {
            if(i == argc - 1) {
                std::cout << "Usage: ./test VIDEO_PATH -m MODE -t TRIES -w WORKERS" << std::endl;
                std::cout << "At least VIDEO_PATH must be specified." << std::endl;
                return -1;
            } else {
                mode = atoi(argv[i+1]);
            }
            if(mode < 0 || mode > 2) {
                std::cout << "MODE must be one of 0 (sequential), 1 (threaded) or 2 (FastFlow)." << std::endl;
                return -1;
            }

        } else if(arg == "-t") {
            if(i == argc - 1) {
                std::cout << "Usage: ./test VIDEO_PATH -m MODE -t TRIES -w WORKERS" << std::endl;
                std::cout << "At least VIDEO_PATH must be specified." << std::endl;
                return -1;
            } else {
                tries = atoi(argv[i+1]);
            }
            if(tries < 1) {
                std::cout << "TRIES must be at least 1." << std::endl;
                return -1;
            }

        } else if(arg == "-w") {
            if(i == argc - 1) {
                std::cout << "Usage: ./test VIDEO_PATH -m MODE -t TRIES -w WORKERS" << std::endl;
                std::cout << "At least VIDEO_PATH must be specified." << std::endl;
                return -1;
            } else {
                workers = atoi(argv[i+1]);
            }
            if(workers < 1 || workers > 64) {
                std::cout << "WORKERS must be in [1, 64]." << std::endl;
                return -1;
            }

        }
    }

    std::unique_ptr<VMD> detector;
    switch(mode) {
    case 0:
        detector = std::unique_ptr<VMD>(new VMD());
        break;
    case 1:
        detector = std::unique_ptr<ThreadedVMD>(new ThreadedVMD(workers));
        break;
    case 2:
        detector = std::unique_ptr<FastFlowVMD>(new FastFlowVMD(workers));
        break;

    default:
        return -1;
    }

    detector->run(videoPath);

    return 0;
}

