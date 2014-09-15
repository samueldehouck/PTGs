#ifndef PERFEVALUATOR_HPP_
#define PERFEVALUATOR_HPP_

#include "PTG.hpp"
#include "PTGFactory.hpp"
class PerfEvaluator{
private:
	unsigned int nbTests;
	void evalStatesTrans(bool);
	void evalResets(bool);
	void evalStates(bool);
	void evalTrans(bool);
	void evalInterval(bool);
	void evalBig(bool);
public:
	PerfEvaluator();
	PerfEvaluator(unsigned int);
	void eval(bool);

};



#endif
