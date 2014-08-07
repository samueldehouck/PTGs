#ifndef SPTG_HPP
#define SPTG_HPP

#include <vector>

#include "PG.hpp"
#include "Fraction.hpp"

using namespace std;

class SPTG: public PG{
protected:
	vector<Fraction>* states;
public:
	SPTG();
	~SPTG();
	void show();
	Fraction getState(unsigned int) const;
	void setState(unsigned int, Fraction);
};

#endif
