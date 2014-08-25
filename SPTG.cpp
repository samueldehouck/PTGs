#include "SPTG.hpp"
#include <iostream>

SPTG::SPTG(){
	states = new vector<Value>();
}

SPTG::~SPTG(){
	delete transitions;
	delete owners;
	delete states;
}

void SPTG::show(){
	cout << "Rates: " << endl;
	for (unsigned int i =0; i < states->size(); ++i)
		cout << (*states)[i] << " ";
	cout << endl;

	cout << "Transitions: "<< endl;
	for (unsigned int i = 0; i < transitions->size(); ++i){
		for (unsigned int j = 0; j < (*transitions)[i].size(); ++j)
			cout << (*transitions)[i][j] << "	";
		cout << endl;
	}

	cout << "Owners: " << endl;
	for (unsigned int i = 0; i < owners->size(); ++i)
		cout << (*owners)[i] << " ";
	cout << endl;
}

Value SPTG::getState(unsigned int index) const{
	return (*states)[index];
}

void SPTG::setState(unsigned int index, Value f){
	(*states)[index] = f;
}
