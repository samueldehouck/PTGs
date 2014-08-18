#include "PTGSolver.hpp"

#include <iostream>

using namespace std;

PTGSolver::PTGSolver(){
	size = 0;
}

void PTGSolver::solvePTG(PTG* p){
	cout << "====SolvePTG====" << endl;
	ptg = p;
	size = ptg->getSize();
	int copyNb = ptg->getNbResets();
	createResets();
	while (copyNb >= 0){
		cout << "====Solving copy nb: " << copyNb << " ====" << endl;



		createEndPoints();
		unsigned int endM = endPoints.back();
		endPoints.pop_back();
		time = endM;
		if(copyNb == (int)ptg->getNbResets())
			init();//Init is after createEndPoints because the time needs to be updated before creating the Strategy object
		//Update the transitions that can be taken between the two given parameters
		keepTransAvailable(time, time);


		//First extendedDijkstra on the biggest "M"
		cout << "====First extended Dijkstra====" << endl;
		PGSolver* pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies, &resets);
		pgSolver->extendedDijkstra(false);
		delete pgSolver;
		//Updating the value functions
		for (unsigned int i = 0; i < size; ++i){
			valueFcts[i].push_front(Point(time,vals[i][0]));
		}

		//Start of the (future) loop
		while(!endPoints.empty()){


			time = endPoints.back();
			cout << "===TIME: " << time << " ===" << endl;

			endPoints.pop_back();
			//First ExtendedDijkstra done at the last "M"
			strategies.push_front(Strategy(size, endM, false));
			keepTransAvailable(time, endM);
			updateBottoms();

			pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies, &bottoms, &resets);

			pgSolver->extendedDijkstra(true);
			delete pgSolver;

			//We need to update the time to get the interval for the resolution of the SPTG
			createMax(endM, endM - time);
			//ptg->show();

			//The solveSPTG is done on the new SPTG with the "max" state
			SPTGSolver* sptgSolver = new SPTGSolver(ptg, &bottoms, &pathsLengths, &vals, &strategies, &valueFcts, &resets);
			sptgSolver->solveSPTG();
			delete sptgSolver;


			//The resolution of a SPTG is done between 0 and 1, we need to rescale the valueFcts
			rescale(time, endM);

			deleteMax();
			show();
			//The last step is to do an extendedDijkstra on the game in the "time" instant
			keepTransAvailable(time, time);
			updateBottoms();
			strategies.push_front(Strategy(size, time, true));

			for (unsigned int i = 0; i < size; ++i){
					for (unsigned int j = 0; j < size; ++j){
						cout << resets[i][j] << "	";
					}
					cout << endl;
				}


			pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies, &bottoms, &resets);
			pgSolver->extendedDijkstra(true);
			for (unsigned int i = 0; i < size; ++i){
				valueFcts[i].push_front(Point(time,vals[i][0]));
			}

			cleanValueFcts();
			endM = time;
			show();
		}
		restoreAllTrans();
		updateResets();
		show();
		--copyNb;

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
			resets[it->origin][it->dest] = -1;
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

void PTGSolver::restoreAllTrans(){
	while(!storage.empty()){
		ptg->setTransition(storage.front().origin, storage.front().dest,storage.front().cost);
		storage.pop_front();
	}
}

void PTGSolver::updateBottoms(){
//Update the bottom transitions
	for (unsigned int i = 0; i < size; ++i){
		bottoms[i] = vals[i][0];
	}
}

void PTGSolver::createMax(const unsigned int endM, const unsigned int d){
	cout << "====Creating MAX==== "<< endl;
	//Create the new sptg with the MAX state
	ptg->createMaxState(ifnty, endM);
	size = ptg->getSize();
	resets.push_back(vector<Fraction>());
	for (unsigned int i = 0; i < size; ++i){
		resets[i].push_back(-1);
		resets[size - 1].push_back(-1);
	}
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
	//The result given by the solveSPTG
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
	//Restore the game as it was before the construction of the SPTG with "MAX"
	cout << "====Deleting Max====" << endl;
	ptg->deleteMaxState();
	size = ptg->getSize();
	vals.pop_back();
	valueFcts.pop_back();
}

void PTGSolver::cleanValueFcts(){
	//In the case where three consecutive points are on the same line, we can delete the middle one
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

void PTGSolver::createResets(){
	for (unsigned int i = 0; i < ptg->getSize(); ++i){
		resets.push_back(vector<Fraction>());
		for (unsigned int j = 0; j < ptg->getSize(); ++j){
			if(ptg->getReset(i,j)){
				resets[i].push_back(ifnty);
			}
			else{
				resets[i].push_back(-1);
			}
		}
	}
}

void PTGSolver::updateResets(){
	cout << "===Update resets===" << endl;

	//The cost of the reset transition is the sum between the cost of the transition and the value of the destination
	for (unsigned int i = 0; i < size; ++i){
		for (unsigned int j = 0; j < size; ++j){
			if(ptg->getReset(i,j)){
				cout << i << " " << j << " " << ptg->getTransition(i,j) << " " << vals[j][0] << endl;
				resets[i][j] = ptg->getTransition(i,j) + vals[j][0];
			}
		}
	}
cout << "===Update resets===" << endl;
	for (unsigned int i = 0; i < size; ++i){
		for (unsigned int j = 0; j < size; ++j){
			cout << resets[i][j] << "	";
		}
		cout << endl;
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
