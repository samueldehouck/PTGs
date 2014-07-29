#include <iostream>
#include "SPTG.hpp"
#include "SPTGSolver.hpp"
#include "PTG.hpp"
#include "Fraction.hpp"
#include <sys/time.h>
#include <fstream>

using namespace std;

int main(){
   struct timeval start, end;

    long seconds, milliseconds, allsecs = 0, allmillisecs = 0;    
     ofstream file;
     file.open ("results.txt");
    
//    double nbr = 1000;
//    for (int i = 0; i < nbr; ++i){
   gettimeofday(&start, NULL);
//    SPTG sptg;
// //    sptg.show();
//    SPTGSolver solver;
//    solver.solveSPTG(&sptg);
   PTG ptg;
   ptg.show();
   PTGSolver solver;
   
    gettimeofday(&end, NULL);

    seconds  = end.tv_sec  - start.tv_sec;
    milliseconds = (end.tv_usec - start.tv_usec)/1000;
    allsecs += seconds;
    allmillisecs += milliseconds;
    cout << seconds <<"s" << milliseconds << "ms" << endl;
//    }
//    file << "Average: " << allsecs/nbr <<"s" << allmillisecs/nbr << "ms" << endl;
  return 1;
}