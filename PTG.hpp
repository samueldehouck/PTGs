#ifndef PTG_HPP
#define PTG_HPP

#include "SPTG.hpp"
#include <vector>
#include "Fraction.hpp"


class PTG: public SPTG{
protected:
	vector<vector<bool> >* resets;
	vector<vector<unsigned int> >* startsCstraints;
	vector<vector<unsigned int> >* endsCstraints;
	unsigned int nbResets;

public:
	PTG();
	unsigned int getStartCst(unsigned int, unsigned int) const;
	unsigned int getEndCst(unsigned int, unsigned int) const;
	unsigned int getReset(unsigned int, unsigned int) const;
	unsigned int getNbResets() const;
	void createMaxState(Fraction, unsigned int);
	void deleteMaxState();
	void show();
};

#endif
