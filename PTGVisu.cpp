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

	unsigned int nbResets = ptg->getNbResets();
	double length = 3;
	double high = 3;
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
	double scaleX = length/(*valueFcts)[0].back().getX().getVal().asDouble();

	//For each state, we draw the function
	for (unsigned int i = 1; i < size; ++i){
		f << "\\begin{tikzpicture}" << endl;


		//Draw the x axis
		f << "\\draw [->][thick] (0,0) -- ( " << nbResets + length * (nbResets + 1) + 1 << ",0);" << endl;
		f << "\\draw ( " << nbResets + length * (nbResets + 1) + 1 << ",0) node[right] {$t$};" << endl;
		//f << "\\draw (0,0) node [below]{\\tiny$0$};" << endl;
		//Draw the y axis
		f << "\\draw [->][thick] (0,0) -- (0," << high + 2 << ");" << endl;
		if(ptg->hasLabels())
			f<< "\\draw (0," << high + 2<<") node[above] {$\\sigma_{" <<  ptg->getLabel(i) << "}(t)$}; " << endl;
		else
			f<< "\\draw (0," << high + 2<<") node[above] {$\\sigma_{" <<  i << "}(t)$}; " << endl;


		list<Point>::iterator it = (*valueFcts)[i].begin();
		list<Point>::iterator itNext = it;
		++itNext;

		list<Fraction> indexes;
		bool inf = false;

		//Trace for each reset
		for (unsigned int x = 0; x <= nbResets && itNext != (*valueFcts)[i].end(); ++x){
			cout << x << endl;
			//Up to the max interval
			while(itNext != (*valueFcts)[i].end() && itNext->getX() != (*valueFcts)[i].back().getX()){
				if(it->getX() == 0 && it->getInclusion()){
					f << "\\draw (" << it->getX().getVal().asDouble()*scaleX + x* length + x <<",0) node[below] {\\tiny$ " << it->getX().getVal()<< "$}; " << endl;

				}
				if(it->getX() != itNext->getX()){
					//We have to trace a segment
					if(!it->getY().isInfinity()){

						f << "\\draw [gray](" << it->getX().getVal().asDouble()*scaleX + x* length + x << ", " << it->getY().getVal().asDouble()*scale <<") -- (" << itNext->getX().getVal().asDouble()*scaleX + x* length + x << "," << itNext->getY().getVal().asDouble()*scale << ");" << endl;
						f << "\\draw (" << itNext->getX().getVal().asDouble()*scaleX + x* length + x <<",0) node[below] {\\tiny$ " << itNext->getX().getVal()<< "$}; " << endl;

						indexes.push_back(it->getY().getVal());
					}

					else{
						f << "\\draw [gray](" << it->getX().getVal().asDouble()*scaleX + x* length + x << ", " << high + 1 <<") -- (" << itNext->getX().getVal().asDouble()*scaleX + x* length + x << "," << high + 1 << ");" << endl;
						f << "\\draw (" << itNext->getX().getVal().asDouble()*scaleX + x* length + x <<",0) node[below] {\\tiny$ " << itNext->getX().getVal()<< "$}; " << endl;
						inf = true;
					}


				}
				else{
					list<Point>::iterator itLast = it;
					if(it != (*valueFcts)[i].begin() && it->getX() != 0)
							--itLast;

					if (it->getInclusion() && (it->getY() != itNext->getY() || it->getY() != itLast->getY()) && !it->getY().isInfinity()){
						//We have a "jump" in the value fct

						f << "\\draw[black,fill=black] (" << it->getX().getVal().asDouble()*scaleX + x* length + x << "," <<it->getY().getVal().asDouble()*scale << ") circle (0.04);" << endl;

					}
				}
				it = itNext;
				++itNext;
			}
			//Need to trace the last segment

			if(!it->getY().isInfinity()){
				f << "\\draw [gray](" << it->getX().getVal().asDouble()*scaleX + x * length + x << ", " << it->getY().getVal().asDouble()*scale <<") -- (" << itNext->getX().getVal().asDouble()*scaleX + x* length + x << "," << itNext->getY().getVal().asDouble()*scale << ");" << endl;
				f<< "\\draw (" << itNext->getX().getVal().asDouble()*scaleX + x* length + x <<",0) node[below] {\\tiny$ " << itNext->getX().getVal()<< "$}; " << endl;

				indexes.push_back(it->getY().getVal());
				indexes.push_back(itNext->getY().getVal());


			}

			else{
				f << "\\draw (" << it->getX().getVal().asDouble()*scaleX + x* length + x << ", " << high + 1 <<") -- (" << itNext->getX().getVal().asDouble()*scaleX + x* length + x << "," << high + 1 << ");" << endl;
				f << "\\draw (" << itNext->getX().getVal().asDouble()*scaleX + x* length + x <<",0) node[below] {\\tiny$ " << itNext->getX().getVal()<< "$}; " << endl;
				inf = true;
			}


			//And maybe the last point
			it = itNext;
			++itNext;
			if (itNext->getInclusion() && it->getY() != itNext->getY() && !itNext->getY().isInfinity()){
				//We have a "jump" in the value fct

				f << "\\draw[black, fill=black] (" << itNext->getX().getVal().asDouble()*scaleX + x* length + x << "," <<itNext->getY().getVal().asDouble()*scale << ") circle (0.04);" << endl;

			}
			//Place the iterators at the start of the next reset interval
			++itNext;
			it = itNext;
			++itNext;


		}

		if (inf){
			//???
		}

		indexes.unique();

		for (list<Fraction>::iterator itY = indexes.begin(); itY != indexes.end(); ++itY){
			f << "\\draw (0," <<  itY->asDouble() * scale << ") node[left] {\\tiny$" << (*itY) << "$};" << endl;
		}

		if (inf)
			f << "\\draw (0," <<  high + 1 << ") node[left] {$inf$};" << endl;


		f << "\\end{tikzpicture}" << endl;
		f << "\\newline" << endl;
	}

	f << "\\end{document}" << endl;
	f.close();
	system("pdflatex valueFcts.tex");
}

void PTGVisu::visualizeStrats(PTG* ptg, vector<list<Point> >* valueFcts){
	ofstream f;
	const int length = 3;

	unsigned int size = ptg->getSize();

	f.open("strategies.tex");
	f << "\\documentclass{article}" << endl;
	f << "\\usepackage{tikz}" << endl;
	f << "\\begin{document}" << endl;


	for(unsigned int i = 1; i < size; ++i){
		f << "\\begin{tikzpicture}" << endl;
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
				color = "blue";
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
				f << "\\draw [" << color << "] (" << (it->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << ((Fraction(it->getDest())/ Fraction(maxY) * Fraction(length))  + 1).asDouble()<< ") -- (" << (itNext->getX().getVal()/maxX  * Fraction(length) + x).asDouble()<< "," << (Fraction(it->getDest()) / Fraction(maxY) * Fraction(length) + 1).asDouble()<< ");" << endl;
				f << "\\draw (" << (it->getX().getVal() / maxX * Fraction(length) + x).asDouble() << ",0) node[below] {\\tiny$" << it->getX().getVal() << "$};" << endl;
				if(ptg->hasLabels())
					f << "\\draw (0,"  << ((Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)) + 1).asDouble() << ") node[left] {\\tiny$" << ptg->getLabel(it->getDest()) << "$};" << endl;
				else
					f << "\\draw (0,"  << ((Fraction(it->getDest())/ Fraction(maxY) * Fraction(length))+ 1).asDouble() << ") node[left] {\\tiny$" << it->getDest() << "$};" << endl;
			}

			if(it->getInclusion() && itNext->getX().getVal() != maxX && ((itNext != (*valueFcts)[i].end() && itNext->getDest() != it->getDest()) || it->getX().getVal() == 0 || it->getX().getVal() == maxX)){
				if(!itNext->getInclusion() && itNext != (*valueFcts)[i].end() && itNext->getDest() != it->getDest() && it->getX().getVal() != 0 && it->getX().getVal() != maxX){
					f << "\\node [circle,draw,fill= " << color <<",scale=0.4] at (" << (it->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(it->getDest())/ Fraction(maxY) * Fraction(length) + 1 << ") {};" << endl;

					f << "\\draw[" << colorNext << ",fill=white] (" << (itNext->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length) + 1 << ") circle (0.07);" << endl;
					if(ptg->hasLabels())
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) + 1).asDouble() << ") node[left] {\\tiny$" << ptg->getLabel(itNext->getDest()) << "$};" << endl;
					else
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length))+ 1).asDouble() << ") node[left] {\\tiny$" << itNext->getDest() << "$};" << endl;

				}
				else if(itNext->getInclusion() && itNext != (*valueFcts)[i].end() && itNext->getDest() != it->getDest() && it->getX().getVal() != 0 && it->getX().getVal() != maxX && itNext->getX().getVal() != maxX){
					f << "\\draw[" << color << ",fill=white] (" << (itNext->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(it->getDest())/ Fraction(maxY) * Fraction(length) + 1 << ") circle (0.07);" << endl;

					f << "\\node [circle,draw,fill="<< colorNext <<",scale=0.4] at (" << (itNext->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length) + 1 << ") {};" << endl;

					if(ptg->hasLabels())
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) + 1).asDouble() << ") node[left] {\\tiny$" << ptg->getLabel(itNext->getDest()) << "$};" << endl;
					else
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) + 1).asDouble() << ") node[left] {\\tiny$" << itNext->getDest() << "$};" << endl;

				}
				else{
					f << "\\node [circle,draw,fill= " << color <<",scale=0.4] at (" << (it->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(it->getDest())/ Fraction(maxY) * Fraction(length) + 1 << ") {};" << endl;

				}

			}

			else if(!it->getInclusion() && itNext != (*valueFcts)[i].end() && itNext->getDest() != it->getDest()){
				f << "\\draw[" << color << ",fill=white] (" << (itNext->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(it->getDest())/ Fraction(maxY) * Fraction(length) + 1 << ") circle (0.07);" << endl;
				cout << "time: " << itNext->getX().getVal() << endl;
				if(itNext != (*valueFcts)[i].end() && itNext->getDest() != it->getDest()){
					if(ptg->hasLabels())
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) + 1).asDouble() << ") node[left] {\\tiny$" << ptg->getLabel(itNext->getDest()) << "$};" << endl;
					else
						f << "\\draw (0,"  << ((Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)) + 1).asDouble() << ") node[left] {\\tiny$" << itNext->getDest() << "$};" << endl;
					f << "\\draw[fill=" << colorNext << "] (" << (itNext->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(itNext->getDest())/ Fraction(maxY) * Fraction(length)+ 1 << ") circle (0.07);" << endl;
				}
			}
			else if(it->getInclusion() && it->getX().getVal() == 0){
				f << "\\draw[fill=" << color << "] (" << (it->getX().getVal()/ maxX * Fraction(length) + x).asDouble() << "," << Fraction(it->getDest())/ Fraction(maxY) * Fraction(length) + 1 << ") circle (0.07);" << endl;
				if(ptg->hasLabels())
					f << "\\draw (0,"  << ((Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)) + 1).asDouble() << ") node[left] {\\tiny$" << ptg->getLabel(it->getDest()) << "$};" << endl;
				else
					f << "\\draw (0,"  << ((Fraction(it->getDest())/ Fraction(maxY) * Fraction(length)) + 1).asDouble() << ") node[left] {\\tiny$" << it->getDest() << "$};" << endl;

			}


		}

		//Draw the x axis
		f << "\\draw [->][thick] (0,0) -- ( " << (x + length + 1).asDouble() << ",0);" << endl;
		f << "\\draw ( " << x + length + 1 << ",0) node[right] {$t$};" << endl;
		f << "\\draw (0,0) node [below]{\\tiny$0$};" << endl;
		//Draw the y axis
		f << "\\draw [->][thick] (0,0) -- (0," << (Fraction(length + 1)).asDouble()  << ");" << endl;
		if(ptg->hasLabels())
			f<< "\\draw (0," << (Fraction(length + 1)).asDouble()<<") node[above] {$\\sigma_{" <<  ptg->getLabel(i) << "}(t)$}; " << endl;
		else
			f<< "\\draw (0," << (Fraction(length + 1)).asDouble()<<") node[above] {$\\sigma_{" <<  i << "}(t)$}; " << endl;

		f << "\\end{tikzpicture}" << endl;
		f << "\\newline" << endl;
	}
	f << "\\begin{tikzpicture}" << endl;
	f << "\\draw[black, fill=black] (0,3) circle (0.07);" << endl;
	f << "\\node [right] at (0.5,3) {Take transition};" << endl;
	f << "\\draw[black, fill=blue] (0,2) circle (0.07);" << endl;
	f << "\\node [right] at (0.5,2) {Wait};" << endl;
	f << "\\draw[black, fill=red] (0,0) circle (0.07);" << endl;
	f << "\\node [right] at (0.5,1) {Reset};" << endl;
	f << "\\end{tikzpicture}" << endl;


	f << "\\end{document}" << endl;
	system("pdflatex strategies.tex");
}


