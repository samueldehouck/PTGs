#include "PTGFactory.hpp"

PTGFactory::PTGFactory(){

}

PTG* PTGFactory::build(){
	PTG* ptg = new PTG(nbStates);
	srand (time(NULL));
	ptg->setNbResets(nbResets);
	//Generates everything
	for(unsigned int i = 1; i < ptg->getSize(); ++i){
		int r = rand() % maxNbTrans + minNbTrans;
		while(r != 0){
			int index = rand() % nbStates;
			if(((unsigned int)index != i) && ptg->getTransition(i,index) == -1){
				ptg->setTransition(i,index, rand() % (maxCost + 1));
				int endCst = rand() % (maxCst + 1);
				ptg->setEndCst(i, index, endCst);
				ptg->setStartCst(i, index, rand() % (endCst + 1));
				--r;
			}
		}
		ptg->setState(i, rand() % (maxRate + 1) + minRate);
		ptg->setOwner(i, rand() % 2);
	}
	int tmp = nbResets;
	while(tmp != 0){
		int i = rand() % nbStates;
		int j = rand() % nbStates;
		if(ptg->getTransition(i,j) != -1){
			ptg->setReset(i,j,true);
			--tmp;
		}
	}

	return ptg;
}
