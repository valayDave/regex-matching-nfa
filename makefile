all: search psearch
search : Serial/nfa-builder.cc
	g++ Serial/nfa-builder.cc -o search --std=c++11 

psearch : Parallel/nfa-builder.cc
	g++ Parallel/nfa-builder.cc -o psearch -fopenmp --std=c++11 