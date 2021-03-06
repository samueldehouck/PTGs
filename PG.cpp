/*
 * Author: Samuel Dehouck
 */

#include "PG.hpp"

#include <iostream>
#include <cstdlib> 

using namespace std;

PG::PG(){
	transitions = new vector<vector<Value> >();
	owners = new vector<bool>();
}

PG::PG(vector<vector<Value> >* t, vector<bool>* o){
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

Value PG::getTransition(unsigned int origin,unsigned int end) const {
	return (*transitions)[origin][end];
}

void PG::setTransition(unsigned int origin, unsigned int end, Value value){
	(*transitions)[origin][end] = value;
}

bool PG::getOwner(unsigned int index) const {
	return (*owners)[index];
}

void PG::setOwner(unsigned int index, bool o){
	(*owners)[index] = o;
}
