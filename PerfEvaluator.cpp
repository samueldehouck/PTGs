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
	nbTests = 500;
}

PerfEvaluator::PerfEvaluator(unsigned int nbT){
	nbTests = nbT;
}

void PerfEvaluator::eval(bool v2){
	cerr << "====Starting computing data===" << endl;
	cerr << "evalStatesTrans" << endl;
	evalStatesTrans(v2);
	cerr << "evalResets" << endl;
	//evalResets(v2);
	cerr << "evalStates" << endl;
	//evalStates(v2);
	cerr << "evalTrans" << endl;
	//evalTrans(v2);
	cerr << "evalInterval" << endl;
	//evalInterval(v2);
	cerr << "evalbigone" << endl;
	//evalBig(v2);
	cerr << "evalBreakPoints" << endl;
	//evalBreakPoints(v2);
}

void PerfEvaluator::evalStatesTrans(bool v2){
	unsigned int minState = 5;
	unsigned int maxState = 20;
	struct timeval start, end;
	ofstream f, o;

	//Compute a tab while modifying the number of states and transitions
	f.open("tabStateTrans.tex");
	f << "\\documentclass{standalone}" << endl;
	f << "\\begin{document}" << endl;
	f << "Intervals: [0,3]" << endl;
	f << "Resets: 0" << endl;
	f << "Max cost: 5" << endl;
	f << "Max rate: 5" << endl;
	f << "\\begin{tabular}{|c|";

	o.open("tabStateTransBreak.tex");
	o << "\\documentclass{standalone}" << endl;
	o << "\\begin{document}" << endl;
	o << "Intervals: [0,3]" << endl;
	o << "Resets: 0" << endl;
	o << "Max cost: 5" << endl;
	o << "Max rate: 5" << endl;
	o << "\\begin{tabular}{|c|";

	for (unsigned int i = minState - 1; i <= (maxState - 1)*(maxState - 1); ++i){
		f << "c|";
		o << "c|";
	}
	f << "}" << endl;
	o << "}" << endl;
	f << "\\hline" << endl;
	o << "\\hline" << endl;
	f << "States/Transitions &";
	o << "States/Transitions &";

	for (unsigned int i = minState - 1; i <= (maxState - 1)*(maxState - 1); ++i){
		if(i < (maxState - 1)*(maxState - 1)){
			f << i << " & ";
			o << i << " & ";
		}
		else{
			f << i << " \\\\" << endl;
			o << i << " \\\\" << endl;
		}
	}
	for (unsigned int nbStates = minState; nbStates <= maxState; ++nbStates){
		f << "\\hline" << endl;
		f << nbStates << " & ";
		o << "\\hline" << endl;
		o << nbStates << " & ";
		for (unsigned int nbTrans = minState - 1; nbTrans <= (maxState - 1)*(maxState - 1); ++nbTrans){
			cerr << "states: " << nbStates << " trans: " << nbTrans << endl;
			if (nbTrans <= (nbStates - 2)){
				if(nbTrans > minState - 1){
					f << " & ";
					o << " & ";
				}
			}
			else if( nbTrans >= (nbStates - 1) && nbTrans <= (nbStates - 1)*(nbStates - 1)){
				unsigned int average = 0;
				double averageBreaks = 0;
				for (unsigned int i = 0; i < nbTests; ++i){
					PTGFactory factory;
					PTG* ptg = factory.buildSPTG(nbStates,nbTrans,5,5);
					//PTG* ptg = factory.buildPTG(nbStates,nbTrans,0,5,5,3);
					PTGSolver solver;
					gettimeofday(&start, NULL);


					solver.solvePTG(ptg, false,v2);

					gettimeofday(&end, NULL);
					average += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
					averageBreaks += solver.getBreakPoints();
					double tmp = 0;
					for(unsigned int state = 0; state < ptg->getSize(); ++state){
						if(solver.getValueFcts()->at(state).front().getY().isInfinity())
							++tmp;
					}
					usleep(10000);
					delete ptg;

				}
				cerr << average << endl;
				average /= nbTests;
				averageBreaks /= double(nbTests);
				if(nbTrans > minState - 1){
					f << " & ";
					o << " & ";
				}
				f << average;
				o << averageBreaks;
			}
			else if (nbTrans > (nbStates - 1)*(nbStates -1) && nbTrans <= (maxState - 1)*(maxState - 1) ){
				f << " & ";
				o << " & ";
			}
		}
		f << "\\\\" << endl;
		o << "\\\\" << endl;
	}
	f << "\\hline" << endl;
	f << "\\end{tabular}" << endl;
	f << "\\end{document}" << endl;

	o << "\\hline" << endl;
	o << "\\end{tabular}" << endl;
	o << "\\end{document}" << endl;
	system("pdflatex tabStateTrans.tex");
	system("pdflatex tabStateTransBreak.tex");
	f.close();
}

void PerfEvaluator::evalResets(bool v2){
	struct timeval start, end;

	unsigned int maxNbResets = 25;
	unsigned int step = 2;
	unsigned int nbStates = 20;
	unsigned int nbTrans = (nbStates - 1)*(nbStates - 1)/2;
	double scaleX = 2;
	double scaleY = 200;

	ofstream f;
	f.open("resetsGraph.tex");
	f << "\\documentclass{standalone}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;
	f << "\\begin{tikzpicture}" << endl;

	//Draw the x axis
	f << "\\draw [->][thick] (0,0) -- (" << double(maxNbResets)/scaleX << ",0);" << endl;
	f << "\\draw ( " << double(maxNbResets)/scaleX << ",0) node [right] {$r$};" << endl;
	//Draw the y axis
	f << "\\draw [->][thick] (0,0) -- (0," << double(1200)/ scaleY  << ");" << endl;
	f<< "\\draw (0," << double(1200)/ scaleY   <<") node [above] {$ms$}; " << endl;
	for (unsigned int i = 0; i <= 1200; i += 100)
		f<< "\\draw (0," << double(i)/ scaleY   <<") node [left] {$"<< i <<"$}; " << endl;


	for (unsigned int nbResets = 0; nbResets <= maxNbResets; nbResets = nbResets + step){
		unsigned int average = 0;
		for (unsigned int i = 0; i < nbTests; ++i){
			cerr << ".";

			PTGFactory factory;
			PTG* ptg = factory.buildPTG(nbStates,nbTrans,nbResets,5,5,3);
			PTGSolver solver;
			gettimeofday(&start, NULL);

			solver.solvePTG(ptg, false,v2);

			delete ptg;
			gettimeofday(&end, NULL);
			average += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
			usleep(10000);
		}
		average /= nbTests;
		cerr << endl << average << endl;

		f << "\\draw ("<< double(nbResets)/scaleX <<"," << double(average)/scaleY << ") node {$\\bullet$};" << endl;
		//f << "\\draw (0," << double(average)/scaleY << ") node [left] {$" << average << "$};" << endl;
		f << "\\draw (" << double(nbResets)/scaleX << ",0) node [below] {$" << nbResets << "$};" << endl;

	}

	f << "\\end{tikzpicture}" << endl;
	f << "\\end{document}" << endl;
	f.close();
	system("pdflatex resetsGraph.tex");
}


void PerfEvaluator::evalStates(bool v2){
	struct timeval start, end;

	unsigned int step = 10;
	unsigned int maxNbStates = 100;
	double scaleX = 2;
	double scaleY = 200;

	ofstream f;
	f.open("states.tex");
	f << "\\documentclass{standalone}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;


	f << "\\begin{tikzpicture}" << endl;

	//Draw the x axis
	f << "\\draw [->][thick] (0,0) -- (" << double(maxNbStates)/scaleX << ",0);" << endl;
	f << "\\draw ( " << double(maxNbStates)/scaleX << ",0) node [right] {$states$};" << endl;
	//Draw the y axis
	f << "\\draw [->][thick] (0,0) -- (0," << double(12000)/ scaleY  << ");" << endl;
	f<< "\\draw (0," << double(12000)/ scaleY   <<") node [above] {$ms$}; " << endl;


	for (unsigned int states = 10; states <= maxNbStates; states += step){
		unsigned int average = 0;
		for (unsigned int i = 0; i < nbTests; ++i){
			cerr << ".";
			PTGFactory factory;
			PTG* ptg = factory.buildPTG(states,(states - 1)*(states - 1)/2,0,5,5,3);
			PTGSolver solver;
			gettimeofday(&start, NULL);

			solver.solvePTG(ptg, false, v2);

			gettimeofday(&end, NULL);
			delete ptg;

			average += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
			usleep(10000);
		}
		average /= nbTests;
		cerr << average << endl;

		f << "\\draw ("<< double(states)/scaleX <<"," << double(average)/scaleY << ") node {$\\bullet$};" << endl;
		f << "\\draw (0," << double(average)/scaleY << ") node [left] {$" << average << "$};" << endl;
		f << "\\draw (" << double(states)/scaleX << ",0) node [below] {$" << states << "$};" << endl;
	}


	f << "\\end{tikzpicture}" << endl;
	f << "\\end{document}" << endl;
	f.close();
	system("pdflatex states.tex");


	maxNbStates = 200;
	f.open("statesfixed.tex");
	f << "\\documentclass{standalone}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;


	f << "\\begin{tikzpicture}" << endl;

	//Draw the x axis
	f << "\\draw [->][thick] (0,0) -- (" << double(maxNbStates)/scaleX << ",0);" << endl;
	f << "\\draw ( " << double(maxNbStates)/scaleX << ",0) node [right] {$states$};" << endl;
	//Draw the y axis
	f << "\\draw [->][thick] (0,0) -- (0," << double(12000)/ scaleY  << ");" << endl;
	f<< "\\draw (0," << double(12000)/ scaleY   <<") node [above] {$ms$}; " << endl;
	for (unsigned int states = 10; states <= maxNbStates; states += step){
		unsigned int average = 0;
		for (unsigned int i = 0; i < nbTests; ++i){
			cerr << ".";
			PTGFactory factory;
			PTG* ptg = factory.buildPTG(states,80,0,5,5,3);
			PTGSolver solver;
			gettimeofday(&start, NULL);

			solver.solvePTG(ptg, false, v2);

			gettimeofday(&end, NULL);
			delete ptg;

			average += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
			usleep(100000);
		}
		average /= nbTests;
		cerr << average << endl;

		f << "\\draw ("<< double(states)/scaleX <<"," << double(average)/scaleY << ") node {$\\bullet$};" << endl;
		f << "\\draw (0," << double(average)/scaleY << ") node [left] {$" << average << "$};" << endl;
		f << "\\draw (" << double(states)/scaleX << ",0) node [below] {$" << states << "$};" << endl;
	}


	f << "\\end{tikzpicture}" << endl;
	f << "\\end{document}" << endl;
	f.close();
	system("pdflatex statesfixed.tex");
}

void PerfEvaluator::evalTrans(bool v2){
	struct timeval start, end;

	unsigned int step = 20;
	unsigned int nbStates = 30;
	double scaleX = 20;
	double scaleY = 50;

	ofstream f;
	f.open("transitions.tex");
	f << "\\documentclass{standalone}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;
	f << "\\begin{tikzpicture}" << endl;

	//Draw the x axis
	f << "\\draw [->][thick] (0,0) -- (" << double((nbStates -1)*(nbStates -1))/scaleX << ",0);" << endl;
	f << "\\draw ( " << double((nbStates -1)*(nbStates -1))/scaleX + 0.5 << ",0) node [right] {$transitions$};" << endl;
	//Draw the y axis
	f << "\\draw [->][thick] (0,0) -- (0," << double(600)/ scaleY  << ");" << endl;
	f<< "\\draw (0," << double(600)/ scaleY   <<") node [above] {$ms$}; " << endl;
	for (unsigned int i = 0; i <= 600; i += 50)
		f << "\\draw (0," << double(i)/scaleY << ") node [left] {$" << i << "$};" << endl;


	for (unsigned int  trans = nbStates ; trans <= (nbStates -1)*(nbStates -1); trans += step){
		unsigned int average = 0;
		for (unsigned int i = 0; i < nbTests; ++i){
			cerr << ".";

			PTGFactory factory;
			PTG* ptg = factory.buildPTG(nbStates,trans,0,5,5,3);
			PTGSolver solver;
			gettimeofday(&start, NULL);

			solver.solvePTG(ptg, false, v2);

			delete ptg;
			gettimeofday(&end, NULL);
			average += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
			usleep(100000);
		}
		average /= nbTests;
		cerr << trans << ": " << average << endl;

		f << "\\draw ("<< double(trans)/scaleX <<"," << double(average)/scaleY << ") node {$\\bullet$};" << endl;
		//f << "\\draw (0," << double(average)/scaleY << ") node [left] {$" << average << "$};" << endl;
		f << "\\draw (" << double(trans)/scaleX << ",0) node [below] {$" << trans << "$};" << endl;
	}


	f << "\\end{tikzpicture}" << endl;
	f << "\\end{document}" << endl;
	f.close();
	system("pdflatex transitions.tex");
}

void PerfEvaluator::evalInterval(bool v2){
	struct timeval start, end;

	unsigned int step = 5;
	unsigned int nbStates = 15;
	unsigned int nbTrans = (nbStates - 1)*(nbStates - 1)/2;
	unsigned int maxEndCst = 50;
	double scaleX = 2;
	double scaleY = 10;

	ofstream f;
	f.open("intervals.tex");
	f << "\\documentclass{standalone}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;
	f << "\\begin{tikzpicture}" << endl;

	//Draw the x axis
	f << "\\draw [->][thick] (0,0) -- (" << double((nbStates -1)*(nbStates -1))/scaleX << ",0);" << endl;
	f << "\\draw ( " << double(maxEndCst)/scaleX << ",0) node [right] {$transitions$};" << endl;
	//Draw the y axis
	f << "\\draw [->][thick] (0,0) -- (0," << double(250)/ scaleY  << ");" << endl;
	f<< "\\draw (0," << double(250)/ scaleY   <<") node [above] {$ms$}; " << endl;


	for (unsigned int  endCst = 1 ; endCst <= maxEndCst; endCst += step){
		unsigned int average = 0;
		for (unsigned int i = 0; i < nbTests; ++i){
			cerr << ".";

			PTGFactory factory;
			PTG* ptg = factory.buildPTG(nbStates,nbTrans,0,5,5,endCst);
			PTGSolver solver;
			gettimeofday(&start, NULL);

			solver.solvePTG(ptg, false, v2);

			delete ptg;
			gettimeofday(&end, NULL);
			average += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
			usleep(10000);
		}
		average /= nbTests;
		cerr << endl << average << endl;
		f << "\\draw ("<< double(endCst)/scaleX <<"," << double(average)/scaleY << ") node {$\\bullet$};" << endl;
		f << "\\draw (0," << double(average)/scaleY << ") node [left] {$" << average << "$};" << endl;
		f << "\\draw (" << double(endCst)/scaleX << ",0) node [below] {$" << endCst << "$};" << endl;
	}


	f << "\\end{tikzpicture}" << endl;
	f << "\\end{document}" << endl;
	f.close();
	system("pdflatex intervals.tex");
}

void PerfEvaluator::evalBig(bool v2){
	struct timeval start, end;
	unsigned int average = 0;
	for (unsigned int i = 0; i < 20; ++i){
		cerr << ".";
		PTGFactory factory;
		PTG* ptg = factory.buildPTG(5,15,0,5,5,3);
		PTGSolver solver;
		gettimeofday(&start, NULL);

		solver.solvePTG(ptg, true, v2);

		delete ptg;
		gettimeofday(&end, NULL);
		average += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
		usleep(200000);
	}
	average /= nbTests;
	cerr << endl << "Average: " << average << "ms" << endl;
}


void PerfEvaluator::evalBreakPoints(bool v2){

 	unsigned int maxNbStates = 50;
	double scaleX = 2;
	double scaleY = 5;

	ofstream f;
	f.open("breakpoints.tex");
	f << "\\documentclass{standalone}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;


	f << "\\begin{tikzpicture}" << endl;

	//Draw the x axis
	f << "\\draw [->][thick] (0,0) -- (" << double(maxNbStates)/scaleX << ",0);" << endl;
	f << "\\draw ( " << double(maxNbStates)/scaleX << ",0) node [right] {$states$};" << endl;
	//Draw the y axis
	f << "\\draw [->][thick] (0,0) -- (0," << double(12000)/ scaleY  << ");" << endl;
	f<< "\\draw (0," << double(12000)/ scaleY   <<") node [above] {$ms$}; " << endl;
	double average = 0;

	unsigned int states = 5;
	for ( int i = 0; i <= 10000; ++i){
		//for (unsigned int i = 0; i < nbTests; ++i){
		if(i%10 == 0)
			cerr << ".";
		PTGFactory factory;
		PTG* ptg = factory.buildSPTG(4,6,5,5);
		PTGSolver solver;

		solver.solvePTG(ptg, false, v2);
		unsigned int nbBreakPoints = solver.getBreakPoints();
		if(nbBreakPoints > 0)
			++average;
		//cerr << nbBreakPoints;

		if(nbBreakPoints >= 2){
			ostringstream tmp;
			//tmp << "sptg" << i;

			ptg->write(tmp.str());
		}

		delete ptg;

		//}


		f << "\\draw ("<< double(states)/scaleX <<"," << double(average)/scaleY << ") node {$\\bullet$};" << endl;
		f << "\\draw (0," << double(average)/scaleY << ") node [left] {$" << average << "$};" << endl;
		f << "\\draw (" << double(states)/scaleX << ",0) node [below] {$" << states << "$};" << endl;
	}
	cerr << average << endl;
	average /= (double)100000;
	cerr << endl << average << endl;
	f << "\\end{tikzpicture}" << endl;
	f << "\\end{document}" << endl;
	f.close();
	//system("pdflatex breakpoints.tex");
}
