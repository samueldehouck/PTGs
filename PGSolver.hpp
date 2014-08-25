#ifndef PGSOLVER_HPP
#define PGSOLVER_HPP

#include <vector>
#include <list>

#include "PG.hpp"
#include "Value.hpp"
#include "CompositeValue.hpp"
#include "Strategy.hpp"
#include "constants.hpp"

class PGSolver{
private:
	PG* pg;
	unsigned int size, nbTransitions;
	vector<bool> ensStates;
	vector<vector<bool> > ensTransitions;
	vector<bool> ensBottoms;
	vector<Value>* pathsLengths;
	vector<CompositeValue>* vals;
	list<Strategy>* strategies;
	vector<Value>* bottoms; //Equivalent of the "bottom" transitions in the paper
	vector<vector<Value> >* resets;
	vector<vector<bool> > ensResets;

	bool remainsStates();
	bool isLastTransition(unsigned int, unsigned int, bool, bool, bool);
public:
	PGSolver(PG* , vector<Value>*, vector<CompositeValue>*, list<Strategy>* , vector<vector<Value> >*);
	PGSolver(PG* , vector<Value>*, vector<CompositeValue>*, list<Strategy>* , vector<Value>*, vector<vector<Value> >*);

	void extendedDijkstra(bool);
};

#endif
