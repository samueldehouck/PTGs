/*
 * Author: Samuel Dehouck
 */


#include "SPTGSolverValIt.hpp"

SPTGSolverValIt::SPTGSolverValIt() {
	// TODO Auto-generated constructor stub

}

SPTGSolverValIt::SPTGSolverValIt(SPTG* s, vector<Value>* b,  vector<Value>* pl, vector<list<Point> >* vF, vector<vector<CompositeValue> >* r, bool outFcts){
	sptg = s;
	solvePTG = true;
	bottoms = b;
	pathsLengths = pl;
	valueFcts = vF;
	resets = r;
	outputFcts = outFcts;
	size = sptg->getSize();
	if(size != 0){

		for (unsigned int i = 0; i < size; ++i){
			complete.push_back(false);
			copyValsSrc.push_back(new list<Point>());
			copyVals.push_back(new list<Point>());

		}
		complete[0] = true;
	}
}

SPTGSolverValIt::~SPTGSolverValIt() {
	if(!solvePTG)
		delete valueFcts;
	for (unsigned int i = 0; i < copyValsSrc.size();++i)
		delete copyValsSrc[i];
	for (unsigned int i = 0; i < copyVals.size();++i)
		delete copyVals[i];
}

void SPTGSolverValIt::show(){
	if(outputEnabled)
	{
		cout << "Lambdas:" << endl;
		for (unsigned int i = 0; i < lambdas.size(); ++i)
			cout <<  lambdas[i] << "	";

		cout << endl;

		cout << "Lengths: " << endl;
		for (unsigned int i = 0; i < pathsLengths->size(); ++i)
			cout << (*pathsLengths)[i] << "	";
		cout << endl;

		cout << "Complete:" << endl;
		for (unsigned int i = 0; i < size; ++i)
			cout << complete[i] << "	";
		cout << endl;

		cout << "CopySrc" << endl;
		for (unsigned int i = 0; i < size; ++i){
			cout << " State " << i <<": ";
			for(list<Point>::iterator it = copyValsSrc[i]->begin(); it != copyValsSrc[i]->end(); ++it){
				cout << "(" << it->getX() << "," << it->getY() << ")	";
			}
			cout << endl;
		}
		cout << endl;

		cout << "CopyStrategies: " << endl;

		for (unsigned int i = 0; i < size; ++i){
			cout << "State " << i << ": ";
			for (list<Point>::iterator it = copyValsSrc[i]->begin(); it != copyValsSrc[i]->end(); ++it){
				cout << "t: " << it->getX() << " ";
				it->getStrategy().show();
			}
			cout << endl;
		}
		cout << endl;
		cout << "Copy" << endl;
		for (unsigned int i = 1; i < size; ++i){
			cout << " State " << i <<": ";
			for(list<Point>::iterator it = copyVals[i]->begin(); it != copyVals[i]->end(); ++it){
				cout << "(" << it->getX() << "," << it->getY() << ")	";
			}
			cout << endl;
		}
	}
}

void SPTGSolverValIt::copyValueFcts(){
	for (unsigned int i = 0; i < size; ++i){
		delete copyValsSrc[i];
		copyValsSrc[i] = copyVals[i];
		copyVals[i] = new list<Point>();
	}
}

bool SPTGSolverValIt::compareCopy(){
	if(outputEnabled)
		cout << "Compare" << endl;
	show();
	for (unsigned int i = 0; i < size; ++i){
		if(copyVals[i]->size() != copyValsSrc[i]->size()){
			return true;
		}
		else{
			list<Point>::iterator itC = copyVals[i]->begin();
			list<Point>::iterator itSrc = copyValsSrc[i]->begin();

			while(itC != copyVals[i]->end()){
				if(outputEnabled)
					cout << itC->getY().isInfinity() << " " << itSrc->getY().isInfinity() << endl;
				if((itC->getY().isInfinity() && !itSrc->getY().isInfinity())  || (itC->getY().isInfinity() && !itSrc->getY().isInfinity()))
					return true;
				else if(itC->getX() != itSrc->getX() || itC->getY() != itSrc->getY())
					return true;
				++itC;
				++itSrc;
			}
		}
	}
	return false;
}

void SPTGSolverValIt::solveSPTG(){
	if(outputEnabled)
		cout << "====SolveSPTG===" << endl;

	Value maxState = 0;
	for (unsigned int i = 0; i < sptg->getSize(); ++i){
		if ( maxState < sptg->getState(i)){
			maxState = sptg->getState(i);
		}
	}

	//Create the value fct of state max
	list<Point> valueMax;
	valueMax.push_front(Point(1,0,Strategy(0,0,false)));
	valueMax.push_front(Point(0,maxState,Strategy(0,1,false)));

	FunctionsMinMax minMax;
	//Fill the value of state 0
	copyValsSrc[0]->push_front(Point(1,0, Strategy(0,0,false)));
	copyValsSrc[0]->push_front(Point(0,0, Strategy(0,0,false)));



	//Initialization of the value iteration
	for(unsigned int i = 1; i < size; ++i){

		//Otherwise the value is initialized to the inf
		copyValsSrc[i]->push_front((*valueFcts)[i].front());
		copyValsSrc[i]->front().setX(1);
		copyValsSrc[i]->front().setInclusion(false);
		copyValsSrc[i]->front().setInf(true);
		copyValsSrc[i]->front().setType(1);
		copyValsSrc[i]->push_front(Point(0,0, Strategy(0,1,false)));
		copyValsSrc[i]->front().setInf(true);

		if (outputFcts && sptg->getFct(i)->size() != 0){
			//If there is a value fct at the exit
			copyValsSrc[i] = minMax.getMinMax(sptg, copyValsSrc[i], i, sptg->getFct(i), 0, !sptg->getOwner(i), sptg->getTransition(i, 0));
		}
	}

	show();

	unsigned int cnt = 0;

	while(compareCopy()){
		if(outputEnabled)
			cout << "Turn " << cnt << endl;

		if(cnt != 0)//If it isn't the first turn, we need to copy the value fcts to the src
			copyValueFcts();
		if(outputEnabled)
		{
			for (unsigned int i = 0; i < size; ++i)
				cout << complete[i] <<  " ";
			cout << endl;
		}
		for (unsigned int i = 0; i < size; ++i){

			if(outputEnabled)
				cout << "State: " << i << endl;

			if(complete[i]){
				for(list<Point>::iterator it = copyValsSrc[i]->begin(); it != copyValsSrc[i]->end(); ++it)
					copyVals[i]->push_back(Point(it->getX(),it->getY(),it->getStrategy()));
			}
			else{
				bool updated = false;
				bool nbSucc = 0;
				for (unsigned int j = 0; j < size; ++j){
					if(sptg->getTransition(i,j) != -1){
						++nbSucc;

						if((*resets)[i][j] == -1){
							if(!updated){

								//We need to copy the first function that is compared
								list<Point>::iterator it = copyValsSrc[j]->begin();
								while(it != copyValsSrc[j]->end()){
									if(it->getX() != 0 && it->getX() != 1)
										copyVals[i]->push_back(Point(it->getX(), it->getY() + sptg->getTransition(i,j), Strategy(j,0,true)));
									else
										copyVals[i]->push_back(Point(it->getX(), it->getY() + sptg->getTransition(i,j), Strategy(j,0,false)));

									++it;
								}

								//Try if waiting is better
								copyVals[i] = minMax.tryWaiting(copyVals[i], sptg->getState(i), !sptg->getOwner(i));

								updated = true;
							}
							else
								copyVals[i] = minMax.getMinMax(sptg, copyVals[i], i, copyValsSrc[j], j, !sptg->getOwner(i), sptg->getTransition(i, j));
						}
						else{

							//This transition has a reset
							//resets already contain the weight of the transition
							list<Point> reset;
							reset.push_front(Point(1,(*resets)[i][j],Strategy(j,0,false)));
							reset.push_front(Point(0,(*resets)[i][j],Strategy(j,0,false)));

							if(!updated){

								copyVals[i]->push_back(Point(0, (*resets)[i][j], Strategy(j,0,false)));
								copyVals[i]->push_back(Point(1, (*resets)[i][j], Strategy(j,0,false)));


								//Try if waiting is better
								copyVals[i] = minMax.tryWaiting(copyVals[i], sptg->getState(i), !sptg->getOwner(i));

								updated = true;
							}
							else
								copyVals[i] = minMax.getMinMax(sptg, copyVals[i], i, &reset, j, !sptg->getOwner(i), sptg->getTransition(i, j));
						}

					}
				}
				if(!bottoms->empty()){
					if(outputEnabled)
						cout << "try bottom transition" << endl;

					if(!updated){
						copyVals[i]->push_back(Point(0, (*bottoms)[i] + sptg->getState(i), Strategy(0,2,false)));
						copyVals[i]->push_back(Point(1, (*bottoms)[i], Strategy(0,2,false)));
					}
					else{
						if(sptg->getOwner(i))
							copyVals[i] = minMax.getMinMax(sptg, copyVals[i], i, &valueMax, 0, !sptg->getOwner(i), (*bottoms)[i]);
						else
							copyVals[i] = minMax.getMinMax(sptg, copyVals[i], i, copyValsSrc[0], 0, !sptg->getOwner(i), (*bottoms)[i]);
					}
				}


				if(outputFcts && sptg->getTransition(i,0) != -1){
					//If there are value fcts at the exit

					if (sptg->getFct(i)->size() != 0){
						copyVals[i] = minMax.getMinMax(sptg, copyVals[i], i, sptg->getFct(i), 0, !sptg->getOwner(i),  sptg->getTransition(i, 0));
					}
				}
			}

		}
		vector<bool> changed;
		for (unsigned int i = 0; i < size; ++i){
			changed.push_back(true);
			if(!complete[i]){
				//If the value fct is complete (all states that can be reached are complete) we just need to copy it
				for (unsigned int j = 0; j < size; ++j){
					if(sptg->getTransition(i,j) != -1 && !complete[j])
						changed[i] = false;
				}
			}
		}

		for (unsigned int i = 0; i < size; ++i){
			if(changed[i])
				complete[i] = true;
		}

		++cnt;
	}

	if(outputEnabled)
		cout << "Result: " << endl;
	show();

	for (unsigned int i = 0; i < size; ++i){
		for(list<Point>::reverse_iterator rit = copyValsSrc[i]->rbegin(); rit != copyValsSrc[i]->rend(); ++rit){
			if(rit != copyValsSrc[i]->rbegin())
				(*valueFcts)[i].push_front(Point(rit->getX(), rit->getY(), rit->getStrategy()));
		}
	}

}



