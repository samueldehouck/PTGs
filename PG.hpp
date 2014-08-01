#ifndef PG_HPP
#define PG_HPP

#include <vector>

using namespace std;

class PG{
protected:
	vector<vector<int> >* transitions;
	vector<bool>* owners;
public:
	PG();
	PG(vector<vector<int> >*, vector<bool>*);
	~PG();
	unsigned int getSize() const;
	int getTransition(unsigned int, unsigned int) const;
	void setTransition(unsigned int,unsigned int, int);
	bool getOwner(unsigned int) const;
};

#endif
