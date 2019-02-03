g++ nfa-builder.cc -o search 
./search "(l*o)+(o*)" haiku0.txt haiku2.txt haiku3.txt

# ./search "(l|o)*" haiku0.txt # haiku2.txt haiku3.txt 
# ./search "(lo)*" haiku0.txt # haiku2.txt haiku3.txt