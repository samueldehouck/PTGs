#ifndef PTGSOLVER_HPP
#define PTGSOLVER_HPP

#include "SPTGSolver.hpp"
#include "PGSolver.hpp"
#include "PTG.hpp"
#include "Fraction.hpp"
#include "Strategy.hpp"
#include "Point.hpp"
#include "Transition.hpp"

class PTGSolver{
private:
	PTG* ptg;
	unsigned int time;
	unsigned int size;
	list<unsigned int> endPoints;
	vector<unsigned int> pathsLengths;
	vector<vector<Fraction> > vals;
	list<Strategy> strategies;
	vector<list<Point> > valueFcts;
	list<Transition> storage;
	vector<vector<Fraction> > lambdas;

	void init();
	void createEndPoints();
	void keepTransAvailable(unsigned int, unsigned int);
public:
	PTGSolver();
	void solvePTG(PTG*);
	void show();
};

#endif
