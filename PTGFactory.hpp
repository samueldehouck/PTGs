#ifndef PTGFACTORY_HPP_
#define PTGFACTORY_HPP_

#include "PTG.hpp"

class PTGFactory{
private:

public:
	PTGFactory();
	PTG* build(int, int, int, int, int, int);
	PTG* buildFromFile(char*);
	PTG* hardBuild(unsigned int);

};

#endif
