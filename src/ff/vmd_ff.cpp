#include "vmd_ff.h"

void FastFlowVMD::run(std::string videoPath) {
    Emitter emitter(videoPath);

    for(int i = 0; i < m_nWorkers; i++) {
        m_workers.push_back(ff::make_unique<Worker>());
    }

    ff::ff_Farm<bool> farm(std::move(m_workers));
	farm.add_emitter(emitter);
	farm.remove_collector();
	farm.wrap_around();
	farm.set_scheduling_ondemand();
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

        // Setup the farm
        auto setupStart = std::chrono::steady_clock::now();
        for(int i = 0; i < m_nWorkers; i++) {
            m_workers.push_back(ff::make_unique<Worker>());
        }

        Emitter emitter(videoPath);
        ff::ff_Farm<bool> farm(std::move(m_workers));
		farm.add_emitter(emitter);
		farm.remove_collector();
		farm.wrap_around();
		farm.set_scheduling_ondemand();
        auto setupEnd = std::chrono::steady_clock::now();
        setupElapsed = std::chrono::duration_cast<std::chrono::microseconds> (setupEnd - setupStart).count();

        auto totalStart = std::chrono::steady_clock::now();
        farm.run_and_wait_end();
        auto totalEnd = std::chrono::steady_clock::now();
        totalElapsed = std::chrono::duration_cast<std::chrono::microseconds> (totalEnd - totalStart).count();

        out << setupElapsed << ";"
            << totalElapsed << "\n";

		farm.ffStats(std::cout);
    }

    out.close();
}
