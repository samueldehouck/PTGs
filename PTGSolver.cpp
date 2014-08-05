#include "PTGSolver.hpp"

#include <iostream>

using namespace std;

PTGSolver::PTGSolver(){
	size = 0;
}

void PTGSolver::solvePTG(PTG* p){
	ptg = p;
	size = ptg->getSize();


	createEndPoints();
	unsigned int endM = endPoints.back();
	unsigned int lastM = endM;
	endPoints.pop_back();
	time = lastM;

	init();//Init is after createEndPoints because the time needs to be updated before creating the Strategy object

	keepTransAvailable(time, time);
	ptg->show();

	//First extendedDijkstra on the biggest "M"
	PGSolver* pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies);
	pgSolver->extendedDijkstra(false);
	delete pgSolver;

	//Start of the loop
	Fraction x = (Fraction(endPoints.back() + lastM))/(Fraction(2));
	strategies.push_front(Strategy(size, x));


	keepTransAvailable(endPoints.back(), lastM);
	updateBottoms();
	pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies, &bottoms);
	pgSolver->extendedDijkstra(true);
	createMax(endM, lastM - endPoints.back());
	updateBottoms();
	ptg->show();

	delete pgSolver;

	SPTGSolver* sptgSolver = new SPTGSolver(ptg, &bottoms, &pathsLengths, &vals, &strategies);
	sptgSolver->solveSPTG();
	//delete sptgSolver;
	cout << "====Results===" << endl;
	//show();
}

void PTGSolver::init(){
	vals.push_back(vector<Fraction>());
	vals[0].push_back(0);
	vals[0].push_back(0);

	strategies.push_front(Strategy(size, time));
	strategies.front().insert(0,0,false);

	pathsLengths.push_back(0);

	bottoms.push_back(0);

	valueFcts.push_back(list<Point>());

	// Fill the initial valors, strategies and the ensemble of states
	for (unsigned int i = 1; i < size; ++i){
		vals.push_back(vector<Fraction>());
		vals[i].push_back(ifnty);
		vals[i].push_back(0);

		strategies.front().insert(i, -1, false);
		pathsLengths.push_back(0);

		bottoms.push_back(0);

		valueFcts.push_back(list<Point>());
	}

}

void PTGSolver::createEndPoints(){
	//  TODO will need to improve
	//   Create all endpoints where some calculation will be needed
	for (unsigned int i = 0; i < size; ++i){
		for (unsigned int j = 0; j < size; ++j){
			if(ptg->getTransition(i,j) != -1){
				endPoints.push_back(ptg->getStartCst(i,j));
				endPoints.push_back(ptg->getEndCst(i,j));
			}
		}
	}

	endPoints.sort();
	endPoints.unique();

}

void PTGSolver::keepTransAvailable(unsigned int start, unsigned int end){
//TODO needs to be improved

	while (!storage.empty()){
		ptg->setTransition(storage.front().origin, storage.front().dest, storage.front().dest);
		storage.pop_front();
	}

	for (unsigned int i = 0; i < size; ++i){
		for (unsigned int j = 0; j < size; ++j){
			if(ptg->getTransition(i,j) != -1 && (ptg->getStartCst(i,j) > start || ptg->getEndCst(i,j) < end)){
				storage.push_back(Transition(i, j, ptg->getTransition(i,j)));
				ptg->setTransition(i,j,-1);
			}
		}
	}
}


void PTGSolver::updateBottoms(){

	for (unsigned int i = 0; i < vals.size(); ++i){
			bottoms[i] = vals[i][0];
		}
}

void PTGSolver::createMax(const unsigned int endM, const unsigned int d){
	cout << "====Creating MAX====" << endl;
	//Create the new sptg with the MAX state
	ptg->createMaxState(ifnty, endM);
	size = ptg->getSize();
	ptg->setTransition(size - 1, 0, 0);
	for (unsigned int i = 0; i < size - 1; ++i){
		ptg->setState(i, ptg->getState(i) * d);
		if(ptg->getOwner(i) && ptg->getTransition(i, 0) != -1){
			Fraction tmp = ptg->getTransition(i, 0);
			storage.push_front(Transition(i,0,tmp));
			ptg->setTransition(i, size - 1, tmp);
			ptg->setTransition(i, 0, -1);
		}

	}
}




void PTGSolver::show(){

	cout << "bottoms:" << endl;
	for (unsigned int i = 0; i < bottoms.size(); ++i){
			cout <<  bottoms[i]<< "	";
		}
		cout << endl;


	cout << "Strategies: " << endl;
	for (list<Strategy>::iterator it = strategies.begin(); it != strategies.end(); ++it){
		it->show();
	}
	cout << endl;

	cout << "Lengths: " << endl;
	for (unsigned int i = 0; i < pathsLengths.size(); ++i)
		cout << pathsLengths[i] << "	";
	cout << endl;

	cout << "Vals:" << endl;
	for (unsigned int i = 0; i < vals.size(); ++i){
		for (unsigned int j = 0; j < vals[i].size(); ++j){
			cout << vals[i][j] << "	";
		}
		cout << endl;
	}

	cout << "Value Functions" << endl;
	for (unsigned int i = 1; i < valueFcts.size(); ++i){
		cout << " State " << i <<": ";
		for(list<Point>::iterator it = valueFcts[i].begin(); it != valueFcts[i].end(); ++it){
			cout << "(" << it->getX() << "," << it->getY() << ")	";
		}
		cout << endl;
	}
}
