#ifndef PTG_HPP
#define PTG_HPP

#include "SPTG.hpp"
#include <vector>
#include "Fraction.hpp"


class PTG: public SPTG{
protected:
	vector<vector<bool> >* resets;
	vector<vector<Fraction> >* startsCstraints;
	vector<vector<Fraction> >* endsCstraints;
	unsigned int nbResets;

public:
	PTG();
	~PTG();
	Fraction getStartCst(unsigned int, unsigned int) const;
	Fraction getEndCst(unsigned int, unsigned int) const;
	bool getReset(unsigned int, unsigned int) const;
	unsigned int getNbResets() const;
	void createMaxState(Fraction, Fraction);
	void deleteMaxState();
	void show();
};

#endif
