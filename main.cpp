/*
 * Author: Samuel Dehouck
 */

#include <iostream>
#include "Constants.hpp"
#include "PerfEvaluator.hpp"
#include "PTG.hpp"
#include "PTGFactory.hpp"
#include "PTGSolver.hpp"
#include "SPTGTester.hpp"
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
	bool test = false;
	bool output = false;
	bool debug = false;
	unsigned int version = 1;
	char* file = NULL;
	char* fctsFile = NULL;

	for (int i = 1; i < argc; ++i){
		if (strcmp(argv[i], "-test") == 0)
			test = true;
		else if (strcmp(argv[i], "-debug") == 0)
			debug = true;
		else if(strcmp(argv[i], "-perf") == 0)
			perf = true;
		else if (strcmp(argv[i], "-vi") == 0)
			version = 2;
		else if (strcmp(argv[i], "-output") == 0)
			output = true;
		else if (file != NULL && fctsFile == NULL)
			fctsFile = argv[i];
		else
			file = argv[i];
	}

	if(output){
		outputEnabled = true;
	}

	if (test){
		SPTGTester tester;
		tester.test();
	}
	else if(debug){
		SPTGTester tester;
		tester.debug(version);
	}
	else if(fctsFile != NULL){
		PTGFactory factory;
		PTG* ptg;

		ptg = factory.buildFromFile(file);
		factory.buildOutputFcts(fctsFile, ptg);
		PTGSolver solver;
		solver.solvePTG(ptg, true, 3,  true);
		delete ptg;
	}
	else if(perf){
		PerfEvaluator perf;
		perf.eval();
	}
	else if (file != NULL){
		PTGFactory factory;
		PTG* ptg = factory.buildFromFile(file);
		PTGSolver solver;
		solver.solvePTG(ptg, true,version, false);
		delete ptg;
	}
	else
		cerr << "Nothing to be done..." << endl;

	fclose (stdout);



	return 1;
}
