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
	//    struct timeval start, end;
	//
	//     long seconds, milliseconds, allsecs = 0, allmillisecs = 0;
	//      ofstream file;
	//      file.open ("results.txt");

	//    double nbr = 1000;
	//    for (int i = 0; i < nbr; ++i){
	//    gettimeofday(&start, NULL);
	/* SPTG sptg;
	       sptg.show();
	      SPTGSolver solver(&sptg);
	      solver.solveSPTG();*/

	for (unsigned int i = 0; i < 1; ++i){
		//freopen("output.txt","w",stdout);
		PTGFactory factory;
		PTG* ptg = factory.hardBuild(5);
		ptg->show();
		PTGSolver solver;
		if(argc > 1 && strcmp(argv[1], "-v") == 0){
			solver.solvePTG(ptg,true);
		}
		else{
			solver.solvePTG(ptg, false);

		}
		cerr << ".";
	}
	cerr << endl;
	//     gettimeofday(&end, NULL);
	//
	//     seconds  = end.tv_sec  - start.tv_sec;
	//     milliseconds = (end.tv_usec - start.tv_usec)/1000;
	//     allsecs += seconds;
	//     allmillisecs += milliseconds;
	//     cout << seconds <<"s" << milliseconds << "ms" << endl;
	//    }
	//    file << "Average: " << allsecs/nbr <<"s" << allmillisecs/nbr << "ms" << endl;

	return 1;
}
