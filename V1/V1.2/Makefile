all: main.o Point.o SPTG.o SPTGSolver.o
	g++ -o solver main.o Point.o SPTG.o SPTGSolver.o

Point.o: Point.cpp
	g++ -c -W -Wall -pedantic Point.cpp
	
SPTG.o: SPTG.cpp 
	g++ -c -W -Wall -pedantic SPTG.cpp

SPTGSolver.o: SPTGSolver.cpp
	g++ -c -W -Wall -pedantic SPTGSolver.cpp
	
main.o: main.cpp
	g++ -c -W -Wall -pedantic main.cpp
	
clean:
	rm *.o