#include <iostream>
#include "PerfEvaluator.hpp"
#include "PTG.hpp"
#include "PTGFactory.hpp"
#include "PTGSolver.hpp"
#include <cstring>
#include <stdio.h>
#include "Fraction.hpp"
#include "Point.hpp"
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[]){

	//!!!!!!!!Needed to be able to build different games
	srand (time(NULL));



	freopen("output.txt","w",stdout);
	bool perf = false;
	bool visu = false;
	bool v2 = false;
	char* file = NULL;

	for (int i = 1; i < argc; ++i){
		if(strcmp(argv[i], "-perf") == 0)
			perf = true;
		else if(strcmp(argv[i],"-v") == 0)
			visu = true;
		else if(strcmp(argv[i],"-v2") == 0)
			v2 = true;
		else
			file = argv[i];
	}

	if(perf){
		PerfEvaluator perf;
		if(visu)
			cerr << "Can't do visualization and perfomances!" << endl;
		perf.eval(v2);

	}
	else if(visu){
		PTGFactory factory;
		PTG* ptg;

		if(file != NULL)
			ptg = factory.buildFromFile(file);
		else
			ptg = factory.buildPTG(10,50,1,2,2,3);
			//ptg = factory.hardBuild(0);
		PTGSolver solver;
		solver.solvePTG(ptg, true, v2);
		cerr << "breakpoints: " << solver.getBreakPoints() << endl;

		delete ptg;
	}
	else if(v2){
		PTGFactory factory;
		PTG* ptg;

		if(file != NULL)
			ptg = factory.buildFromFile(file);
		else
			ptg = factory.hardBuild(0);
		PTGSolver solver;
		solver.solvePTG(ptg, false,v2);
		delete ptg;
	}
	else if (file != NULL){
		PTGFactory factory;
		PTG* ptg = factory.buildFromFile(argv[1]);
		PTGSolver solver;
		solver.solvePTG(ptg, true,v2);
		delete ptg;
	}
	else if (argc == 1){
		PTGFactory factory;
		PTG* ptg = factory.hardBuild(1);
		PTGSolver solver;
		solver.solvePTG(ptg, false,v2);
		delete ptg;
	}
	fclose (stdout);
	return 1;
}
