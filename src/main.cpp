#include <iostream>
#include "seq/seq_vmd.h"
#include "threads/threaded_vmd.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cout << "Usage: ./test VIDEO_PATH" << std::endl;
        return 0;
    }

	ThreadedVMD detector(8);
//	SequentialVMD detector;
	detector.benchmarkRun(argv[1], 10);

    return 0;
}

