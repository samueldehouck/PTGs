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
	endPoints.pop_back();
	time = endM;

	init();//Init is after createEndPoints because the time needs to be updated before creating the Strategy object

	//Update the transitions that can be taken between the two given parameters
	keepTransAvailable(time, time);
	//ptg->show();

	//First extendedDijkstra on the biggest "M"
	cout << "====First extended Dijkstra====" << endl;
	PGSolver* pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies);
	pgSolver->extendedDijkstra(false);
	delete pgSolver;

	//Updating the value functions
	for (unsigned int i = 0; i < size; ++i){
		valueFcts[i].push_front(Point(time,vals[i][0]));
	}
	//show();
	unsigned int count = 4;
	//Start of the (future) loop
	while(!endPoints.empty()){


		time = endPoints.back();
		cout << "===TIME: " << time << " ===" << endl;

		endPoints.pop_back();
		//First ExtendedDijkstra done at the last "M"
		strategies.push_front(Strategy(size, endM, false));
		keepTransAvailable(time, endM);
		updateBottoms();
		pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies, &bottoms);
		pgSolver->extendedDijkstra(true);
		delete pgSolver;

		//We need to update the time to get the interval for the resolution of the SPTG
		createMax(endM, endM - time);
		//ptg->show();

		//The solveSPTG is done on the new SPTG with the "max" state
		SPTGSolver* sptgSolver = new SPTGSolver(ptg, &bottoms, &pathsLengths, &vals, &strategies, &valueFcts);
		sptgSolver->solveSPTG();
		delete sptgSolver;

		//The resolution of a SPTG is done between 0 and 1, we need to rescale the valueFcts
		rescale(time, endM);
		//show();

		deleteMax();

		//The last step is to do an extendedDijkstra on the game in the "time" instant
		keepTransAvailable(time, time);
		updateBottoms();
		//ptg->show();
		strategies.push_front(Strategy(size, time, true));

		//Update the time

		pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies, &bottoms);
		pgSolver->extendedDijkstra(true);
		for (unsigned int i = 0; i < size; ++i){
			valueFcts[i].push_front(Point(time,vals[i][0]));
		}

		cleanValueFcts();
		endM = time;
		--count;
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
	cout << "===Updating Transitions (" << start << "," << end << ") ====" << endl;

	//Restore the transitions stored
	list<Transition>::iterator next = storage.begin();
	for (list<Transition>::iterator it = storage.begin(); it != storage.end(); it = next){
		++next;
		if(ptg->getStartCst(it->origin, it->dest) <= start && ptg->getEndCst(it->origin, it->dest) >= end){
			ptg->setTransition(it->origin, it->dest, it->cost);
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
	//ptg->show();
}


void PTGSolver::updateBottoms(){

	for (unsigned int i = 0; i < vals.size(); ++i){
		bottoms[i] = vals[i][0];
	}
}

void PTGSolver::createMax(const unsigned int endM, const unsigned int d){
	cout << "====Creating MAX==== "<< endl;
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

void PTGSolver::cleanValueFcts(){
	cout << "====Cleaning Value Fcts====" << endl;

	for (vector<list<Point> >::iterator itV = valueFcts.begin(); itV != valueFcts.end(); ++itV){
		//For every state
		list<Point>::iterator itNext = itV->begin();
		++itNext;
		if(itNext != itV->end()){
			list<Point>::iterator itCurrent = itV->begin();
			++itNext;
			++itCurrent;
			for (list<Point>::iterator itLast = itV->begin(); itNext != itV->end(); ++itNext){
				bool deleted = false;
				Fraction coef = (itNext->getY() - itLast->getY())/(itNext->getX() - itLast->getX());

				if(itLast->getY() + (coef * (itCurrent->getX() - itLast->getX())) == itCurrent->getY()){
					itV->erase(itCurrent);
					deleted = true;
				}
				if(deleted){
					//itCurrent is on a space that has been deleted so we can't do ++itCurrent and itLast needs to stay where it was
					itCurrent = itNext;
				}
				else{
					++itCurrent;
					++itLast;
				}
			}
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
			cout << "(" << it->getX() << "," << it->getY() << ") ";
		}
		cout << endl;
	}
}
