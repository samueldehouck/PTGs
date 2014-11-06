#ifndef SPTGSOLVERVALIT_HPP_
#define SPTGSOLVERVALIT_HPP_

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

using namespace std;

class SPTGSolverValIt {
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
	vector<bool> updated;

public:
	SPTGSolverValIt();
	SPTGSolverValIt(SPTG* , vector<Value>* ,  vector<Value>* , vector<CompositeValue>* , vector<list<Point> >* , vector<vector<CompositeValue> >* );
	virtual ~SPTGSolverValIt();
	void show();

};

#endif /* SPTGSOLVERVALIT_HPP_ */
