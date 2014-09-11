#ifndef SPTGSOLVERV2_HPP_
#define SPTGSOLVERV2_HPP_

#include "CompositeValue.hpp"
#include "Value.hpp"
#include "Point.hpp"
#include "SPTG.hpp"
#include <list>
#include <vector>
#include <queue>

using namespace std;

class SPTGSolverV2{
private:
	SPTG* sptg;
	bool solvePTG;
	unsigned int size;
	vector<CompositeValue>* vals;
	vector<Value>* pathsLengths;
	vector<list<Point> >* valueFcts;//We need a list of points for every state
	vector<Value>* bottoms;
	vector<vector<CompositeValue> >* resets;
	vector<vector<bool> > availableTrans;
	vector<bool> treatedStates;

	void buildValueFcts();
	bool updateValueFct(unsigned int);
	bool getMaxFct(unsigned int, unsigned int, Value);
	bool getMinFct(unsigned int, unsigned int, Value);
	void cleanValueFct(list<Point>*);
	void propagate(list<unsigned int> &, unsigned int);
	bool compareWaiting(unsigned int);

public:
	SPTGSolverV2(SPTG*);
	SPTGSolverV2(SPTG*, vector<Value>*,  vector<Value>*, vector<CompositeValue>*, vector<list<Point> >*, vector<vector<CompositeValue> >*);
	~SPTGSolverV2();
	void solveSPTG();
	void show();
};

#endif
