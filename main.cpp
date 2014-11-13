/*
 * Author: Samuel Dehouck
 */

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
#include <sys/time.h>

using namespace std;

int main(int argc, char *argv[]){

	//!!!!!!!!Needed to be able to build different games
	srand (time(NULL));



	freopen("output.txt","w",stdout);
	bool perf = false;
	bool visu = false;
	unsigned int version = 1;
	bool sat = false;
	char* file = NULL;

	for (int i = 1; i < argc; ++i){
		if(strcmp(argv[i], "-perf") == 0)
			perf = true;
		else if(strcmp(argv[i],"-v") == 0)
			visu = true;
		else if(strcmp(argv[i],"-v2") == 0)
			version = 2;
		else if (strcmp(argv[i], "-v3") == 0)
			version = 3;
		else if(strcmp(argv[i],"-sat") == 0)
			sat = true;
		else
			file = argv[i];
	}

	if(perf){
		PerfEvaluator perf;
		if(visu)
			cerr << "Can't do visualization and perfomances!" << endl;
		perf.eval(version);

	}
	else if(sat){
		PTGFactory factory;
		PTG* ptg;

		if(file != NULL)
			ptg = factory.buildFromFile(file);
		else
			//ptg = factory.buildPTG(100,500,15,20,20,10);
			ptg = factory.hardBuild(0);
		PTGSolver solver;
		solver.solvePTG(ptg, false, version, sat);
		cerr << "breakpoints: " << solver.getBreakPoints() << endl;

		delete ptg;
	}
	else if(visu){
		PTGFactory factory;
		PTG* ptg;

		if(file != NULL)
			ptg = factory.buildFromFile(file);
		else
			//ptg = factory.buildPTG(100,500,15,20,20,10);
			ptg = factory.hardBuild(0);
		PTGSolver solver;
		solver.solvePTG(ptg, true, version, false);
		cerr << "breakpoints: " << solver.getBreakPoints() << endl;

		delete ptg;
	}
	else if(version == 2){
		PTGFactory factory;
		PTG* ptg;

		if(file != NULL)
			ptg = factory.buildFromFile(file);
		else
			ptg = factory.hardBuild(0);
		PTGSolver solver;
		solver.solvePTG(ptg, false,version, false);
		delete ptg;
	}
	else if(version == 3){
		PTGFactory factory;
		PTG* ptg;

		if(file != NULL)
			ptg = factory.buildFromFile(file);
		else
			ptg = factory.hardBuild(0);
		PTGSolver solver;
		solver.solvePTG(ptg, false, version, false);
		delete ptg;
	}
	else if (file != NULL){
		PTGFactory factory;
		PTG* ptg = factory.buildFromFile(argv[1]);
		PTGSolver solver;
		solver.solvePTG(ptg, true,version, false);
		delete ptg;
	}
	else if (argc == 1){
		PTGFactory factory;
		PTG* ptg = factory.buildPTG(100,9000,15,20,20,10);
		PTGSolver solver;

		struct timeval start, end;
		gettimeofday(&start, NULL);
		solver.solvePTG(ptg, false, version, false);
		gettimeofday(&end, NULL);
		cerr << "time: " <<  1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000 << endl;

		delete ptg;
	}
	fclose (stdout);



	return 1;
}
