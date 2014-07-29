all: main.o Point.o SPTG.o SPTGSolver.o Fraction.o Strategy.o PTG.o
	g++ -o solver main.o Point.o SPTG.o SPTGSolver.o Fraction.o Strategy.o PTG.o

Point.o: Point.cpp
	g++ -c -W -Wall -pedantic Point.cpp
	
SPTG.o: SPTG.cpp 
	g++ -c -W -Wall -pedantic SPTG.cpp

SPTGSolver.o: SPTGSolver.cpp
	g++ -c -W -Wall -pedantic SPTGSolver.cpp

Fraction.o: Fraction.cpp
	g++ -c -W -Wall -pedantic Fraction.cpp
	
Strategy.o: Strategy.cpp
	g++ -c -W -Wall -pedantic Strategy.cpp
	
PTG.o: PTG.cpp
	g++ -c -W -Wall -pedantic PTG.cpp
	
main.o: main.cpp
	g++ -c -W -Wall -pedantic main.cpp
	
clean:
	rm *.o solver