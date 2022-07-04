#include "vmd_ff.h"

void FastFlowVMD::run(std::string videoPath) {
    Emitter emitter(videoPath);
    Collector collector;

    ff::ff_Farm<bool> farm(std::move(m_workers), emitter, collector);
    farm.run_and_wait_end();
}

void FastFlowVMD::benchmarkRun(std::string videoPath, int tries) {
    std::ofstream out;
    for(int i = 0; i < tries; i++) {
        out.open("benchmark/benchmark.csv");

        out << "Total\n";
        int totalElapsed = 0;

        auto start = std::chrono::steady_clock::now();

        Emitter emitter(videoPath);
        Collector collector;
        ff::ff_Farm<bool> farm(std::move(m_workers), emitter, collector);
        farm.run_and_wait_end();
        auto end = std::chrono::steady_clock::now();

        totalElapsed = std::chrono::duration_cast<std::chrono::microseconds> (end - start).count();

        out << totalElapsed << "\n";
    }

    out.close();
}
