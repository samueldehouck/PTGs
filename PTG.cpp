/*
 * Author: Samuel Dehouck
 */


#include "PTG.hpp"
#include <iostream>
#include <fstream>


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
		transitions->push_back(vector<Value>());
		owners->push_back(0);
		states->push_back(0);
		resets->push_back(vector<bool>());
		startsCstraints->push_back(vector<Fraction>());
		endsCstraints->push_back(vector<Fraction>());
		outputFcts.push_back(new list<Point>());
		for(unsigned int j = 0; j < size; ++j){
			(*transitions)[i].push_back(-1);
			(*resets)[i].push_back(false);
			(*startsCstraints)[i].push_back(-1);
			(*endsCstraints)[i].push_back(-1);
		}
	}
}

PTG::PTG(unsigned int size, bool label){
	//Just in the case where the input is an xml file
	resets = new vector<vector<bool> >();
	startsCstraints = new vector<vector<Fraction> >();
	endsCstraints = new vector<vector<Fraction> >();
	nbResets = 0;


	for (unsigned int i = 0; i < size; ++i){
		transitions->push_back(vector<Value>());
		owners->push_back(0);
		states->push_back(0);
		if(label)
			labels.push_back("");
		resets->push_back(vector<bool>());
		startsCstraints->push_back(vector<Fraction>());
		endsCstraints->push_back(vector<Fraction>());
		outputFcts.push_back(new list<Point>());

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
	for (unsigned int i = 0; i < outputFcts.size();++i)
		delete outputFcts[i];
}

Fraction PTG::getStartCst(unsigned int origin, unsigned int dest) const{
	if(origin < (*startsCstraints).size() && dest < (*startsCstraints)[origin].size())
		return (*startsCstraints)[origin][dest];
	else
		return 0;
}

void PTG::setStartCst(unsigned int origin, unsigned int dest, Fraction val){
	if(origin < (*startsCstraints).size() && dest < (*startsCstraints)[origin].size())
		(*startsCstraints)[origin][dest] = val;
}

Fraction PTG::getEndCst(unsigned int origin, unsigned int dest)const{
	if(origin < (*endsCstraints).size() && dest < (*endsCstraints)[origin].size())
		return (*endsCstraints)[origin][dest];
	else
		return 0;
}

void PTG::setEndCst(unsigned int origin, unsigned int dest, Fraction val){
	if(origin < (*endsCstraints).size() && dest < (*endsCstraints)[origin].size())
		(*endsCstraints)[origin][dest] = val;
}

bool PTG::getReset(unsigned int origin, unsigned int dest) const{
	if(origin < (*resets).size() && dest < (*resets)[origin].size())
		return (*resets)[origin][dest];
	else
		return false;
}

void PTG::setReset(unsigned int origin, unsigned int dest, bool val){
	if(origin < (*resets).size() && dest < (*resets)[origin].size())
		(*resets)[origin][dest] = val;
}
unsigned int PTG::getNbResets() const{
	return nbResets;
}

void PTG::setNbResets(unsigned int nb){
	nbResets = nb;
}

string PTG::getLabel(unsigned int index) const{
	if(index < labels.size())
		return labels[index];
	else
		return "";
}

void PTG::setLabel(unsigned int index, string l){
	if (index < labels.size())
		labels[index] = l;
}

bool PTG::hasLabels(){
	if (labels.size() > 0)
		return true;
	return false;
}





void PTG::show(){
	if(outputEnabled){
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
}

void PTG::write(string s){
	ofstream f;
	string tmp = "samples/" + s;
	f.open(tmp.c_str());

	f << "====PTG===" << endl;
	f << "Rates: " << endl;
	for (unsigned int i =0; i < states->size(); ++i)
		f << (*states)[i] << " ";
	f << endl;

	f << "Transitions: "<< endl;
	for (unsigned int i = 0; i < transitions->size(); ++i){
		for (unsigned int j = 0; j < (*transitions)[i].size(); ++j)
			f << (*transitions)[i][j] << "	";
		f << endl;
	}

	f << "Owners: " << endl;
	for (unsigned int i = 0; i < owners->size(); ++i)
		f << (*owners)[i] << " ";
	f << endl;

	f << "Constraints: " << endl;
	for (unsigned int i = 0; i < startsCstraints->size(); ++i){
		for (unsigned int j = 0; j < (*startsCstraints)[i].size(); ++j){
			f << "[" << (*startsCstraints)[i][j] << "," << (*endsCstraints)[i][j] << "]	";
		}
		f << endl;
	}
	f << "Resets: " << endl;
	for(unsigned int i = 0; i < resets->size(); ++i){
		for (unsigned int j = 0; j < (*resets)[i].size(); ++j)
			f << (*resets)[i][j] << "	";
		f << endl;
	}

	f << endl;


	f.close();

}
