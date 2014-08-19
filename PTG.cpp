#include "PTG.hpp"
#include <iostream>

using namespace std;

PTG::PTG(){
	//   Init of a game
	resets = new vector<vector<bool> >();
	startsCstraints = new vector<vector<Fraction> >();
	endsCstraints = new vector<vector<Fraction> >();

	states->push_back(0);
	states->push_back(3);
	states->push_back(2);
	states->push_back(0);
	states->push_back(1);
	states->push_back(5);


	for (unsigned int i = 0; i < states->size(); ++i){
		transitions->push_back(vector<Fraction>());
		for (unsigned int j = 0; j < states->size(); ++j){
			(*transitions)[i].push_back(-1);
		}
	}
	(*transitions)[1][3] = 1;
	(*transitions)[2][1] = 2;
	(*transitions)[2][3] = 1;
	(*transitions)[2][4] = 2;
	(*transitions)[2][5] = 1;
	(*transitions)[3][0] = 0;
	(*transitions)[3][4] = 2;
	(*transitions)[4][5] = 3;
	(*transitions)[5][0] = 2;


	owners->push_back(1);
	owners->push_back(1);
	owners->push_back(1);
	owners->push_back(0);
	owners->push_back(0);
	owners->push_back(1);




	for (unsigned int i = 0; i < states->size(); ++i){
		resets->push_back(vector<bool>());
		for (unsigned int j = 0; j < states->size(); ++j)
			(*resets)[i].push_back(false);
	}


	for (unsigned int i = 0; i < transitions->size(); ++i){
		startsCstraints->push_back(vector<Fraction>());
		endsCstraints->push_back(vector<Fraction>());
		for (unsigned int j = 0; j < (*transitions)[i].size(); ++j){
			(*startsCstraints)[i].push_back(-1);
			(*endsCstraints)[i].push_back(-1);
		}

	}

	(*startsCstraints)[1][3] = 2;
	(*endsCstraints)[1][3] = 2;
	(*startsCstraints)[2][1] = 1;
	(*endsCstraints)[2][1] = 2;
	(*startsCstraints)[2][3] = 0;
	(*endsCstraints)[2][3] = 2;
	(*startsCstraints)[2][4] = 0;
	(*endsCstraints)[2][4] = 1;
	(*startsCstraints)[2][5] = 0;
	(*endsCstraints)[2][5] = 2;
	(*startsCstraints)[3][4] = 0;
	(*endsCstraints)[3][4] = 1;
	(*startsCstraints)[3][0] = 1;
	(*endsCstraints)[3][0] = 2;
	(*startsCstraints)[4][5] = 1;
	(*endsCstraints)[4][5] = 2;
	(*startsCstraints)[5][0] = 0;
	(*endsCstraints)[5][0] = 2;


	(*resets)[3][4] = true;
	(*resets)[2][5] = true;

	nbResets = 2;
	cout << "PTG initalized" << endl;
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

Fraction PTG::getEndCst(unsigned int origin, unsigned int dest)const{
	return (*endsCstraints)[origin][dest];
}

bool PTG::getReset(unsigned int origin, unsigned int dest) const{
	return (*resets)[origin][dest];
}

unsigned int PTG::getNbResets() const{
	return nbResets;
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
