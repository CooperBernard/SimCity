main: main.o Cell.o
	g++ main.o Cell.o -o main

main.o: main.cpp
	g++ -c main.cpp

Cell.o: Cell.cpp Cell.h
	g++ -c Cell.cpp

clean:
	rm -f *.o