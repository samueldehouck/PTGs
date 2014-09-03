#include "PTGFactory.hpp"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

PTGFactory::PTGFactory(){

}

PTG* PTGFactory::build(int nbStates, int nbTrans, int nbResets, int maxRate, int maxCost, int maxCst){
	//cerr << "====Building the PTG====" << endl;
	PTG* ptg = new PTG(nbStates);
	srand (time(NULL));
	ptg->setNbResets(nbResets);
	//Generates everything
	ptg->setOwner(0,1);
	ptg->setState(0,0);
	for (unsigned int i = 1; i < ptg->getSize(); ++i){
		ptg->setState(i, rand() % (maxRate + 1));
		ptg->setOwner(i, rand()%2);
	}
	int tmp = nbTrans;
	while (tmp != 0){
		int i = rand() % (nbStates - 1) + 1;
		int j = rand() % nbStates;
		if(ptg->getTransition(i,j) == -1 && i != j){
			ptg->setTransition(i,j,rand() % (maxCost + 1));
			int endCst = rand() % (maxCst + 1);
			ptg->setEndCst(i, j, endCst);
			ptg->setStartCst(i, j, rand() % (endCst + 1));
			--tmp;
		}
	}

	tmp = nbResets;
	while(tmp != 0){
		int i = rand() % nbStates;
		int j = rand() % nbStates;
		if(ptg->getTransition(i,j) != -1){
			ptg->setReset(i,j,true);
			--tmp;
		}
	}
	ptg->show();
	return ptg;
}


PTG* PTGFactory::buildFromFile(char* f){
	ifstream file(f);
	string line = "";
	getline(file,line);//====PTG====
	getline(file,line);//Rates:
	getline(file,line);
	istringstream* stream = new istringstream(line);
	int n;
	vector<int> v;

	while (*stream >> n)
	{
		v.push_back(n);
	}
	PTG* ptg = new PTG(v.size());
	for (unsigned int i = 0; i < v.size(); ++i)
		ptg->setState(i,v[i]);

	delete stream;
	getline(file,line);//Transitions:
	for (unsigned int i = 0; i < ptg->getSize(); ++i){
		getline(file, line);
		stream = new istringstream(line);
		for (unsigned int j = 0; *stream >> n && j < ptg->getSize(); ++j){
			ptg->setTransition(i,j,n);
		}
		delete stream;
	}
	getline(file,line);//Owners
	getline(file, line);
	stream = new istringstream(line);
	for (unsigned i = 0; *stream >> n && i < ptg->getSize(); ++i){
		ptg->setOwner(i,n);
	}
	getline(file,line);//Constraints
	for (unsigned int i = 0; i < ptg->getSize(); ++i){
		getline(file, line);
		int index = 0;
		for (unsigned int j = 0; j < ptg->getSize(); ++j){
			string next = "";
			while (line[index] != '[')
				++index;
			++index;
			while(line[index] != ','){
				next.push_back(line[index]);
				++index;
			}
			n = atoi(next.c_str());
			ptg->setStartCst(i,j,n);
			++index;
			next = "";
			while(line[index] != ']'){
				next.push_back(line[index]);
				++index;
			}
			n = atoi(next.c_str());
			ptg->setEndCst(i,j,n);
		}
	}
	getline(file,line);//Resets:
	unsigned int cnt = 0;
	for (unsigned int i = 0; i < ptg->getSize(); ++i){
		getline(file, line);
		stream = new istringstream(line);
		for (unsigned int j = 0; *stream >> n && j < ptg->getSize(); ++j){
			ptg->setReset(i,j,(bool)n);
			if(n == 1)
				++cnt;
		}
		delete stream;
	}
	ptg->setNbResets(cnt);
	ptg->show();
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
		ptg->setTransition(1,3,2);
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


		ptg->setReset(1,4,true);
		ptg->setNbResets(1);

		//ptg->setNbResets(0);

		ptg->setStartCst(1,3,1);
		ptg->setEndCst(1,3,3);
		ptg->setStartCst(1,4,0);
		ptg->setEndCst(1,4,2);
		ptg->setStartCst(2,1,1);
		ptg->setEndCst(2,1,3);
		ptg->setStartCst(3,0,0);
		ptg->setEndCst(3,0,3);
		ptg->setStartCst(3,4,2);
		ptg->setEndCst(3,4,2);
		ptg->setStartCst(4,2,2);
		ptg->setEndCst(4,2,2);
		ptg->setStartCst(4,0,1);
		ptg->setEndCst(4,0,3);
	}
	else if(build == 5){
		ptg = new PTG(4);
		ptg->setState(0,0);
		ptg->setState(1,2);
		ptg->setState(2,1);
		ptg->setState(3,1);

		ptg->setOwner(0,1);
		ptg->setOwner(1,1);
		ptg->setOwner(2,0);
		ptg->setOwner(3,1);

		ptg->setTransition(1,3,3);
		ptg->setTransition(2,0,1);
		ptg->setTransition(2,1,3);
		ptg->setTransition(2,3,0);
		ptg->setTransition(3,0,3);
		ptg->setTransition(3,2,1);


		ptg->setNbResets(0);
		ptg->setStartCst(1,3,1);
		ptg->setStartCst(2,0,0);
		ptg->setStartCst(2,1,0);
		ptg->setStartCst(2,3,1);
		ptg->setStartCst(3,0,1);
		ptg->setStartCst(3,2,0);

		ptg->setEndCst(1,3,2);
		ptg->setEndCst(2,0,2);
		ptg->setEndCst(2,1,1);
		ptg->setEndCst(2,3,1);
		ptg->setEndCst(3,0,2);
		ptg->setEndCst(3,2,2);

	}
	else if(build == 6){

		ptg = new PTG(5);
		ptg->setState(0,0);
		ptg->setState(1,2);
		ptg->setState(2,1);
		ptg->setState(3,2);
		ptg->setState(4,1);

		ptg->setOwner(0,1);
		ptg->setOwner(1,1);
		ptg->setOwner(2,1);
		ptg->setOwner(3,0);
		ptg->setOwner(3,0);

		ptg->setTransition(1,3,1);
		ptg->setTransition(1,2,1);
		ptg->setTransition(2,3,0);
		ptg->setTransition(2,4,2);
		ptg->setTransition(3,2,2);
		ptg->setTransition(3,0,1);
		ptg->setTransition(4,0,2);

		ptg->setStartCst(1,3,0);
		ptg->setStartCst(1,2,0);
		ptg->setStartCst(2,3,0);
		ptg->setStartCst(2,4,0);
		ptg->setStartCst(3,2,1);
		ptg->setStartCst(3,0,0);
		ptg->setStartCst(4,0,0);

		ptg->setEndCst(1,3,2);
		ptg->setEndCst(1,2,2);
		ptg->setEndCst(2,3,2);
		ptg->setEndCst(2,4,1);
		ptg->setEndCst(3,2,2);
		ptg->setEndCst(3,0,2);
		ptg->setEndCst(4,0,2);

		ptg->setNbResets(2);
		ptg->setReset(1,2,true);
		ptg->setReset(3,2,true);
	}


	return ptg;
}
