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
	void timeBPs();
	void timeStates();
	void timeTrans();
	void timeResets();
	void timeConsts();
	void timeWeights();
	void timeRates();
	void timeRatesWeights();
	void recupBPs();
public:
	PerfEvaluator();
	PerfEvaluator(double);
	void eval();

};



#endif
