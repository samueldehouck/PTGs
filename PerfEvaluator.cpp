#include "PerfEvaluator.hpp"
#include "PTGFactory.hpp"
#include "PTG.hpp"
#include "PTGSolver.hpp"
#include <sys/time.h>
#include <iostream>
#include <fstream>
#include <unistd.h>

using namespace std;

void PerfEvaluator::eval(){
	cerr << "====Starting computing data===" << endl;
	//evalStatesTrans();
	evalResets();
}

void PerfEvaluator::evalStatesTrans(){
	unsigned int minState = 5;
	unsigned int maxState = 15;
	unsigned int nbTests = 20;
	struct timeval start, end;
	ofstream f, o;

	//Compute a tab while modifying the number of states and transitions
	f.open("tabStateTrans.tex");
	f << "\\documentclass{standalone}" << endl;
	f << "\\begin{document}" << endl;
	f << "\\begin{tabular}{|c|";

	o.open("tabStateTransInf.tex");
	o << "\\documentclass{standalone}" << endl;
	o << "\\begin{document}" << endl;
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
				double averageInf = 0;
				for (unsigned int i = 0; i < nbTests; ++i){
					PTGFactory factory;
					PTG* ptg = factory.build(nbStates,nbTrans,0,5,5,3);
					PTGSolver solver;
					gettimeofday(&start, NULL);

					solver.solvePTG(ptg, false);

					delete ptg;
					gettimeofday(&end, NULL);
					average += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
					double tmp = 0;
					for(unsigned int state = 0; i < ptg->getSize(); ++i){
						if(solver.getValueFcts()->at(i).front().getY().isInfinity())
							++tmp;
					}
					tmp = tmp/double(nbStates);
					averageInf += tmp;
					//usleep(300000);

				}
				cerr << average << endl;
				average /= nbTests;
				averageInf /= double(nbTests);
				if(nbTrans > minState - 1){
					f << " & ";
					o << " & ";
				}
				f << average;
				o << averageInf;
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
	//system("pdflatex tabstatetrans.tex");
	f.close();
}

void PerfEvaluator::evalResets(){
	struct timeval start, end;

	unsigned int maxNbResets = 20;
	unsigned int step = 2;
	unsigned int nbStates = 20;
	unsigned int nbTrans = 100;
	unsigned int nbTests = 20;
	Fraction scaleX = 4;
	Fraction scaleY = 80;

	ofstream f;
	f.open("resetsGraph.tex");
	f << "\\documentclass{standalone}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;
	f << "\\begin{tikzpicture}" << endl;

	//Draw the x axis
		f << "\\draw [->][thick] (0,0) -- ( " << Fraction(maxNbResets)/scaleX << ",0);" << endl;
		f << "\\draw ( " << Fraction(maxNbResets)/scaleX << ",0) node[right] {$r$};" << endl;
		//Draw the y axis
		f << "\\draw [->][thick] (0,0) -- (0," << Fraction(400)/ scaleY  << ");" << endl;
		f<< "\\draw (0," << Fraction(400)/ scaleY   <<") node[above] {$ms$}; " << endl;


	for (unsigned int nbResets = 0; nbResets <= maxNbResets; nbResets = nbResets + step){
		unsigned int average = 0;
		for (unsigned int i = 0; i < nbTests; ++i){
			PTGFactory factory;
			PTG* ptg = factory.build(nbStates,nbTrans,nbResets,5,5,3);
			PTGSolver solver;
			gettimeofday(&start, NULL);

			solver.solvePTG(ptg, false);

			delete ptg;
			gettimeofday(&end, NULL);
			average += 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec)/1000;
			//usleep(300000);
			cerr << average << endl;
		}
		average /= nbTests;

		f << "\\draw ("<< Fraction(nbResets)/scaleX <<"," << Fraction(average)/scaleY << ") node {} node{$\bullet$};" << endl;
		f << "\\draw (0," << Fraction(average)/scaleY << "node [left] {$" << average << "$};" << endl;
		f << "\\draw (" << Fraction(nbResets)/scaleX << ",0) node[below] {$" << nbResets << "$}" << endl;

	}

	f << "\\end{tikzpicture}" << endl;
	f << "\\end{document}" << endl;
	f.close();
	//system("pdflatex resetsGraph.tex");
}
