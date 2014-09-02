#include "PerfEvaluator.hpp"
#include "PTGFactory.hpp"
#include "PTG.hpp"
#include "PTGSolver.hpp"
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

void PerfEvaluator::eval(){
	cerr << "====Starting computing data===" << endl;
	struct timeval start, end;


	for (unsigned int nbStates = 15; nbStates < 25; ++nbStates){
		for (unsigned int nbTrans = nbStates; nbTrans < (nbStates - 1)*(nbStates - 1); ++nbTrans){

			PTGFactory factory;
			PTG* ptg = factory.build(nbStates,nbTrans,1,5,5,3);
			PTGSolver solver;
			gettimeofday(&start, NULL);

			solver.solvePTG(ptg, false);

			delete ptg;
			gettimeofday(&end, NULL);

			cerr << nbStates << " " << nbTrans << ":" << 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000 << "ms" << endl;
			usleep(300000);
		}
	}
	cerr << endl;

}
