#include "PG.hpp"

#include <iostream>
#include <cstdlib> 

using namespace std;

PG::PG(){
	transitions = new vector<vector<Fraction> >();
	owners = new vector<bool>();
}

PG::PG(vector<vector<Fraction> >* t, vector<bool>* o){
	transitions = t;
	owners = o;
}

PG::~PG(){
	//Be careful, vectors aren't deleted because they weren't built by the PG but by the SPTG using them
	transitions = NULL;
	owners = NULL;
}

unsigned int PG::getSize() const{
	return owners->size();
}

Fraction PG::getTransition(unsigned int origin,unsigned int end) const {
	return (*transitions)[origin][end];
}

void PG::setTransition(unsigned int origin, unsigned int end, Fraction value){
	(*transitions)[origin][end] = value;
}

bool PG::getOwner(unsigned int index) const {
	return (*owners)[index];
}
