/*
 * Author: Samuel Dehouck
 */


#ifndef SPTGSOLVERVALIT_HPP_
#define SPTGSOLVERVALIT_HPP_

#include "SPTG.hpp"
#include "Point.hpp"
#include "Fraction.hpp"
#include "Value.hpp"
#include "Strategy.hpp"
#include <stddef.h>
#include <list>
#include "PG.hpp"
#include "PGSolver.hpp"
#include "FunctionsMinMax.hpp"

using namespace std;

class SPTGSolverValIt {
private:
	SPTG* sptg;
	bool solvePTG;
	unsigned int size;
	vector<Value>* pathsLengths;
	vector<CompositeValue> lambdas;
	vector<list<Point> >* valueFcts;//We need a list of points for every state
	vector<Value>* bottoms;
	vector<vector<CompositeValue> >* resets;
	vector<bool> complete;
	vector<list<Point>* > copyValsSrc;
	vector<list<Point>* > copyVals;
	bool outputFcts;
	bool compareCopy();
	void copyValueFcts();

public:
	SPTGSolverValIt();
	SPTGSolverValIt(SPTG* , vector<Value>* ,  vector<Value>* ,  vector<list<Point> >* , vector<vector<CompositeValue> >* , bool);
	virtual ~SPTGSolverValIt();
	void show();
	void solveSPTG();
};

#endif /* SPTGSOLVERVALIT_HPP_ */
