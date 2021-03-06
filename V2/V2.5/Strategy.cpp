#include "Strategy.hpp"
#include "constants.hpp"

using namespace std;

Strategy::Strategy(){

}

Strategy::Strategy(unsigned int s){
	time = ifnty;
	size = s;
	included = false;
	for (unsigned int j = 0; j <= size; ++j){
		dests.push_back(0);
		lambdas.push_back(false);
	}
}

Strategy::Strategy(unsigned int s, Fraction t, bool i){
	time = t;
	size = s;
	included = i;
	for (unsigned int j = 0; j < size; ++j){
		dests.push_back(0);
		lambdas.push_back(false);
	}
}

Strategy::Strategy(const Strategy& s){
	time = s.getTime();
	dests.clear();
	lambdas.clear();
	size = s.getSize();
	included = s.getInclusion();
	for (unsigned int i = 0; i < size; ++i){
		dests.push_back(s.getDest(i));
		lambdas.push_back(s.getLambda(i));
	}
}

void Strategy::insert(unsigned int index, unsigned int dest, bool lambda){
	if(dests.size() > index){
		dests[index] = dest;
		lambdas[index] = lambda;
	}
}

unsigned int Strategy::getDest(unsigned int index) const{
	return dests[index];
}

bool Strategy::getLambda(unsigned int index) const{
	return lambdas[index];
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
		cout << dests[i] << " (" << lambdas[i] << ") ";
	}
	cout << endl;
}
