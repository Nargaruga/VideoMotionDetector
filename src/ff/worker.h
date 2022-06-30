#ifndef WORKER_H
#define WORKER_H

//CULPRIT
//   |
//   v
#include <ff/ff.hpp>
#include <ff/node.hpp>

// Don't mind this, it's still a WIP
template<typename TIN, typename TOUT, typename H>
class Worker : public ff::ff_node {
	public:
		TOUT* svc(TIN* task) {
			if(task == nullptr)
				return ff::FF_GO_OUT;

			handler.handle(task);
		}

	private:
		H handler;
};

#endif 
