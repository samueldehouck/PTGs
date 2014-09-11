#ifndef PGSOLVER_HPP
#define PGSOLVER_HPP

#include <vector>
#include <list>

#include "PG.hpp"
#include "Value.hpp"
#include "CompositeValue.hpp"
#include "Point.hpp"

class PGSolver{
private:
	PG* pg;
	unsigned int size, nbTransitions;
	vector<bool> ensStates;
	vector<vector<bool> > ensTransitions;
	vector<bool> ensBottoms;
	vector<Value>* pathsLengths;
	vector<CompositeValue>* vals;
	vector<list<Point> >* valueFcts;
	vector<Value>* bottoms; //Equivalent of the "bottom" transitions in the paper
	vector<vector<CompositeValue> >* resets;
	vector<vector<bool> > ensResets;

	bool remainsStates();
	bool isLastTransition(unsigned int, unsigned int, bool, bool, bool);
public:
	PGSolver(PG* , vector<Value>*, vector<CompositeValue>*, vector<list<Point> >* , vector<vector<CompositeValue> >*);
	PGSolver(PG* , vector<Value>*, vector<CompositeValue>*, vector<list<Point> >* , vector<Value>*, vector<vector<CompositeValue> >*);

	void extendedDijkstra(bool);
};

#endif
