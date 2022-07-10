#include "vmd_ff.h"

void FastFlowVMD::run(std::string videoPath) {
    Emitter emitter(videoPath);
    Collector collector;

    for(int i = 0; i < m_nWorkers; i++) {
        m_workers.push_back(ff::make_unique<Worker>());
    }

    ff::ff_Farm<bool> farm(std::move(m_workers), emitter, collector);
    farm.run_and_wait_end();
}

void FastFlowVMD::benchmarkRun(std::string videoPath, int tries, std::string outFilePath) {
    // Prepare the benchmark output file
    std::ofstream out(outFilePath);
    if(!out.is_open()) {
        std::cerr << "Failed to open benchmark file. Closing." << std::endl;
        return;
    }
    out << "Setup;Total\n";

    for(int i = 0; i < tries; i++) {
        int setupElapsed = 0;
        int totalElapsed = 0;

        for(int i = 0; i < m_nWorkers; i++) {
            m_workers.push_back(ff::make_unique<Worker>());
        }

        // Setup the farm
        auto setupStart = std::chrono::steady_clock::now();
        Emitter emitter(videoPath);
        Collector collector;
        ff::ff_Farm<bool> farm(std::move(m_workers), emitter, collector);
        auto setupEnd = std::chrono::steady_clock::now();
        setupElapsed = std::chrono::duration_cast<std::chrono::microseconds> (setupEnd - setupStart).count();

        auto totalStart = std::chrono::steady_clock::now();
        farm.run_and_wait_end();
        auto totalEnd = std::chrono::steady_clock::now();
        totalElapsed = std::chrono::duration_cast<std::chrono::microseconds> (totalEnd - totalStart).count();

        out << setupElapsed << ";"
            << totalElapsed << "\n";
    }

    out.close();
}
