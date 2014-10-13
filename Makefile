
cs463project3: dpll.o InputParser.o 
	mpiCC dpll.o InputParser.o -o cs463project3


dpll.o: dpll.cpp
	mpiCC -c dpll.cpp

InputParser.o: InputParser.cpp
	mpiCC -c InputParser.cpp

clean:
	rm -f *.o cs463project3