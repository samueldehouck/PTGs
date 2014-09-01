#include <iostream>
#include "SPTG.hpp"
#include "SPTGSolver.hpp"
#include "PTG.hpp"
#include "PTGSolver.hpp"
#include "PG.hpp"
#include "PTGFactory.hpp"
#include <sys/time.h>
#include <fstream>
#include <cstring>

using namespace std;

int main(int argc, char *argv[]){
	struct timeval start, end;

	ofstream file;
	file.open ("results.txt");


	for (unsigned int i = 0; i < 1; ++i){
		//freopen("output.txt","w",stdout);
		PTGFactory factory;
		PTG* ptg = factory.hardBuild(2);
		ptg->show();
		PTGSolver solver;
		gettimeofday(&start, NULL);

		if(argc > 1 && strcmp(argv[1], "-v") == 0){
			solver.solvePTG(ptg,true);
		}
		else{
			solver.solvePTG(ptg, false);

		}
		gettimeofday(&end, NULL);

		cerr << i << ":" << 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000 << "ms" << endl;
	}
	cerr << endl;


	return 1;
}
