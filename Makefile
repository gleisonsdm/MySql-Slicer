queries: slicer/queries.hpp
	g++ -c slicer/queries.cpp -o queries.o -l mysqlcppconn

process: slicer/process.hpp
	g++ -c slicer/process.cpp -o process.o -l mysqlcppconn

main: slicer/queries.hpp slicer/process.hpp
	g++ -c slicer/main.cpp -o main.o -l mysqlcppconn

all: queries process main
	g++ queries.o process.o main.o -o main.out  -l mysqlcppconn
