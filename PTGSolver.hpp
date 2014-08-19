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
	Fraction time;
	unsigned int size;
	list<Fraction> endPoints;
	vector<unsigned int> pathsLengths;
	vector<vector<Fraction> > vals;
	list<Strategy> strategies;
	vector<list<Point> > valueFcts;
	list<Transition> storage;
	vector<Fraction> bottoms;
	vector<vector<Fraction> > resets;

	void init();
	void createEndPoints();
	void keepTransAvailable(Fraction, Fraction);
	void restoreAllTrans();
	void updateBottoms();
	void createMax(const Fraction, const Fraction);
	void rescale(Fraction, Fraction);
	void deleteMax();
	void cleanValueFcts();
	void createResets();
	void updateResets();
public:
	PTGSolver();
	void solvePTG(PTG*);
	void show();
};

#endif
