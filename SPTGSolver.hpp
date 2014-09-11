#ifndef SPTGSOLVER_HPP_
#define SPTGSOLVER_HPP_

#include "SPTG.hpp"
#include "Point.hpp"
#include "Fraction.hpp"
#include "Value.hpp"
#include "CompositeValue.hpp"
#include "Strategy.hpp"
#include <stddef.h>
#include <list>
#include "PG.hpp"
#include "PGSolver.hpp"

class SPTGSolver{
private:
	SPTG* sptg;
	bool solvePTG;
	unsigned int size;
	vector<CompositeValue>* vals;
	vector<Value>* pathsLengths;
	vector<CompositeValue> lambdas;
	vector<list<Point> >* valueFcts;//We need a list of points for every state
	vector<Value>* bottoms;
	vector<vector<CompositeValue> >* resets;
	Value time;

	void init();
	void strategyIteration();
	void actualizeLambdas();
	void actualizeVals(Value);
	bool makeImpSwitchesP1();
	bool makeImpSwitchesP2();
	void propagate(unsigned int);
	void addPoint(unsigned int, Value, Value);
	void buildValueFcts(Value);
	Value nextEventPoint();
	vector<list<Point> >* getValueFcts();

public:
	SPTGSolver(SPTG*);
	SPTGSolver(SPTG*, vector<Value>*,  vector<Value>*, vector<CompositeValue>*, vector<list<Point> >*, vector<vector<CompositeValue> >*);
	~SPTGSolver();
	void show();
	void solveSPTG();

};

#endif
