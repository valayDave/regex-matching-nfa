#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <stdlib.h>
#include <string>
using namespace std;

struct transition {
    int vertex_start;
    int vertex_end;
    char trans_symbol;
};

struct trans {
    int destination;
    char symbol;
};

class NFA {
public:
    vector< vector<trans> > node_graph;
    vector<int> vertices;
    vector<transition> node_trans;
    int final_state;

    void set_transition(int start_vertex, int end_vertex, char trans_symbol)
    {
        transition new_trans;
        new_trans.trans_symbol = trans_symbol;
        new_trans.vertex_start = start_vertex;
        new_trans.vertex_end = end_vertex;
        node_trans.push_back(new_trans);
    }

    //Populates the Node Graph For Traversal.
    void construct_node_graph(){
        vector<transition>::iterator ptr;
        //Iterate through the node transitions and construct the graph 
        for(ptr = node_trans.begin();ptr < node_trans.end();ptr++){ 
            trans node_connect;
            node_connect.destination = ptr->vertex_end;
            node_connect.symbol = ptr->trans_symbol;
            node_graph[ptr->vertex_start].push_back(node_connect);
        }
    }

    int get_node_count() { return vertices.size(); }

    void set_nodes(int no_of_vertices)
    {
        for (int i = 0; i < no_of_vertices; i++) {
            vertices.push_back(i);

        }
    }

    int get_final_state() { return final_state; }

    void match_string(string text)
    {
        //This will be the Method through which the state transitions will happen.

        // The Nodes in an NFA can be visulised as a graph where one can iterate through the graph.

        //Start from the 0th Node and check take each character and see if it is going to the other States. 
        
        //Take Epsilon transitions to reach other states and and then to th

        //Or DFA Can be constructed from this. Once DFA is constructed traverse its's Nodal Graph to reach other states that address each token

            //TODO : Psuedo Code : --> Still Incomplete. Need to Figure if recursion is the answer or something else.  
                //state = 0;
                //buffer = "";
                //for each token t 
                    //if state_trans_available(state,t) 
                        //new_state =  move(state,transition_symbol) --> this yeilds next state
                        //buffer+=t;
                        //if new_state ==  is_final_state(state)
                            // print buffer , column number etc.
                            // buffer = "" --> reset the buffer. 
                            //if   
                    //else 
                        //continue;
    }
};

//TODO: Create base NFA constructions for Kleene star

//TODO: Create Base NFA constructions for Union

//TODO:Create Base NFA constructions for Concatenation

//TODO : Create a RE to NFA function that Takes the regular Expression converts it into a post fix operation and then constructs the NFA's from the Regex

//TODO : Once the final NFA is constructed from postfix expression --> Either Convert that To DFA or Try and run the string matching in this NFA. 

//TODO : Should I construct a DFA with this. 

int main(int argc, char* argv[])
{
    // Arguement 0 is the Name of the Binary so it is ignored.
    // First Arguement of the function is a regex to be Found Within the File.
    string regularExpression = argv[1];
    vector<string> fileNames;

    // As the Number of Files that will be passed to the Code with be unknown a
    // Vector is chosen. This is Inefficient. I can use strings Instead of
    // Vectors.
    for (int i = 2; i < argc; i++) {
        fileNames.push_back(argv[i]);
        // cout << argv[i] << endl;
    }
}