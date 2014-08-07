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

	//Update the transitions that can be taken between the two given parameters
	keepTransAvailable(time, time);
	ptg->show();

	//First extendedDijkstra on the biggest "M"
	cout << "====First extended Dijkstra====" << endl;
	PGSolver* pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies);
	pgSolver->extendedDijkstra(false);
	delete pgSolver;

	//Updating the value functions
	for (unsigned int i = 0; i < size; ++i){
		valueFcts[i].push_front(Point(time,vals[i][0]));
	}
	show();

	//Start of the (future) loop
	//Fraction x = (Fraction(endPoints.back() + lastM))/(Fraction(2));
	//strategies.push_front(Strategy(size, x));

	strategies.push_front(Strategy(size, time, false));
	keepTransAvailable(endPoints.back(), lastM);
	updateBottoms();
	pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies, &bottoms);
	pgSolver->extendedDijkstra(true);
	createMax(endM, lastM - endPoints.back());
	ptg->show();
	show();
	delete pgSolver;

	SPTGSolver* sptgSolver = new SPTGSolver(ptg, &bottoms, &pathsLengths, &vals, &strategies, &valueFcts);

	sptgSolver->solveSPTG();
	delete sptgSolver;

	rescale(endPoints.back(), lastM);
	ptg->show();
	deleteMax();
	keepTransAvailable(endPoints.back(), endPoints.back());
	updateBottoms();
	ptg->show();
	strategies.push_front(Strategy(size, endPoints.back(), true));
	time = endPoints.back();
	endPoints.pop_back();
	pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies, &bottoms);
	pgSolver->extendedDijkstra(true);
	for (unsigned int i = 0; i < size; ++i){
		valueFcts[i].push_front(Point(time,vals[i][0]));
	}

	cout << "====Results SolvePTG===" << endl;
	show();

}

void PTGSolver::init(){
	vals.push_back(vector<Fraction>());
	vals[0].push_back(0);
	vals[0].push_back(0);

	strategies.push_front(Strategy(size, time, true));
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
	cout << "===Updating Transitions====" << endl;

	//Restore the transitions stored
	list<Transition>::iterator next = storage.begin();
	for (list<Transition>::iterator it = storage.begin(); it != storage.end(); it = next){
		++next;
		if(ptg->getStartCst(it->origin, it->dest) <= start && ptg->getEndCst(it->origin, it->dest) >= end){
			ptg->setTransition(storage.front().origin, storage.front().dest, storage.front().cost);
			storage.erase(it);

		}

	}

	//Store the ones that can't be taken
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
	for (unsigned int i = 0; i < size; ++i){
		ptg->setState(i, ptg->getState(i) * d);
		if(ptg->getOwner(i) && ptg->getTransition(i, 0) != -1){
			Fraction tmp = ptg->getTransition(i, 0);
			storage.push_front(Transition(i,0,tmp));
			ptg->setTransition(i, size - 1, tmp);
			ptg->setTransition(i, 0, -1);
		}
	}
	vals.push_back(vector<Fraction>());
	vals.back().push_back(0);
	vals.back().push_back(0);
	valueFcts.push_back(list<Point>());

	//The transition from MAX to bottom is considered as a bottom transition
	bottoms[size - 1] = 0;
}


void PTGSolver::rescale(unsigned int start, unsigned int end){
	cout << "====Rescaling====" << endl;
	for (vector<list<Point> >::iterator it = valueFcts.begin(); it != valueFcts.end(); ++it){
		for (list<Point>::iterator itL = it->begin(); itL != it->end() && itL->getX() <= 1 ; ++itL){
			itL->setX((itL->getX() * Fraction(end - start)) + start);
		}
	}
	for (list<Strategy>::iterator it = strategies.begin(); it != strategies.end() && it->getTime() < 1; ++it)
		it->setTime((it->getTime() * Fraction(end - start)) + start);
}

void PTGSolver::deleteMax(){
	cout << "====Deleting Max====" << endl;
	ptg->deleteMaxState();
	size = ptg->getSize();
	vals.pop_back();
	valueFcts.pop_back();
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
