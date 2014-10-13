
cs463project3: dpll.o InputParser.o SATSolverApp.o
	mpiCC dpll.o InputParser.o SATSolverApp.o -o cs463project3

SATSolverApp.o: SATSolverApp.cpp
	mpiCC -c SATSolverApp.cpp

dpll.o: dpll.cpp
	mpiCC -c dpll.cpp

InputParser.o: InputParser.cpp
	mpiCC -c InputParser.cpp

clean:
	rm -f *.o cs463project3