#include <iostream>
#include "PerfEvaluator.hpp"
#include "PTG.hpp"
#include "PTGFactory.hpp"
#include "PTGSolver.hpp"
#include <cstring>
#include <stdio.h>

using namespace std;

int main(int argc, char *argv[]){
	freopen("output.txt","w",stdout);

	if(argc > 1 && strcmp(argv[1], "-perf") == 0){

		PerfEvaluator perf;
		perf.eval();
}
	else if(argc > 1 && strcmp(argv[1], "-v") == 0){
		PTGFactory factory;
		PTG* ptg = factory.hardBuild(1);
		ptg->show();
		PTGSolver solver;
		solver.solvePTG(ptg, true);
	}
	else if (argc > 1){
		PTGFactory factory;
		PTG* ptg = factory.buildFromFile(argv[1]);
		PTGSolver solver;
		solver.solvePTG(ptg, true);
	}
	else if (argc == 1){
		PTGFactory factory;
		PTG* ptg = factory.hardBuild(1);
		PTGSolver solver;
		solver.solvePTG(ptg, false);
	}
	fclose (stdout);

return 1;
}
