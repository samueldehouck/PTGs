#ifndef PG_HPP
#define PG_HPP

#include <vector>
#include "Fraction.hpp"

using namespace std;

class PG{
protected:
	vector<vector<Fraction> >* transitions;
	vector<bool>* owners;
public:
	PG();
	PG(vector<vector<Fraction> >*, vector<bool>*);
	~PG();
	unsigned int getSize() const;
	Fraction getTransition(unsigned int, unsigned int) const;
	void setTransition(unsigned int,unsigned int, Fraction);
	bool getOwner(unsigned int) const;
	void setOwner(unsigned int, bool);
};

#endif
