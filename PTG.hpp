/*
 * Author: Samuel Dehouck
 */


#ifndef PTG_HPP_
#define PTG_HPP_

#include "SPTG.hpp"
#include "Fraction.hpp"
#include <vector>
#include <string>


class PTG: public SPTG{
protected:
	vector<vector<bool> >* resets;
	vector<vector<Fraction> >* startsCstraints;
	vector<vector<Fraction> >* endsCstraints;
	unsigned int nbResets;
	vector<string> labels;
public:
	PTG();
	PTG(unsigned int);
	PTG(unsigned int, bool);
	~PTG();
	Fraction getStartCst(unsigned int, unsigned int) const;
	void setStartCst(unsigned int, unsigned int, Fraction);
	Fraction getEndCst(unsigned int, unsigned int) const;
	void setEndCst(unsigned int, unsigned int, Fraction);
	bool getReset(unsigned int, unsigned int) const;
	void setReset(unsigned int, unsigned int, bool);
	unsigned int getNbResets() const;
	void setNbResets(unsigned int);
	string getLabel(unsigned int) const;
	void setLabel(unsigned int, string);
	bool hasLabels();
	void show();
	void write(string);

};

#endif
