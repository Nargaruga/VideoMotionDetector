#include <iostream>
#include "sequential_motion_detector.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: ./test VIDEO_PATH" << std::endl;
        return 0;
    }

	SequentialMotionDetector detector;
	detector.benchmarkRun(argv[1], 10);

    return 0;
}

