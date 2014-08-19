#ifndef SPTGSOLVER_HPP
#define SPTGSOLVER_HPP

#include "SPTG.hpp"
#include "Point.hpp"
#include "Fraction.hpp"
#include "Strategy.hpp"
#include <stddef.h>
#include <list>
#include "PG.hpp"
#include "PGSolver.hpp"

class SPTGSolver{
private:
	SPTG* sptg;
	unsigned int size;
	vector<vector<Fraction> > vals;
	vector<unsigned int> pathsLengths;
	vector<vector<Fraction> > lambdas;
	vector<list<Point> > valueFcts;//We need a list of points for every state
	list<Strategy> strategies;
	Fraction time;

	void init();
	bool extendedDijkstra();
	bool remainsStates();
	bool isLastTransition(unsigned int, unsigned int);
	void strategyIteration();
	void actualizeLambdas();
	void actualizeVals(Fraction);
	bool makeImpSwitchesP1();
	bool makeImpSwitchesP2();
	void propagate(unsigned int);
	void addPoint(unsigned int, Fraction, Fraction);
	void buildValueFcts(Fraction);
	Fraction nextEventPoint();
	list<Strategy> getStrategies();

public:
	SPTGSolver();
	~SPTGSolver();
	void show();
	void solveSPTG(SPTG*);

};

#endif
