#include "SPTGSolverValIt.hpp"

SPTGSolverValIt::SPTGSolverValIt() {
	// TODO Auto-generated constructor stub

}

SPTGSolverValIt::SPTGSolverValIt(SPTG* s, vector<Value>* b,  vector<Value>* pl, vector<CompositeValue>* v, vector<list<Point> >* vF, vector<vector<CompositeValue> >* r){
	sptg = s;
	solvePTG = true;
	bottoms = b;
	pathsLengths = pl;
	vals = v;
	valueFcts = vF;
	resets = r;

	size = sptg->getSize();
	if(size != 0){

		for (unsigned int i = 0; i < size; ++i){

			//Used to force the player 2 to, at least, set his strategy
			updated.push_back(false);
			complete.push_back(false);
			copyValsSrc.push_back(new list<Point>());
			copyVals.push_back(new list<Point>());
			defined.push_back(false);
		}
		complete[0] = true;
	}
}

SPTGSolverValIt::~SPTGSolverValIt() {
	if(!solvePTG)
		delete valueFcts;
}

void SPTGSolverValIt::show(){

	cout << "Lambdas:" << endl;
	for (unsigned int i = 0; i < lambdas.size(); ++i)
		cout <<  lambdas[i] << "	";

	cout << endl;

	cout << "Strategies: " << endl;

	for (unsigned int i = 0; i < size; ++i){
		cout << "State " << i << ": ";
		for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it){
			cout << "t: " << it->getX() << " ";
			it->getStrategy().show();
		}
		cout << endl;
	}

	cout << "Lengths: " << endl;
	for (unsigned int i = 0; i < pathsLengths->size(); ++i)
		cout << (*pathsLengths)[i] << "	";
	cout << endl;

	cout << "Complete:" << endl;
	for (unsigned int i = 0; i < size; ++i)
		cout << complete[i] << "	";
	cout << endl;

	cout << "CopySrc" << endl;
	for (unsigned int i = 1; i < size; ++i){
		cout << " State " << i <<": ";
		for(list<Point>::iterator it = copyValsSrc[i]->begin(); it != copyValsSrc[i]->end(); ++it){
			cout << "(" << it->getX() << "," << it->getY() << ")	";
		}
		cout << endl;
	}
	cout << "Copy" << endl;
	for (unsigned int i = 1; i < size; ++i){
		cout << " State " << i <<": ";
		for(list<Point>::iterator it = copyVals[i]->begin(); it != copyVals[i]->end(); ++it){
			cout << "(" << it->getX() << "," << it->getY() << ")	";
		}
		cout << endl;
	}
}

void SPTGSolverValIt::copyValueFcts(){
	for (unsigned int i = 0; i < size; ++i){

		if(!complete[i])
			//If the value has completely been computed, copyVals[i] == copyValsSrc[i]
			delete copyValsSrc[i];
		copyValsSrc[i] = copyVals[i];
		copyVals[i] = new list<Point>();
		if(!copyValsSrc[i]->empty())
			defined[i] = true;
	}
}

bool SPTGSolverValIt::compareCopy(){
	bool different = false;
	for (unsigned int i = 0; i < size; ++i){
		if(copyVals[i]->size() != copyValsSrc[i]->size()){
			different = true;
		}
		else{
			list<Point>::iterator itC = copyVals[i]->begin();
			list<Point>::iterator itSrc = copyValsSrc[i]->begin();

			while(itC != copyVals[i]->end() && !different){
				cout << "diff?" << endl;
				if(itC->getX() != itSrc->getX() || itC->getY() != itSrc->getY())
					different = true;
				++itC;
				++itSrc;
			}
		}
	}
	cout << different << endl;
	return different;
}

void SPTGSolverValIt::solveSPTG(){
	cout << "====SolveSPTG===" << endl;
	FunctionsMinMax minMax;
	PGSolver* ps;
	if(solvePTG){
		ps = new PGSolver(sptg, pathsLengths, vals, valueFcts, bottoms, resets);//PGSolver will consider sptg as a pg thanks to inheritance
		ps->extendedDijkstra(true);
	}
	else{
		ps = new PGSolver(sptg, pathsLengths, vals, valueFcts, resets);//PGSolver will consider sptg as a pg thanks to inheritance
		ps->extendedDijkstra(false); //If extendedDijkstra returns false, some states can't be treated and there is a cycle
	}

	//Initialization of the value iteration
	for(unsigned int i = 0; i < size; ++i){
		if(sptg->getTransition(i,0) != -1){
			defined[i] = true;
			copyValsSrc[i]->push_front((*valueFcts)[i].front());
			if(sptg->getOwner(i)){
				copyValsSrc[i]->push_front(Point(0,(*valueFcts)[i].front().getY(), Strategy(0,0,false)));
			}
			else{
				copyValsSrc[i]->push_front(Point(0,(*valueFcts)[i].front().getY() + sptg->getState(i), Strategy(0,1,false)));
			}
		}
	}

	delete ps;
	show();

	//!!!! This algorithm is convergent under some hypothesis
	unsigned int cnt = 0;

	while(compareCopy() && cnt < 10){
		cout << "Turn " << cnt << endl;

		if(cnt != 0)
			copyValueFcts();



		for (unsigned int i = 1; i < size; ++i){
			cout << "State: " << i << endl;
			updated[i] = false;
			complete[i] = true;

			for (unsigned int j = 0; j < size; ++j){
				if(sptg->getTransition(i,j) != -1 && !complete[j])
					complete[i] = false;
			}

			if(complete[i]){
				copyVals[i] = copyValsSrc[i];
			}
			else{
				for (unsigned int j = 1; j < size; ++j){
					if(sptg->getTransition(i,j) != -1 && defined[j]){
						if(!updated[i]){
							for (list<Point>::iterator it = copyValsSrc[j]->begin(); it != copyValsSrc[j]->end(); ++it){
								if(it->getX() != 0 && it->getX() != 1)
									copyVals[i]->push_back(Point(it->getX(), it->getY() + sptg->getTransition(i,j), Strategy(j, 0, true)));
								else
									copyVals[i]->push_back(Point(it->getX(), it->getY() + sptg->getTransition(i,j), Strategy(j, 0, false)));

							}
							updated[i] = true;
						}
						copyVals[i] = minMax.getMinMax(sptg, copyVals[i], i, copyValsSrc[j], j, !sptg->getOwner(i));
					}
				}
			}
		}
		show();
		++cnt;

	}

	cout << "Result: " << endl;
	show();
	copyValueFcts();
	for (unsigned int i = 0; i < size; ++i){
		for(list<Point>::reverse_iterator rit = copyValsSrc[i]->rbegin(); rit != copyValsSrc[i]->rend(); ++rit){
			(*valueFcts)[i].push_front(Point(rit->getX(), rit->getY(), rit->getStrategy()));
		}
	}
}



