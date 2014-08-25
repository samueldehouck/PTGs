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

PTG* PTGFactory::hardBuild(unsigned int build){
	PTG* ptg;
	if(build == 1)
	{
		ptg = new PTG(6);

		ptg->setState(0,0);
		ptg->setState(1,3);
		ptg->setState(2,2);
		ptg->setState(3,0);
		ptg->setState(4,1);
		ptg->setState(5,5);

		ptg->setTransition(1,3,1);
		ptg->setTransition(2,1,2);
		ptg->setTransition(2,3,1);
		ptg->setTransition(2,4,2);
		ptg->setTransition(2,5,1);
		ptg->setTransition(3,0,0);
		ptg->setTransition(3,4,2);
		ptg->setTransition(4,5,3);
		ptg->setTransition(5,0,2);

		ptg->setOwner(0,1);
		ptg->setOwner(1,1);
		ptg->setOwner(2,1);
		ptg->setOwner(3,0);
		ptg->setOwner(4,0);
		ptg->setOwner(5,1);

		ptg->setStartCst(1,3,2);
		ptg->setStartCst(2,1,1);
		ptg->setStartCst(2,3,0);
		ptg->setStartCst(2,4,0);
		ptg->setStartCst(2,5,0);
		ptg->setStartCst(3,4,0);
		ptg->setStartCst(3,0,1);
		ptg->setStartCst(4,5,1);
		ptg->setStartCst(5,0,0);
		ptg->setEndCst(1,3,2);
		ptg->setEndCst(2,1,2);
		ptg->setEndCst(2,3,2);
		ptg->setEndCst(2,4,1);
		ptg->setEndCst(2,5,2);
		ptg->setEndCst(3,4,1);
		ptg->setEndCst(3,0,2);
		ptg->setEndCst(4,5,2);
		ptg->setEndCst(5,0,2);

		ptg->setReset(3,4,true);
		ptg->setReset(2,5,true);

		ptg->setNbResets(2);
	}
	else if (build == 2){
		ptg = new PTG(5);

		ptg->setState(0,0);
		ptg->setState(1,1);
		ptg->setState(2,3);
		ptg->setState(3,1);
		ptg->setState(4,3);

		ptg->setTransition(1,2,2);
		ptg->setTransition(1,3,1);
		ptg->setTransition(2,0,5);
		ptg->setTransition(2,3,0);
		ptg->setTransition(3,0,1);
		ptg->setTransition(3,4,3);
		ptg->setTransition(4,1,2);

		ptg->setOwner(0,1);
		ptg->setOwner(1,0);
		ptg->setOwner(2,1);
		ptg->setOwner(3,0);
		ptg->setOwner(4,1);

		ptg->setStartCst(1,2,1);
		ptg->setStartCst(1,3,0);
		ptg->setStartCst(2,0,0);
		ptg->setStartCst(2,3,1);
		ptg->setStartCst(3,0,0);
		ptg->setStartCst(3,4,0);
		ptg->setStartCst(4,1,0);
		ptg->setEndCst(1,2,2);
		ptg->setEndCst(1,3,2);
		ptg->setEndCst(2,0,1);
		ptg->setEndCst(2,3,2);
		ptg->setEndCst(3,0,2);
		ptg->setEndCst(3,4,2);
		ptg->setEndCst(4,1,2);

		ptg->setReset(4,1,true);
		ptg->setReset(2,3,true);
		ptg->setNbResets(2);
	}
	else if(build == 3){
		ptg = new PTG(6);


		ptg->setState(0,0);
		ptg->setState(1,2);
		ptg->setState(2,5);
		ptg->setState(3,1);
		ptg->setState(4,2);
		ptg->setState(5,3);

		ptg->setTransition(1,4,2);
		ptg->setTransition(2,1,3);
		ptg->setTransition(2,5,1);
		ptg->setTransition(3,0,1);
		ptg->setTransition(4,3,2);
		ptg->setTransition(4,0,0);
		ptg->setTransition(5,4,1);
		ptg->setTransition(5,0,2);

		ptg->setOwner(0,1);
		ptg->setOwner(1,1);
		ptg->setOwner(2,1);
		ptg->setOwner(3,1);
		ptg->setOwner(4,0);
		ptg->setOwner(5,0);


		ptg->setStartCst(1,4,0);
		ptg->setEndCst(1,4,3);
		ptg->setStartCst(2,1,0);
		ptg->setEndCst(2,1,1);
		ptg->setStartCst(2,5,2);
		ptg->setEndCst(2,5,3);
		ptg->setStartCst(3,0,0);
		ptg->setEndCst(3,0,3);
		ptg->setStartCst(4,3,1);
		ptg->setEndCst(4,3,3);
		ptg->setStartCst(4,0,2);
		ptg->setEndCst(4,0,2);
		ptg->setStartCst(5,4,1);
		ptg->setEndCst(5,4,2);
		ptg->setStartCst(5,0,0);
		ptg->setEndCst(5,0,3);

		ptg->setReset(4,3,true);
		ptg->setNbResets(1);
	}
	else if(build == 4){
		ptg = new PTG(5);



		ptg->setState(0,0);
		ptg->setState(1,6);
		ptg->setState(2,2);
		ptg->setState(3,5);
		ptg->setState(4,3);

		ptg->setTransition(1,3,1);
		ptg->setTransition(1,4,5);
		ptg->setTransition(2,1,1);
		ptg->setTransition(3,0,3);
		ptg->setTransition(3,4,1);
		ptg->setTransition(4,0,1);
		ptg->setTransition(4,2,2);


		ptg->setOwner(0,1);
		ptg->setOwner(1,1);
		ptg->setOwner(2,0);
		ptg->setOwner(3,0);
		ptg->setOwner(4,1);


		//ptg->setReset(1,4,true);
		//ptg->setNbResets(1);

		ptg->setNbResets(0);

		ptg->setStartCst(1,3,1);
		ptg->setEndCst(1,3,5);
		ptg->setStartCst(1,4,0);
		ptg->setEndCst(1,4,2);
		ptg->setStartCst(2,1,1);
		ptg->setEndCst(2,1,3);
		ptg->setStartCst(3,0,0);
		ptg->setEndCst(3,0,5);
		ptg->setStartCst(3,4,4);
		ptg->setEndCst(3,4,4);
		ptg->setStartCst(4,2,2);
		ptg->setEndCst(4,2,2);
		ptg->setStartCst(4,0,1);
		ptg->setEndCst(4,0,3);
	}
	return ptg;
}
