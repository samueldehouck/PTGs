/*
 * Author: Samuel Dehouck
 */


#ifndef PERFEVALUATOR_HPP_
#define PERFEVALUATOR_HPP_

#include "PTG.hpp"
#include "PTGFactory.hpp"
class PerfEvaluator{
private:
	unsigned int nbTests;
	void evalStatesTrans(unsigned int);
	void evalResets(unsigned int);
	void evalStates(unsigned int);
	void evalTrans(unsigned int);
	void evalInterval(unsigned int);
	void evalBig(unsigned int);
	void evalBreakPoints(unsigned int);
public:
	PerfEvaluator();
	PerfEvaluator(unsigned int);
	void eval(unsigned int);

};



#endif
