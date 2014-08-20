#include "PTGFactory.hpp"

PTGFactory::PTGFactory(){

}

PTG* PTGFactory::build(){
	PTG* ptg = new PTG(nbStates);
	srand (time(NULL));
	ptg->setNbResets(nbResets);
	//Generates everything
	ptg->setOwner(0,1);
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

PTG* PTGFactory::hardBuild(){
	PTG* ptg = new PTG(4);

	ptg->setState(0,0);
	ptg->setState(1,0);
	ptg->setState(2,4);
	ptg->setState(3,4);

	ptg->setOwner(0,1);
	ptg->setOwner(1,1);
	ptg->setOwner(2,1);
	ptg->setOwner(3,0);

	ptg->setTransition(1,2,5);
	ptg->setTransition(2,0,0);
	ptg->setTransition(2,3,3);
	ptg->setTransition(3,0,5);
	ptg->setTransition(3,1,4);
	ptg->setTransition(3,2,4);


	ptg->setStartCst(1,2,2);
	ptg->setStartCst(2,0,2);
	ptg->setStartCst(2,3,1);
	ptg->setStartCst(3,0,1);
	ptg->setStartCst(3,1,2);
	ptg->setStartCst(3,2,0);


	ptg->setEndCst(1,2,2);
	ptg->setEndCst(2,0,2);
	ptg->setEndCst(2,3,2);
	ptg->setEndCst(3,0,1);
	ptg->setEndCst(3,1,2);
	ptg->setEndCst(3,2,0);

	ptg->setReset(3,2,true);

	ptg->setNbResets(1);






	return ptg;
}
