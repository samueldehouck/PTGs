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
	bool solvePTG;
	unsigned int size;
	vector<vector<Fraction> >* vals;
	vector<unsigned int>* pathsLengths;
	vector<vector<Fraction> > lambdas;
	vector<list<Point> >* valueFcts;//We need a list of points for every state
	list<Strategy>* strategies;
	vector<Fraction>* bottoms;
	vector<vector<Fraction> >* resets;
	Fraction time;

	void init();
	void strategyIteration();
	void actualizeLambdas();
	void actualizeVals(Fraction);
	bool makeImpSwitchesP1();
	bool makeImpSwitchesP2();
	void propagate(unsigned int);
	void addPoint(unsigned int, Fraction, Fraction);
	void buildValueFcts(Fraction);
	Fraction nextEventPoint();
	list<Strategy>* getStrategies();
	void manageCycle(unsigned int);

public:
	SPTGSolver(SPTG*);
	SPTGSolver(SPTG*, vector<Fraction>*,  vector<unsigned int>*, vector<vector<Fraction> >*, list<Strategy>*, vector<list<Point> >*, vector<vector<Fraction> >*);
	~SPTGSolver();
	void show();
	void solveSPTG();

};

#endif
