#ifndef PTGSOLVER_HPP_
#define PTGSOLVER_HPP_

#include "SPTGSolver.hpp"
#include "PGSolver.hpp"
#include "PTG.hpp"
#include "Fraction.hpp"
#include "Strategy.hpp"
#include "Point.hpp"
#include "Transition.hpp"
#include "Value.hpp"
#include "SPTGSolverV2.hpp"

class PTGSolver{
private:
	PTG* ptg;
	Value time;
	unsigned int size;
	list<Value> endPoints;
	vector<Value> pathsLengths;
	vector<CompositeValue> vals;
	vector<list<Strategy> > strategies;
	vector<list<Point> > valueFcts;
	list<Transition> storage;
	vector<Value> bottoms;
	vector<vector<CompositeValue> > resets;

	void init();
	void createEndPoints();
	void keepTransAvailable(Value, Value);
	void restoreAllTrans();
	void updateBottoms();
	void rescale(Value, Value);
	void cleanValueFcts();
	void correctStrats();
	void cleanStrats();
	void createResets();
	void updateResets();
	void visualize();

public:
	PTGSolver();
	void solvePTG(PTG*, bool);
	vector<list<Point> >* getValueFcts();
	void show();
};

#endif
