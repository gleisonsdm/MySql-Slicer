queries: slicer/queries.hpp
	g++ -c slicer/queries.cpp -o queries.o

process: slicer/process.hpp
	g++ -c slicer/process.cpp -o process.o

main: slicer/queries.hpp slicer/process.hpp
	g++ -c slicer/main.cpp -o main.o

all: queries process main
	g++ queries.o process.o main.o -o main.out  -l mysqlcppconn
