#ifndef PTGFACTORY_HPP
#define PTGFACTORY_HPP

#include "PTG.hpp"
#include "constants.hpp"
#include <cstdlib>

class PTGFactory{
private:

public:
	PTGFactory();
	PTG* build();
};

#endif
