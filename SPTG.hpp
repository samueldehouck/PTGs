#ifndef SPTG_HPP
#define SPTG_HPP

#include <vector>

#include "PG.hpp"
#include "Value.hpp"

using namespace std;

class SPTG: public PG{
protected:
	vector<Value>* states;
public:
	SPTG();
	~SPTG();
	void show();
	Value getState(unsigned int) const;
	void setState(unsigned int, Value);
};

#endif
