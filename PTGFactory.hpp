/*
 * Author: Samuel Dehouck
 */


#ifndef PTGFACTORY_HPP_
#define PTGFACTORY_HPP_

#include "PTG.hpp"

class PTGFactory{
private:

public:
	PTGFactory();
	PTG* buildPTG(int, int, int, int, int, int);
	PTG* buildSPTG(int, int, int, int);
	PTG* buildFromFile(char*);
	PTG* buildFromFlatFile(char*);
	PTG* buildFromXmlFile(char*);
	PTG* hardBuild(unsigned int);
	void buildOutputFcts(char*, PTG*);

};

#endif
