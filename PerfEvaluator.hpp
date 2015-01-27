/*
 * Author: Samuel Dehouck
 */


#ifndef PERFEVALUATOR_HPP_
#define PERFEVALUATOR_HPP_

#include "PTG.hpp"
#include "PTGFactory.hpp"
class PerfEvaluator{
private:
	double nbTests;
	void nbBreakPoints();
public:
	PerfEvaluator();
	PerfEvaluator(double);
	void eval();

};



#endif
