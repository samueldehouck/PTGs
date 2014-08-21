#include "PTG.hpp"
#include <iostream>

using namespace std;

PTG::PTG(){
	//   Init of a game
	resets = new vector<vector<bool> >();
	startsCstraints = new vector<vector<Fraction> >();
	endsCstraints = new vector<vector<Fraction> >();
	nbResets = 0;

}

PTG::PTG(unsigned int size){
	resets = new vector<vector<bool> >();
	startsCstraints = new vector<vector<Fraction> >();
    endsCstraints = new vector<vector<Fraction> >();
    nbResets = 0;

    for (unsigned int i = 0; i < size; ++i){
    	transitions->push_back(vector<Fraction>());
    	owners->push_back(0);
    	states->push_back(0);
    	resets->push_back(vector<bool>());
    	startsCstraints->push_back(vector<Fraction>());
    	endsCstraints->push_back(vector<Fraction>());
    	for(unsigned int j = 0; j < size; ++j){
    		(*transitions)[i].push_back(-1);
    		(*resets)[i].push_back(false);
    		(*startsCstraints)[i].push_back(-1);
    		(*endsCstraints)[i].push_back(-1);
    	}
    }
}

PTG::~PTG(){
	delete startsCstraints;
	delete endsCstraints;
	delete resets;
	//delete states;
	//delete transitions;
	//delete owners;
}

Fraction PTG::getStartCst(unsigned int origin, unsigned int dest) const{
	return (*startsCstraints)[origin][dest];
}

void PTG::setStartCst(unsigned int origin, unsigned int dest, Fraction val){
	(*startsCstraints)[origin][dest] = val;
}

Fraction PTG::getEndCst(unsigned int origin, unsigned int dest)const{
	return (*endsCstraints)[origin][dest];
}

void PTG::setEndCst(unsigned int origin, unsigned int dest, Fraction val){
	(*endsCstraints)[origin][dest] = val;
}

bool PTG::getReset(unsigned int origin, unsigned int dest) const{
	return (*resets)[origin][dest];
}

void PTG::setReset(unsigned int origin, unsigned int dest, bool val){
	(*resets)[origin][dest] = val;
}
unsigned int PTG::getNbResets() const{
	return nbResets;
}

void PTG::setNbResets(unsigned int nb){
	nbResets = nb;
}
void PTG::createMaxState(Fraction f, Fraction endCst){
	states->push_back(f);
	owners->push_back(false);
	transitions->push_back(vector<Fraction>());
	resets->push_back(vector<bool>());
	startsCstraints->push_back(vector<Fraction>());
	endsCstraints->push_back(vector<Fraction>());

	for (unsigned int i = 0; i < getSize() - 1; ++i){
		//Fills the new column of each rows
		(*transitions)[i].push_back(-1);
		(*resets)[i].push_back(false);
		(*startsCstraints)[i].push_back(0);
		(*endsCstraints)[i].push_back(endCst);

		//Fills the columns of the new row
		(*transitions)[getSize()-1].push_back(-1);
		(*resets)[getSize()-1].push_back(false);
		(*startsCstraints)[getSize()-1].push_back(0);
		(*endsCstraints)[getSize()-1].push_back(endCst);

	}
	(*transitions)[getSize()-1].push_back(-1);
	(*resets)[getSize()-1].push_back(false);
	(*startsCstraints)[getSize()-1].push_back(0);
	(*endsCstraints)[getSize()-1].push_back(endCst);

}

void PTG::deleteMaxState(){
	states->pop_back();
	owners->pop_back();
	transitions->pop_back();
	resets->pop_back();
	startsCstraints->pop_back();
	endsCstraints->pop_back();

	//Delete at all ends
	for (unsigned int i = 0; i < getSize(); ++i){
		(*transitions)[i].pop_back();
		(*resets)[i].pop_back();
		(*startsCstraints)[i].pop_back();
		(*endsCstraints)[i].pop_back();
	}
}

void PTG::show(){
	cout << "====PTG===" << endl;
	SPTG::show();

	cout << "Constraints: " << endl;
	for (unsigned int i = 0; i < startsCstraints->size(); ++i){
		for (unsigned int j = 0; j < (*startsCstraints)[i].size(); ++j){
			cout << "[" << (*startsCstraints)[i][j] << "," << (*endsCstraints)[i][j] << "]	";
		}
		cout << endl;
	}
	cout << "Resets: " << endl;
	for(unsigned int i = 0; i < resets->size(); ++i){
		for (unsigned int j = 0; j < (*resets)[i].size(); ++j)
			cout << (*resets)[i][j] << "	";
		cout << endl;
	}

	cout << endl;
}
