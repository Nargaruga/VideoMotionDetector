#include "vmd_ff.h"

void FastFlowVMD::run(std::string videoPath) {
	Emitter emitter(videoPath);
	Collector collector;
	
	ff::ff_Farm<bool> farm(std::move(m_workers), emitter, collector);
	farm.run_and_wait_end();
}

