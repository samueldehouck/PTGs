#include "Strategy.hpp"
#include "constants.hpp"

using namespace std;

Strategy::Strategy(){
	time = ifnty;
	size = 0;
	included = false;
}

Strategy::Strategy(unsigned int s){
	time = ifnty;
	size = s;
	included = false;
	for (unsigned int j = 0; j <= size; ++j){
		dests.push_back(0);
		types.push_back(0);
	}
}

Strategy::Strategy(unsigned int s, Fraction t, bool i){
	time = t;
	size = s;
	included = i;
	for (unsigned int j = 0; j < size; ++j){
		dests.push_back(0);
		types.push_back(0);
	}
}

Strategy::Strategy(const Strategy& s){
	time = s.getTime();
	dests.clear();
	types.clear();
	size = s.getSize();
	included = s.getInclusion();
	for (unsigned int i = 0; i < size; ++i){
		dests.push_back(s.getDest(i));
		types.push_back(s.getType(i));
	}
}

void Strategy::insert(unsigned int index, unsigned int dest, unsigned int type){
	if(dests.size() > index){
		dests[index] = dest;
		types[index] = type;
	}
}

unsigned int Strategy::getDest(unsigned int index) const{
	return dests[index];
}

unsigned int Strategy::getType(unsigned int index) const{
	return types[index];
}

Fraction Strategy::getTime() const{
	return time;
}

void Strategy::setTime(Fraction t){
	time = t;
}

unsigned int Strategy::getSize() const{
	return size;
}

bool Strategy::getInclusion() const{
	return included;
}

void Strategy::show(){
	if(included)
		cout << "Time: " << time << " (included)" << endl;
	else
		cout << "Time: " << time << endl;
	for (unsigned int i = 0; i < size; ++i){
		cout << dests[i] << " (" << types[i] << ") ";
	}
	cout << endl;
}
