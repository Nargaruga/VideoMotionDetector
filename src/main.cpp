#include <iostream>
#include "vmd.h"
#include "threads/threaded_vmd.h"

int main(int argc, char* argv[]) {
    if(argc < 4 || argc > 5) {
        std::cout << "Usage: ./test VIDEO_PATH MODE TRIES WORKERS" << std::endl;
        std::cout << "Specifying the number of workers is optional (default value is 1)." << std::endl;
        return -1;
    }

    std::string videoPath = argv[1];
    int mode = atoi(argv[2]);
    int tries = atoi(argv[3]);
	int workers = 1;

    if(mode < 0 || mode > 2) {
        std::cout << "MODE must be one of 0 (sequential), 1 (threaded) or 2 (FastFlow)." << std::endl;
        return -1;
    }

    if(tries < 1) {
        std::cout << "TRIES must be at least 1." << std::endl;
        return -1;
    }

    if(argc == 5) {
		workers = atoi(argv[4]);
        if(workers < 1 || workers > 64) {
            std::cout << "WORKERS must be in [1, 64]." << std::endl;
            return -1;
        }
    }
	std::cout << workers << std::endl;
	std::unique_ptr<VMD> detector;
    switch(mode) {
    case 0:
		detector = std::unique_ptr<VMD>(new VMD());
        break;
    case 1:
        detector = std::unique_ptr<ThreadedVMD>(new ThreadedVMD(workers));
        break;
    case 2:
        break;

    default:
        return -1;
    }

    detector->benchmarkRun(videoPath, tries);

    return 0;
}

