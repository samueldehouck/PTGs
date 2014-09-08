#ifndef PERFEVALUATOR_HPP_
#define PERFEVALUATOR_HPP_

#include "PTG.hpp"
#include "PTGFactory.hpp"
class PerfEvaluator{
private:
	unsigned int nbTests;
	void evalStatesTrans();
	void evalResets();
	void evalStates();
	void evalTrans();
	void evalInterval();
public:
	PerfEvaluator();
	PerfEvaluator(unsigned int);
	void eval();

};



#endif
