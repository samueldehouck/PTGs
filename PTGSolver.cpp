/*
 * Author: Samuel Dehouck
 */


#include "PTGSolver.hpp"
#include "PTGVisu.hpp"
#include "stdlib.h"
#include <iostream>
#include <fstream>
#include <list>

using namespace std;

PTGSolver::PTGSolver(){
	size = 0;
	ptg = NULL;
	size = 0;
	time = 0;
}


void PTGSolver::solvePTG(PTG* p, bool visu, unsigned int version, bool strats, bool outputFcts){
	cout << "====SolvePTG====" << endl;


	ptg = p;
	ptg->write("problem");

	size = ptg->getSize();
	ptg->show();
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
			//We need to reset everything
			for(unsigned int i = 0; i < size; ++i)
				valueFcts[i].push_front(Point(time,0,Strategy(0,0,true)));


			valueFcts[0].front().getStrategy().setType(0);
			for (unsigned int i = 1; i < size; ++i){
				vals[i].setInf(true);

				pathsLengths.push_back(Value());
				//To force taking the first transitions available.
				if(ptg->getOwner(i))
					pathsLengths[i].setInf(true);
				valueFcts[i].front().setDest(-1);
				valueFcts[i].front().setType(false);
			}
			//Update the transitions that can be taken between the two given parameters
			keepTransAvailable(time, time);


			//First extendedDijkstra on the biggest "M"
			cout << "====First extended Dijkstra====" << endl;
			PGSolver* pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &valueFcts, &resets);
			pgSolver->extendedDijkstra(false);
			delete pgSolver;
			//Updating the value functions
			for (unsigned int i = 1; i < size; ++i){
				valueFcts[i].front().setX(time);
				valueFcts[i].front().setY(vals[i]);
			}
			//show();

			while(!endPoints.empty()){

				for(unsigned int i = 0; i < size; ++i)
					valueFcts[i].push_front(Point(time, 0, Strategy(0,0,false)));

				time = endPoints.back();
				cout << "===TIME: " << time << " ===" << endl;

				endPoints.pop_back();
				//First ExtendedDijkstra done at the last "M"

				keepTransAvailable(time, endM);
				updateBottoms();

				pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &valueFcts, &bottoms, &resets);

				pgSolver->extendedDijkstra(true);
				delete pgSolver;

				//show();

				//All rates need to be multiplied by the length of the interval
				for(unsigned int i = 0; i < size; ++i)
					ptg->setState(i,ptg->getState(i) * (endM - time));

				if(version == 1){
					SPTGSolver* sptgSolver = new SPTGSolver(ptg, &bottoms, &pathsLengths, &vals, &valueFcts, &resets);
					sptgSolver->solveSPTG();
					delete sptgSolver;
				}
				else if (version == 2){
					SPTGSolverValIt* sptgSolver = new SPTGSolverValIt(ptg, &bottoms, &pathsLengths, &vals, &valueFcts, &resets, outputFcts);
					sptgSolver->solveSPTG();
					delete sptgSolver;
				}


				//The resolution of a SPTG is done between 0 and 1, we need to rescale the valueFcts
				rescale(time, endM);

				//All rates need to be restored as they were before solveSPTG
				for(unsigned int i = 0; i < size; ++i)
					ptg->setState(i,ptg->getState(i) / (endM - time));
				//show();

				//The last step is to do an extendedDijkstra on the game in the "time" instant
				for(unsigned int i = 0; i < size; ++i){
					vals[i] = valueFcts[i].front().getY();
					valueFcts[i].push_front(Point(time, 0, Strategy(0,0,true)));
				}

				keepTransAvailable(time, time);
				updateBottoms();


				pgSolver = new PGSolver(ptg, &pathsLengths, &vals, &valueFcts, &bottoms, &resets);
				pgSolver->extendedDijkstra(true);
				for (unsigned int i = 0; i < size; ++i){
					valueFcts[i].front().setY(vals[i]);
				}

				delete pgSolver;
				endM = time;
				//show();
			}

			restoreAllTrans();
			updateResets();
			//show();
			--copyNb;

		}
		correctStrats();

		show();
		if(strats){
			cerr << "Drawing..." << endl;
			visualize(false, true);
		}
		else if (visu){
			cerr << "Drawing..." << endl;
			visualize(true, true);
		}
		cleanValueFcts();
		cout << endl << "====Results SolvePTG===" << endl;
		show();
	}
	else
		cout << "The game is empty" << endl;

}

void PTGSolver::init(){
	//Build all vectors and fill them with zeros
	vals.push_back(CompositeValue());
	pathsLengths.push_back(0);
	bottoms.push_back(0);
	valueFcts.push_back(list<Point>());

	for (unsigned int i = 1; i < size; ++i){
		vals.push_back(CompositeValue());
		bottoms.push_back(0);
		valueFcts.push_back(list<Point>());
	}
}

void PTGSolver::createEndPoints(){
	//   Create all endpoints where calculations will be needed
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

	//Restore all transitions that can be taken but were stored
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
	//Done when we have completely solver a copy of the game
	while(!storage.empty()){
		ptg->setTransition(storage.front().origin, storage.front().dest,storage.front().cost);
		storage.pop_front();
	}
}

void PTGSolver::updateBottoms(){
	//Update the bottom transitions
	for (unsigned int i = 0; i < size; ++i){
		bottoms[i] = vals[i];
	}
}

void PTGSolver::rescale(Value start, Value end){
	//The result given by the solveSPTG needs to be rescaled (from (0,1) to (start, end))
	cout << "====Rescaling====" << endl;
	/*for (vector<list<Point> >::iterator it = valueFcts.begin(); it != valueFcts.end(); ++it){
		for (list<Point>::iterator itL = it->begin(); itL != it->end() && itL->getX() <= 1 ; ++itL){
			itL->setX((itL->getX() * (end.getVal() - start.getVal())) + start);
		}
	}*/
	for (unsigned int i = 0; i < size; ++i){
		for (list<Point>::iterator it = valueFcts[i].begin(); it != valueFcts[i].end() && it->getX() < 1; ++it)
			it->setX((it->getX() * (end.getVal() - start.getVal())) + start);
	}
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
				if(!itLast->getY().isInfinity() && !itCurrent->getY().isInfinity() && !itNext->getY().isInfinity() && (itLast->getX().getVal() <= itCurrent->getX().getVal()) && (itCurrent->getX().getVal() <= itNext->getX().getVal())){
					Value coef = (itNext->getY() - itLast->getY())/(itNext->getX() - itLast->getX());
					if(itLast->getY() + (coef * (itCurrent->getX() - itLast->getX())) == itCurrent->getY()){
						itV->erase(itCurrent);
						deleted = true;
					}
				}
				else if(itLast->getY().isInfinity() && itCurrent->getY().isInfinity() && itNext->getY().isInfinity()&& (itLast->getX().getVal() <= itCurrent->getX().getVal()) && (itCurrent->getX().getVal() <= itNext->getX().getVal())){
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

void PTGSolver::correctStrats(){
	//Correct the strategies, when a lambda or a bottom transition is taken it means we wait until we make an action

	for (unsigned int i = 0; i < size; ++i){
		list<Point>::reverse_iterator rit = valueFcts[i].rbegin();
		list<Point>::reverse_iterator ritPrev = rit;
		++rit;
		for(; rit != valueFcts[i].rend(); ++rit){
			if(ritPrev->getType() == 1 || ritPrev->getType() == 2){
				ritPrev->setDest(rit->getDest());
			}

		}
	}
}



void PTGSolver::createResets(){
	//Create the matrix of resets used by all the solver
	for (unsigned int i = 0; i < ptg->getSize(); ++i){
		resets.push_back(vector<CompositeValue>());
		for (unsigned int j = 0; j < ptg->getSize(); ++j){
			if(ptg->getReset(i,j)){
				resets[i].push_back(CompositeValue());
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
	cout << "Strategies: " << endl;

	for (unsigned int i = 0; i < size; ++i){
		cout << "State " << i << ": " << endl;;
		for (list<Point>::iterator it = valueFcts[i].begin(); it != valueFcts[i].end(); ++it){
			cout << "t: " << it->getX() << " -> ";
			it->getStrategy().show();
		}
		cout << endl;
	}


	cout << "Vals:" << endl;
	for (unsigned int i = 0; i < vals.size(); ++i){
		cout << vals[i] << "	";
	}
	cout << endl;


	cout << "Value Functions" << endl;
	for (unsigned int i = 1; i < valueFcts.size(); ++i){
		if(ptg->hasLabels())
			cout << "State " << ptg->getLabel(i) << ": ";
		else
			cout << " State " << i <<": ";
		for(list<Point>::iterator it = valueFcts[i].begin(); it != valueFcts[i].end(); ++it){
			cout << "(" << it->getX() << "," << it->getY() << ") ";
		}
		cout << endl;
	}
}

void PTGSolver::visualize(bool vals, bool strats){
	PTGVisu v;
	if(vals)
		v.visualizeVals(ptg, &valueFcts);
	if(strats)
		v.visualizeStrats(ptg, &valueFcts);
}

vector<list<Point> >* PTGSolver::getValueFcts(){
	return &valueFcts;
}

unsigned int PTGSolver::getBreakPoints(){
	//Only works well on SPTG
	unsigned int max = 0;
	for (unsigned int i = 0; i < size; ++i){
		if(valueFcts[i].size() - 2  > max )
			max = valueFcts[i].size() - 2;
	}
	return max;
}

