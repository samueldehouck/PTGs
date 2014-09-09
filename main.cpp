#include <iostream>
#include "PerfEvaluator.hpp"
#include "PTG.hpp"
#include "PTGFactory.hpp"
#include "PTGSolver.hpp"
#include <cstring>
#include <stdio.h>
#include "Fraction.hpp"
#include "Point.hpp"

using namespace std;

int main(int argc, char *argv[]){
	freopen("output.txt","w",stdout);

	if(argc > 1 && strcmp(argv[1], "-perf") == 0){
		PerfEvaluator perf;
		perf.eval();
}
	else if(argc > 1 && strcmp(argv[1], "-v") == 0){
		PTGFactory factory;
		PTG* ptg;
		// ptg = factory.buildPTG(5,12,0,3,3,1);
		//ptg = factory.buildSPTG(6,10,3,3);
		ptg = factory.hardBuild(0);
		PTGSolver solver;
		solver.solvePTG(ptg, true);
		delete ptg;
	}
	else if (argc > 1){
		PTGFactory factory;
		PTG* ptg = factory.buildFromFile(argv[1]);
		PTGSolver solver;
		solver.solvePTG(ptg, true);
		delete ptg;
	}
	else if (argc == 1){
		PTGFactory factory;
		PTG* ptg = factory.hardBuild(1);
		PTGSolver solver;
		solver.solvePTG(ptg, false);
		delete ptg;
	}
	fclose (stdout);
	return 1;
}
