#ifndef SPTGSOLVERV2_HPP_
#define SPTGSOLVERV2_HPP_

#include "CompositeValue.hpp"
#include "Value.hpp"
#include "Point.hpp"
#include "Strategy.hpp"
#include "SPTG.hpp"
#include <list>
#include <vector>

using namespace std;

class SPTGSolverV2{
private:
	SPTG* sptg;
	bool solvePTG;
	unsigned int size;
	vector<CompositeValue>* vals;
	vector<Value>* pathsLengths;
	vector<list<Point> >* valueFcts;//We need a list of points for every state
	list<Strategy>* strategies;
	vector<Value>* bottoms;
	vector<vector<CompositeValue> >* resets;
	vector<vector<bool> > availableTrans;
	vector<bool> treatedStates;

	void buildValueFcts();
	bool updateValueFct(unsigned int);
	bool getMaxFct(unsigned int, unsigned int, Value);
	bool getMinFct(unsigned int, unsigned int, Value);
	void cleanValueFct(list<Point>*);

public:
	SPTGSolverV2(SPTG*);
	SPTGSolverV2(SPTG*, vector<Value>*,  vector<Value>*, vector<CompositeValue>*, list<Strategy>*, vector<list<Point> >*, vector<vector<CompositeValue> >*);
	~SPTGSolverV2();
	void solveSPTG();
	void show();
};

#endif
