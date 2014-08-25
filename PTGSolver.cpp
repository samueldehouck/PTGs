#include "PTGSolver.hpp"

#include <iostream>

using namespace std;

PTGSolver::PTGSolver(){
	size = 0;
	ptg = NULL;
	size = 0;
	time = 0;
}

void PTGSolver::solvePTG(PTG* p){
	cout << "====SolvePTG====" << endl;
	ptg = p;
	size = ptg->getSize();
	if(size > 0){
		int copyNb = ptg->getNbResets();
		createResets();
		while (copyNb >= 0){
			cout << "====Solving copy nb: " << copyNb << " ====" << endl;



			createEndPoints();
			Value endM = endPoints.back();
			endPoints.pop_back();
			time = endM;
			if(vals.size() == 0)
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
				valueFcts[i].push_front(Point(time,vals[i]));
			}
			show();

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

				show();

				SPTGSolver* sptgSolver = new SPTGSolver(ptg, &bottoms, &pathsLengths, &vals, &strategies, &valueFcts, &resets);
				sptgSolver->solveSPTG();
				delete sptgSolver;

				//The resolution of a SPTG is done between 0 and 1, we need to rescale the valueFcts
				rescale(time, endM);
				show();

				//The last step is to do an extendedDijkstra on the game in the "time" instant
				keepTransAvailable(time, time);
				updateBottoms();
				strategies.push_front(Strategy(size, time, true));




				pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &strategies, &bottoms, &resets);
				pgSolver->extendedDijkstra(true);
				for (unsigned int i = 0; i < size; ++i){
					valueFcts[i].push_front(Point(time,vals[i]));
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
		cout << endl << "====Results SolvePTG===" << endl;
		show();
	}
	else
		cout << "The game is empty" << endl;

}

void PTGSolver::init(){
	vals.push_back(CompositeValue());

	strategies.push_front(Strategy(size, time, true));
	strategies.front().insert(0,0,false);

	pathsLengths.push_back(0);

	bottoms.push_back(0);

	valueFcts.push_back(list<Point>());


	// Fill the initial valors, strategies and the ensemble of states
	for (unsigned int i = 1; i < size; ++i){
		vals.push_back(CompositeValue());
		vals[i].setInf(true);

		strategies.front().insert(i, -1, false);
		pathsLengths.push_back(Value());
		//To force taking the first transitions available.
		if(ptg->getOwner(i))
			pathsLengths[i].setInf(true);


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

void PTGSolver::keepTransAvailable(Value start, Value end){
	cout << "===Updating Transitions (" << start << "," << end << ") ====" << endl;

	//Restore the transitions stored
	list<Transition>::iterator next = storage.begin();
	for (list<Transition>::iterator it = storage.begin(); it != storage.end(); it = next){
		++next;
		if(Value(ptg->getStartCst(it->origin, it->dest)) <= start && Value(ptg->getEndCst(it->origin, it->dest)) >= end){
			ptg->setTransition(it->origin, it->dest, it->cost);

			storage.erase(it);

		}

	}

	//Store the ones that can't be taken
	for (unsigned int i = 0; i < size; ++i){
		for (unsigned int j = 0; j < size; ++j){
			if(ptg->getTransition(i,j) != -1 && (Value(ptg->getStartCst(i,j)) > start || Value(ptg->getEndCst(i,j)) < end)){
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
	//cout << "updatebottoms" << endl;
	for (unsigned int i = 0; i < size; ++i){
		bottoms[i] = vals[i];
		//cout << bottoms[i] << " " << vals[i] << endl;
	}
}

void PTGSolver::rescale(Value start, Value end){
	//The result given by the solveSPTG
	cout << "====Rescaling====" << endl;
	for (vector<list<Point> >::iterator it = valueFcts.begin(); it != valueFcts.end(); ++it){
		for (list<Point>::iterator itL = it->begin(); itL != it->end() && itL->getX() <= 1 ; ++itL){
			itL->setX((itL->getX() * (end.getVal() - start.getVal())) + start);
		}
	}
	for (list<Strategy>::iterator it = strategies.begin(); it != strategies.end() && it->getTime() < 1; ++it)
		it->setTime((it->getTime() * (end.getVal() - start.getVal())) + start);
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
				Value coef = (itNext->getY() - itLast->getY())/(itNext->getX() - itLast->getX());
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
		resets.push_back(vector<Value>());
		for (unsigned int j = 0; j < ptg->getSize(); ++j){
			if(ptg->getReset(i,j)){
				resets[i].push_back(Value());
				resets[i][j].setInf(true);
			}
			else{
				resets[i].push_back(CompositeValue(-1));
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
				//cout << i << " " << j << " " << ptg->getTransition(i,j) << " " << vals[j][0] << endl;
				resets[i][j] = vals[j] + ptg->getTransition(i,j);
			}
		}
	}
}

void PTGSolver::show(){

	/*cout << "bottoms:" << endl;
	for (unsigned int i = 0; i < bottoms.size(); ++i){
		cout <<  bottoms[i]<< "	";
	}
	cout << endl;*/


	cout << "Strategies: " << endl;
	for (list<Strategy>::iterator it = strategies.begin(); it != strategies.end(); ++it){
		it->show();
	}
	cout << endl;

	/*cout << "Lengths: " << endl;
	for (unsigned int i = 0; i < pathsLengths.size(); ++i)
		cout << pathsLengths[i] << "	";
	cout << endl;*/

	cout << "Vals:" << endl;
	for (unsigned int i = 0; i < vals.size(); ++i){
			cout << vals[i] << "	";
	}
	cout << endl;


	cout << "Value Functions" << endl;
	for (unsigned int i = 1; i < valueFcts.size(); ++i){
		cout << " State " << i <<": ";
		for(list<Point>::iterator it = valueFcts[i].begin(); it != valueFcts[i].end(); ++it){
			cout << "(" << it->getX() << "," << it->getY() << ") ";
		}
		cout << endl;
	}
}
