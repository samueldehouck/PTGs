/*
 * Author: Samuel Dehouck
 */


#include "SPTGTester.hpp"
#include "PTG.hpp"
#include "PTGSolver.hpp"
#include "PTGFactory.hpp"
#include <iostream>
#include <list>
using namespace std;



SPTGTester::SPTGTester() {
	// TODO Auto-generated constructor stub

}

SPTGTester::~SPTGTester() {
	// TODO Auto-generated destructor stub
}


void SPTGTester::test(){
	unsigned int nbTests = 100;
	bool equal = true;

	PTGSolver* solverVF;
	PTGSolver* solverVI;
	PTGFactory factory;
	PTG* ptg;
	for (unsigned int nbStates = 3; equal && nbStates < 10; ++nbStates){
		if(outputEnabled)
			cerr << "nbStates = " << nbStates << endl;;
		for(unsigned int nbTrans = nbStates; equal && nbTrans < (nbStates - 1)*(nbStates - 1); ++nbTrans){
			if(outputEnabled)
				cerr << nbTrans << ":" ;
			for (unsigned int i = 0; i < nbTests && equal; ++i){

				if(outputEnabled)
					cerr << ".";
				equal = false;
				ptg = factory.buildSPTG(nbStates,nbTrans,20,50);
				solverVF = new PTGSolver();
				solverVF->solvePTG(ptg,false,1,false);

				solverVI = new PTGSolver();
				solverVI->solvePTG(ptg,false,2,false);
				if(solverVF->getBreakPoints() == solverVI->getBreakPoints() && compareVF(ptg->getSize(),  solverVF->getValueFcts(), solverVI->getValueFcts()))
					equal = true;
				delete solverVF;
				delete solverVI;

			}
			if(outputEnabled)
				cerr << endl;
		}
		if(outputEnabled)
			cerr << endl;
	}
	if(outputEnabled)
		cerr << endl;
	if(!equal){
		if(outputEnabled)
			cerr << "PROB FOUND" << endl;
		ptg->write("prob.txt");
	}
}

bool SPTGTester::compareVF(unsigned int size, vector<list<Point> >* vFSI, vector<list<Point> >* vFVI){
	for (unsigned int i = 1; i < size; ++i){
		if((*vFSI)[i].size() != (*vFVI)[i].size()){
			if(outputEnabled)
				cerr << "NOT SAME SIZE" << endl;
			return false;
		}
		list<Point>::iterator itS = (*vFSI)[i].begin();
		list<Point>::iterator itV = (*vFVI)[i].begin();
		for (; itS != (*vFSI)[i].end() && itV != (*vFVI)[i].end(); ++itS, ++itV){
			if(itS->getInf() != itV->getInf()){
				if(outputEnabled)
					cerr << "NOT BOTH INF" << endl;
				return false;
			}
			if(!itS->getInf() && (itS->getX() != itV->getX() || itS->getY() != itV->getY())){
				if(outputEnabled)
					cerr << "NOT EQUAL" << endl;
				return false;
			}
		}
	}
	return true;
}
