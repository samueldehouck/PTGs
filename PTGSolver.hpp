#ifndef PTGSOLVER_HPP
#define PTGSOLVER_HPP

#include "SPTGSolver.hpp"
#include "PGSolver.hpp"
#include "PTG.hpp"
#include "Fraction.hpp"
#include "Strategy.hpp"
#include "Point.hpp"
#include "Transition.hpp"
#include "Value.hpp"

class PTGSolver{
private:
	PTG* ptg;
	Value time;
	unsigned int size;
	list<Value> endPoints;
	vector<Value> pathsLengths;
	vector<CompositeValue> vals;
	list<Strategy> strategies;
	vector<list<Point> > valueFcts;
	list<Transition> storage;
	vector<Value> bottoms;
	vector<vector<Value> > resets;

	void init();
	void createEndPoints();
	void keepTransAvailable(Value, Value);
	void restoreAllTrans();
	void updateBottoms();
	void rescale(Value, Value);
	void cleanValueFcts();
	void createResets();
	void updateResets();
public:
	PTGSolver();
	void solvePTG(PTG*);
	void show();
};

#endif
