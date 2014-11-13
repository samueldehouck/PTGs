/*
 * Author: Samuel Dehouck
 */


#include "PTGVisu.hpp"
#include <iostream>
#include <fstream>
#include "stdlib.h"
#include "Fraction.hpp"


using namespace std;

PTGVisu::PTGVisu(){

}

PTGVisu::~PTGVisu(){

}

void PTGVisu::visualizeVals(PTG* ptg, vector<list<Point> >* valueFcts){

	ofstream f;

	unsigned int size = ptg->getSize();

	double length = 5;
	double high = 5;
	f.open("valueFcts.tex");
	f << "\\documentclass{article}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;

	double max = 0;

	//Get the max value to determine the scale
	for (unsigned int i = 0; i < size; ++i){
		for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end(); ++it){
			if(!it->getY().isInfinity() && it->getY() > max)
				max = it->getY().getVal().asDouble();
		}
	}


	double scale = high/max;

	for (unsigned int i = 1; i < size; ++i){
		f << "\\begin{tikzpicture}" << endl;

		double x = 0;

		//Draw the x axis
		f << "\\draw [->][thick] (0,0) -- ( " << x + length + 1 << ",0);" << endl;
		f << "\\draw ( " << x + length + 1 << ",0) node[right] {$t$};" << endl;
		f << "\\draw (0,0) node [below]{\\footnotesize$0$};" << endl;
		//Draw the y axis
		f << "\\draw [->][thick] (0,0) -- (0," << length + 1 << ");" << endl;
		if(ptg->hasLabels())
			f<< "\\draw (0," << length + 1<<") node[above] {$\\sigma_{" <<  ptg->getLabel(i) << "}(t)$}; " << endl;
		else
			f<< "\\draw (0," << length + 1<<") node[above] {$\\sigma_{" <<  i << "}(t)$}; " << endl;

		cerr << scale << endl;

		list<Point>::iterator it = (*valueFcts)[i].begin();
		list<Point>::iterator itNext = it;
		++itNext;

		list<Fraction> indexes;
		bool inf = false;
		while(itNext != (*valueFcts)[i].end() && it != (*valueFcts)[i].end()){


			cerr << "loop 1" << endl;
			while(itNext != (*valueFcts)[i].end() && itNext->getX() != 1){
				if(!it->getY().isInfinity()){
				f << "\\draw (" << it->getX().getVal().asDouble()*length + x* length + x << ", " << it->getY().getVal().asDouble()*scale <<") -- (" << itNext->getX().getVal().asDouble()*length + x* length + x << "," << itNext->getY().getVal().asDouble()*scale << ");" << endl;
				f<< "\\draw (" << itNext->getX().getVal().asDouble()*length + x* length + x <<",0) node[below] {$ " << itNext->getX().getVal()<< "$}; " << endl;


				indexes.push_back(it->getY().getVal());
				}

				else{
					inf = true;
				}

				it = itNext;
				++itNext;
			}
			//Need to trace the last segment

			if(!it->getY().isInfinity()){
			f << "\\draw (" << it->getX().getVal().asDouble()*length + x* length + x << ", " << it->getY().getVal().asDouble()*scale <<") -- (" << itNext->getX().getVal().asDouble()*length + x* length + x << "," << itNext->getY().getVal().asDouble()*scale << ");" << endl;
			f<< "\\draw (" << itNext->getX().getVal().asDouble()*length + x* length + x <<",0) node[below] {$ " << itNext->getX().getVal()<< "$}; " << endl;

			indexes.push_back(it->getY().getVal());
			indexes.push_back(itNext->getY().getVal());


			}

			else{
				inf = true;
			}

			//Place the iterators at the start of the next reset interval
			++itNext;
			++itNext;
			it = itNext;
			++itNext;


			++x;

		}

		if (inf){

		}

		indexes.unique();

		for (list<Fraction>::iterator itY = indexes.begin(); itY != indexes.end(); ++itY){
			f << "\\draw (0," <<  itY->asDouble() * scale << ") node[left] {$" << (*itY) << "$};" << endl;
		}


		f << "\\end{tikzpicture}" << endl;
	}

	f << "\\end{document}" << endl;
	f.close();
	system("pdflatex valueFcts.tex");
}

void PTGVisu::visualizeStrats(PTG* ptg, vector<list<Point> >* valueFcts){
	ofstream f;
	Fraction y = 0;
	const int length = 3;

	unsigned int size = ptg->getSize();

	f.open("strategies.tex");
	f << "\\documentclass{article}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;
	f << "\\begin{tikzpicture}" << endl;

	y = 0;

	for(unsigned int i = 1; i < size; ++i){
		cout << "State " << i << endl;
		Fraction x = 0;
		Fraction maxX = (*valueFcts)[i].back().getX().getVal();
		unsigned int maxY = size;
		for (list<Point>::iterator it = (*valueFcts)[i].begin(); it != (*valueFcts)[i].end() ; ++it){

			if(it->getX().getVal() == 0 && it->getInclusion() && it != (*valueFcts)[i].begin()){
				x = x + length + 1;
			}
			string color;
			if(it->getType() == 0)
				color = "black";
			else if(it->getType() == 1)
				color = "blue";
			else if(it->getType() == 2)
				color = "green";
			else if(it->getType() == 3)
				color = "red";

			list<Point>::iterator itNext = it;
			++itNext;

			string colorNext;
			if(itNext != (*valueFcts)[i].end() && itNext->getType() == 0)
				colorNext = "black";
			else if(itNext != (*valueFcts)[i].end() && itNext->getType() == 1)
				colorNext = "blue";
			else if(itNext != (*valueFcts)[i].end() && itNext->getType() == 2)
				colorNext = "green";
			else if(itNext != (*valueFcts)[i].end() && itNext->getType() == 3)
				colorNext = "red";


			//Draw the line
			if(itNext != (*valueFcts)[i].end() && itNext->getX().getVal() != 0){
				f << "\\draw [" << color << "] (" << (it->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << ((Fraction(it->getDest())/ Fraction(maxY) * Fraction(length))  - y + 1).asDouble()<< ") -- (" << (itNext->getX().getVal()/maxX  * Fraction(length) + x).asDouble()<< "," << (Fraction(it->getDest()) / Fraction(maxY) * Fraction(length) - y + 1).asDouble()<< ");" << endl;
				f << "\\draw (" << (it->getX().getVal() / maxX * Fraction(length) + x).asDouble()<< "," << Fraction (0) - y <<") node[below] {\\footnotesize$" << it->getX().getVal() << "$};" << endl;
				if(ptg->hasLabels())
					f << "\\draw (0,"  << ((Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)) - y + 1).asDouble() << ") node[left] {\\footnotesize$" << ptg->getLabel(it->getDest()) << "$};" << endl;
				else
					f << "\\draw (0,"  << ((Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)) - y + 1).asDouble() << ") node[left] {\\footnotesize$" << it->getDest() << "$};" << endl;
			}

			if(it->getInclusion() && itNext->getX().getVal() != maxX && ((itNext != (*valueFcts)[i].end() && itNext->getDest() != it->getDest()) || it->getX().getVal() == 0 || it->getX().getVal() == maxX)){
				if(!itNext->getInclusion() && itNext != (*valueFcts)[i].end() && itNext->getDest() != it->getDest() && it->getX().getVal() != 0 && it->getX().getVal() != maxX){
					f << "\\node [circle,draw,fill= " << color <<",scale=0.4] at (" << (it->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)  - y + 1 << ") {};" << endl;

					f << "\\draw[" << colorNext << ",fill=white] (" << (itNext->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)  - y + 1 << ") circle (0.07);" << endl;
					if(ptg->hasLabels())
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) - y + 1).asDouble() << ") node[left] {\\footnotesize$" << ptg->getLabel(itNext->getDest()) << "$};" << endl;
					else
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) - y + 1).asDouble() << ") node[left] {\\footnotesize$" << itNext->getDest() << "$};" << endl;

				}
				else if(itNext->getInclusion() && itNext != (*valueFcts)[i].end() && itNext->getDest() != it->getDest() && it->getX().getVal() != 0 && it->getX().getVal() != maxX && itNext->getX().getVal() != maxX){
					f << "\\draw[" << color << ",fill=white] (" << (itNext->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)  - y + 1 << ") circle (0.07);" << endl;

					f << "\\node [circle,draw,fill="<< colorNext <<",scale=0.4] at (" << (itNext->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)  - y + 1 << ") {};" << endl;

					if(ptg->hasLabels())
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) - y + 1).asDouble() << ") node[left] {\\footnotesize$" << ptg->getLabel(itNext->getDest()) << "$};" << endl;
					else
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) - y + 1).asDouble() << ") node[left] {\\footnotesize$" << itNext->getDest() << "$};" << endl;

				}
				else{
					f << "\\node [circle,draw,fill= " << color <<",scale=0.4] at (" << (it->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)  - y + 1 << ") {};" << endl;

				}

			}

			else if(!it->getInclusion() && itNext != (*valueFcts)[i].end() && itNext->getDest() != it->getDest()){
				f << "\\draw[" << color << ",fill=white] (" << (itNext->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)  - y + 1 << ") circle (0.07);" << endl;
				cout << "time: " << itNext->getX().getVal() << endl;
				if(itNext != (*valueFcts)[i].end() && itNext->getDest() != it->getDest()){
					if(ptg->hasLabels())
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) - y + 1).asDouble() << ") node[left] {\\footnotesize$" << ptg->getLabel(itNext->getDest()) << "$};" << endl;
					else
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) - y + 1).asDouble() << ") node[left] {\\footnotesize$" << itNext->getDest() << "$};" << endl;
					f << "\\draw[fill=" << colorNext << "] (" << (itNext->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)  - y + 1 << ") circle (0.07);" << endl;
				}
			}
			else if(it->getInclusion() && it->getX().getVal() == 0){
				f << "\\draw[fill=" << color << "] (" << (it->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)  - y + 1 << ") circle (0.07);" << endl;
				if(ptg->hasLabels())
					f << "\\draw (0,"  << ((Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)) - y + 1).asDouble() << ") node[left] {\\footnotesize$" << ptg->getLabel(it->getDest()) << "$};" << endl;
				else
					f << "\\draw (0,"  << ((Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)) - y + 1).asDouble() << ") node[left] {\\footnotesize$" << it->getDest() << "$};" << endl;

			}


		}

		//Draw the x axis
		f << "\\draw [->][thick] (0," << (Fraction(0) - y).asDouble() << ") -- ( " << (x + length + 1).asDouble() << "," << (Fraction(0) - y).asDouble() << ");" << endl;
		f << "\\draw ( " << x + length + 1 << "," << (Fraction(0) - y).asDouble() << ") node[right] {$t$};" << endl;
		f << "\\draw (0," << Fraction(0) - y <<") node [below]{\\footnotesize$0$};" << endl;
		//Draw the y axis
		f << "\\draw [->][thick] (0," << (Fraction(0) - y).asDouble() << ") -- (0," << (Fraction(length + 1) - y).asDouble()  << ");" << endl;
		if(ptg->hasLabels())
			f<< "\\draw (0," << (Fraction(length + 1) - y).asDouble()<<") node[above] {$\\sigma_{" <<  ptg->getLabel(i) << "}(t)$}; " << endl;
		else
			f<< "\\draw (0," << (Fraction(length + 1) - y).asDouble()<<") node[above] {$\\sigma_{" <<  i << "}(t)$}; " << endl;

		y = y + 5;
	}

	f << "\\draw[black, fill=black] (0," << (Fraction(3) - y).asDouble() << ") circle (0.07);" << endl;
	f << "\\node [right] at (0.5, " << (Fraction(3) - y).asDouble() << ") {Normal transition};" << endl;
	f << "\\draw[black, fill=blue] (0," << (Fraction(2) - y).asDouble() << ") circle (0.07);" << endl;
	f << "\\node [right] at (0.5, " << (Fraction(2) - y).asDouble() << ") {Waiting (lambda transition)};" << endl;
	f << "\\draw[black, fill=green] (0," << (Fraction(1) - y).asDouble() << ") circle (0.07);" << endl;
	f << "\\node [right] at (0.5, " << (Fraction(1) - y).asDouble() << ") {Waiting (bottom transition)};" << endl;
	f << "\\draw[black, fill=red] (0," << (Fraction(0) - y).asDouble() << ") circle (0.07);" << endl;
	f << "\\node [right] at (0.5, " << (Fraction(0) - y).asDouble() << ") {Reset};" << endl;

	f << "\\end{tikzpicture}" << endl;
	f << "\\end{document}" << endl;
	system("pdflatex strategies.tex");
}


