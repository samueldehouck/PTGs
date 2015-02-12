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
	nbTests = 1;
}

PerfEvaluator::PerfEvaluator(double nbT){
	nbTests = nbT;
}

void PerfEvaluator::eval(){
//	nbBreakPoints();
//	timeBPs();
	timeStates();
	timeTrans();
	timeResets();
	timeConsts();
	timeWeights();
	timeRates();
}

void PerfEvaluator::nbBreakPoints(){
	PTGFactory factory;
	PTG* ptg;
	PTGSolver* solver;
	unsigned int minState = 4;
	unsigned int maxState = 25;


	double bigProb = 0;
	unsigned int bigS = 0;
	unsigned int bigT = 0;
	unsigned int bigCW = 0;
	unsigned int bestBP = 0;
	unsigned int bestS = 0;
	unsigned int bestT = 0;
	unsigned int bestCW = 0;
	ofstream f;
	f.open("results/nbBreakPoints.tex");
	f << "\\documentclass{article}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;
	for (unsigned int cstwei= 1; cstwei < 2; ++cstwei){
		cerr << "Constants and weights: " << cstwei*10 << endl;
		f << "Constants and weights: " << cstwei*10 << "\\\\" << endl;
		vector<vector<double> > results;
		for (unsigned int i = minState; i < maxState; ++i){
			cerr << "States: " << i << endl;
			results.push_back(vector<double>());
			unsigned int max = (i - 1)*(i - 1);
			double step = max / 10.0;
			for (unsigned int j = 1; j < 10; ++j){
				double cnt = 0;
				for(unsigned int count = 0; (int) (j*step) != 0 && count < nbTests; ++count){
					ptg = factory.buildSPTG(i,(int) (j*step),cstwei*10,cstwei*10);
					solver = new PTGSolver();
					solver->solvePTG(ptg,false,1,false);
					if (solver->getBreakPoints() != 0){
						++cnt;
						if(solver->getBreakPoints() > bestBP){
							bestBP = solver->getBreakPoints();
							bestS = i;
							bestT = (int)j*step;
							bestCW = cstwei;
						}
					}
					delete solver;
					delete ptg;
				}
				if(cnt != 0){
					results[i-minState].push_back(cnt/nbTests*(nbTests/100));
					if(cnt/nbTests*(nbTests/100) > bigProb){
						bigProb = cnt/nbTests*(nbTests/100);
						bigS = i;
						bigT = (int)(j*step);
						bigCW = cstwei;
					}
				}
				else
					results[i-minState].push_back(0);

			}
		}
		f << "\\resizebox{\\textwidth}{!}{%" << endl;
		f << "\\begin{tabular}[H]{";

		for (unsigned int i = 0; i < maxState - minState + 1; ++i ){
			f << "|c";
		}
		f << "|}" << endl;
		f << "\\hline" << endl;
		f << "$|S|$";
		for (unsigned int i = minState; i < maxState; ++i)
			f << " & " <<  i;
		f << "\\\\" << endl;
		f << "\\hline" << endl;

		for (unsigned int j = 0; j < results[0].size(); ++j){
			f << j + 1 << "0";
			for (unsigned int i = 0; i < results.size(); ++i)
				f << " & " << results[i][j];
			f << "\\\\" << endl;
			f << "\\hline" << endl;

		}
		f << "\\end{tabular}" << endl;
		f << "}" << endl;
		f << "\\vspace{1cm}" << endl << endl;
	}

	f << "Highest probability: " << bigProb << " with " << bigS << " States, " << bigT << " Transitions, " << bigCW*10 << " as max constant and weight \\\\";
	f << "Biggest number of breakPoints: " << bestBP << " with " << bestS << " States, " << bestT << " Transitions, " << bestCW*10 << " as max constant and weight \\\\";


	f << "\\end{document}" << endl;
	f.close();
	system("pdflatex results/nbBreakPoints.tex");
}

void PerfEvaluator::timeBPs(){
	PTGFactory factory;
	PTG* ptg;
	PTGSolver* solver;
	unsigned int nbStates = 30;
	unsigned int nbTrans = 80;
	vector<unsigned int> times;
	vector<unsigned int> counts;
	for (unsigned int bp = 0; bp < 10; ++bp){
		counts.push_back(0);
		times.push_back(0);
	}
	for (unsigned int i = 0; i < nbTests; ++i){
		ptg = factory.buildSPTG(nbStates, nbTrans, 50,50);
		solver = new PTGSolver();

		struct timeval start, end;
		gettimeofday(&start, NULL);
		solver->solvePTG(ptg,false,1,false);
		gettimeofday(&end, NULL);
		++counts[solver->getBreakPoints()];
		times[solver->getBreakPoints()] += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
		delete solver;
		delete ptg;
	}

	for (unsigned int i = 0; i < 10; ++i){
		if(counts[i] != 0)
			cerr << times[i]/counts[i] << " ";
		else
			cerr << "0" << " ";
	}
	cerr << endl;
}

void PerfEvaluator::timeStates(){
	cerr << "timeStates" << endl;
	PTGFactory factory;
	PTG* ptg;
	PTGSolver* solver;
	unsigned int minStates = 5;
	ofstream f;
	f.open("results/timeStates.txt");
    f << "(states, (states -1)*(states - 1) /2, 0, 50, 50, 10)" << endl;

	for (int states = minStates; states < 101; states += 5){
		double count = 0;
	for (unsigned int i = 0; i < nbTests; ++i){
		ptg = factory.buildPTG(states, (states -1)*(states - 1) /2, 0, 50, 50, 10);
		solver = new PTGSolver();

		struct timeval start, end;
		gettimeofday(&start, NULL);
		solver->solvePTG(ptg,false,1,false);
		gettimeofday(&end, NULL);
		count += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
		delete solver;
		delete ptg;
		}
	 f << states << ":" << count/nbTests << endl;
	}
	f.close();
}

void PerfEvaluator::timeTrans(){
	cerr << "timeTrans" << endl;

	PTGFactory factory;
	PTG* ptg;
	PTGSolver* solver;
	unsigned int states = 50;

	ofstream f;
	f.open("results/timeTrans.txt");
	f << "(states, step*trans, 0, 50, 50, 10)" << endl;
	unsigned int max = (states - 1 )*(states - 1);
	unsigned int step = max/10;
	for (int trans = 1; trans < 10; ++trans){
		double count = 0;
	for (unsigned int i = 0; i < nbTests; ++i){
		ptg = factory.buildPTG(states, step*trans, 0, 50, 50, 10);
		solver = new PTGSolver();

		struct timeval start, end;
		gettimeofday(&start, NULL);
		solver->solvePTG(ptg,false,1,false);
		gettimeofday(&end, NULL);
		count += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
		delete solver;
		delete ptg;
		}
	 f << trans << ":" << count/nbTests << endl;
	}
	f.close();
}

void PerfEvaluator::timeResets(){
	cerr << "timeResets" << endl;

	PTGFactory factory;
	PTG* ptg;
	PTGSolver* solver;
	unsigned int states = 50;

	ofstream f;
	f.open("results/timeResets.txt");
	f << "(states, (states-1)*(states-1)/2, resets, 50, 50, 10)" << endl;
	for (int resets = 0; resets < 100; resets += 5){
		double count = 0;
	for (unsigned int i = 0; i < nbTests; ++i){
		ptg = factory.buildPTG(states, (states-1)*(states-1)/2, resets , 50, 50, 10);
		solver = new PTGSolver();

		struct timeval start, end;
		gettimeofday(&start, NULL);
		solver->solvePTG(ptg,false,1,false);
		gettimeofday(&end, NULL);
		count += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
		delete solver;
		delete ptg;
		}
	 f << resets << ":" << count/nbTests << endl;
	}
	f.close();
}

void PerfEvaluator::timeConsts(){
	cerr << "timeConsts" << endl;

	PTGFactory factory;
	PTG* ptg;
	PTGSolver* solver;
	unsigned int states = 50;

	ofstream f;
	f.open("results/timeConsts.txt");
	f << "(states, (states-1)*(states-1)/2, 0, 50, 50, consts)" << endl;

	for (int consts = 0; consts < 100; consts += 5){
		double count = 0;
	for (unsigned int i = 0; i < nbTests; ++i){
		ptg = factory.buildPTG(states, (states-1)*(states-1)/2, 0 , 50, 50, consts);
		solver = new PTGSolver();

		struct timeval start, end;
		gettimeofday(&start, NULL);
		solver->solvePTG(ptg,false,1,false);
		gettimeofday(&end, NULL);
		count += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
		delete solver;
		delete ptg;
		}
	 f << consts << ":" << count/nbTests << endl;
	}
	f.close();
}

void PerfEvaluator::timeWeights(){
	cerr << "timeWeights" << endl;

	PTGFactory factory;
	PTG* ptg;
	PTGSolver* solver;
	unsigned int states = 50;

	ofstream f;
	f.open("results/timeWeights.txt");
	f << "(states, (states-1)*(states-1)/2, 0, 50, costs, 10)" << endl;

	for (int costs = 20; costs < 500; costs += 20){
		double count = 0;
	for (unsigned int i = 0; i < nbTests; ++i){
		ptg = factory.buildPTG(states, (states-1)*(states-1)/2, 0 , 50, costs, 10);
		solver = new PTGSolver();

		struct timeval start, end;
		gettimeofday(&start, NULL);
		solver->solvePTG(ptg,false,1,false);
		gettimeofday(&end, NULL);
		count += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
		delete solver;
		delete ptg;
		}
	 f << costs << ":" << count/nbTests << endl;
	}
	f.close();
}

void PerfEvaluator::timeRates(){
	cerr << "timeRates" << endl;

	PTGFactory factory;
	PTG* ptg;
	PTGSolver* solver;
	unsigned int states = 50;

	ofstream f;
	f.open("results/timeRates.txt");
	f << "(states, (states-1)*(states-1)/2, 0, rates, 50, 10)" << endl;

	for (int rates = 20; rates < 500; rates += 20){
		double count = 0;
	for (unsigned int i = 0; i < nbTests; ++i){
		ptg = factory.buildPTG(states, (states-1)*(states-1)/2, 0 , rates, 50, 10);
		solver = new PTGSolver();

		struct timeval start, end;
		gettimeofday(&start, NULL);
		solver->solvePTG(ptg,false,1,false);
		gettimeofday(&end, NULL);
		count += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
		delete solver;
		delete ptg;
		}
	 f << rates << ":" << count/nbTests << endl;
	}
	f.close();
}

