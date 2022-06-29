#ifndef FF_VMD_H
#define FF_VMD_H

#include "../vmd.h"

class FastFlowVMD : public VMD {
public:
	FastFlowVMD(int nw = 1) : m_nWorkers(nw) {}
	~FastFlowVMD() {}

    void run(std::string videoPath);
    void benchmarkRun(std::string videoPath, int tries);
	
private:
	int m_nWorkers;
};

#endif
