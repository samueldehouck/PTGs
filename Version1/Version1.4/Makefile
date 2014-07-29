all: main.o Point.o SPTG.o SPTGSolver.o Fraction.o
	g++ -o solver main.o Point.o SPTG.o SPTGSolver.o Fraction.o

Point.o: Point.cpp
	g++ -c -W -Wall -pedantic Point.cpp
	
SPTG.o: SPTG.cpp 
	g++ -c -W -Wall -pedantic SPTG.cpp

SPTGSolver.o: SPTGSolver.cpp
	g++ -c -W -Wall -pedantic SPTGSolver.cpp

Fraction.o: Fraction.cpp
	g++ -c -W -Wall -pedantic Fraction.cpp
	
main.o: main.cpp
	g++ -c -W -Wall -pedantic main.cpp
	
clean:
	rm *.o solver