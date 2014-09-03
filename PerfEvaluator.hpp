#ifndef PERFEVALUATOR_HPP_
#define PERFEVALUATOR_HPP_

#include "PTG.hpp"
#include "PTGFactory.hpp"
class PerfEvaluator{

public:
	void eval();
	void evalStatesTrans();
	void evalResets();
	void evalStates();
	void evalTrans();
	void evalInterval();
};



#endif
