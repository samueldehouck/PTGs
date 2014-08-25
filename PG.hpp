#ifndef PG_HPP
#define PG_HPP

#include <vector>
#include "Value.hpp"

using namespace std;

class PG{
protected:
	vector<vector<Value> >* transitions;
	vector<bool>* owners;
public:
	PG();
	PG(vector<vector<Value> >*, vector<bool>*);
	~PG();
	unsigned int getSize() const;
	Value getTransition(unsigned int, unsigned int) const;
	void setTransition(unsigned int,unsigned int, Value);
	bool getOwner(unsigned int) const;
	void setOwner(unsigned int, bool);
};

#endif
