/*
 * Author: Samuel Dehouck
 */


#include "PerfEvaluator.hpp"
#include "PTGFactory.hpp"
#include "PTG.hpp"
#include "PTGSolver.hpp"
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <sstream>
#include <unistd.h>

using namespace std;

PerfEvaluator::PerfEvaluator(){
	nbTests = 1000;
}

PerfEvaluator::PerfEvaluator(double nbT){
	nbTests = nbT;
}

void PerfEvaluator::eval(){
	nbBreakPoints();
}

void PerfEvaluator::nbBreakPoints(){
	PTGFactory factory;
	PTG* ptg;
	PTGSolver* solver;
	vector<vector<double> > results;
	unsigned int minState = 4;
	unsigned int maxState = 5;
	for (unsigned int nbBreaks = 1; nbBreaks < 2; ++nbBreaks){
		//Create new file
		for (unsigned int i = minState; i < maxState; ++i){
			results.push_back(vector<double>());
			unsigned int max = (i - 1)*(i - 1);
			unsigned int step = max / 10;
			cerr << step << endl;
			for (unsigned int j = 1; j < 10; ++j){
				double cnt = 0;
				for(unsigned int count = 0; count < nbTests; ++count){
					cerr << "truc" << endl;
					ptg = factory.buildSPTG(i,j*step,50,50);
					solver = new PTGSolver();
					solver->solvePTG(ptg,false,1,false,false);
					if (solver->getBreakPoints() == nbBreaks)
						++cnt;
					delete solver;
				}
				results[i].push_back(cnt/nbTests);
				cerr << i << " " << j << ": " << results[i][j] << endl;
			}
		}
	}
}
