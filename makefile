all: search psearch
search : Serial/nfa-builder.cc
	g++-8 Serial/nfa-builder.cc -o search

psearch : Parallel/nfa-builder.cc
	g++-8 Parallel/nfa-builder.cc -o psearch -fopenmp