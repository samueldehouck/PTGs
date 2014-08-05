#ifndef PGSOLVER_HPP
#define PGSOLVER_HPP

#include <vector>
#include <list>

#include "PG.hpp"
#include "Fraction.hpp"
#include "Strategy.hpp"
#include "constants.hpp"

class PGSolver{
private:
	PG* pg;
	unsigned int size, nbTransitions;
	vector<bool> ensStates;
	vector<vector<bool> > ensTransitions;
	vector<bool> ensBottoms;
	vector<unsigned int>* pathsLengths;
	vector<vector<Fraction> >* vals;
	list<Strategy>* strategies;
	vector<Fraction>* bottoms; //Equivalent of the "bottom" transition in the paper

	bool remainsStates();
	bool isLastTransition(unsigned int, unsigned int, bool);
public:
	PGSolver(PG* , vector<unsigned int>*, vector<vector<Fraction> >*, list<Strategy>* );
	PGSolver(PG* , vector<unsigned int>*, vector<vector<Fraction> >*, list<Strategy>* , vector<Fraction>*);

	bool extendedDijkstra(bool);
	void addLambdaTrans();
};

#endif
