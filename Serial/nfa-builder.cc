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

bool isOperand(char c);

bool isOperator(char c);

int getOperatorWeight(char inputOperator);

bool isHigherPresedence(char a, char b);

string changeRegexOperators(string regex);

string convertRegexToPostfix(string expression);

class NFA {
public:
    vector< vector<trans> > node_graph;
    vector<int> vertices;
    vector<transition> node_trans;
    int final_state;

    void set_transition(int start_vertex, int end_vertex, char trans_symbol) {
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

    int get_node_count() {
      return vertices.size();
    }

    void set_nodes(int no_of_vertices) {
      for (int i = 0; i < no_of_vertices; i++) {
        vertices.push_back(i);
      }
    }

    int get_final_state() {
      return final_state;
    }

    void match_string(string text){
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


/**
 *<summary>Check if a Character is an operator for a regular expression</summary> 
 *<param name="c">character to be evaluated</param>
 */

bool isOperator(char c) {
  if (c == '+' || c == '*' || c == '|')
    return true;

  return false;
}

/**
 *<summary>Check if a Character is an operand for a regular expression</summary>
 *<param name="c">character to be evaluated</param>
 */
bool isOperand(char c) {
    if (c >= '0' && c <= '9')
        return true;
    if (c >= 'a' && c <= 'z')
        return true;
    if (c >= 'A' && c <= 'Z')
        return true;
    if (c == '.')
        return true;
    return false;
}

/**
 *<summary>Find the Weight of the Operator to create the post fix notation</summary>
 *<param name="inputOperator"> Operator whose weight needs to be evaluated for post fix notation evaluation.</param> 
 *<return> The weight ofthe operator for determining precedence </return>
 */
int getOperatorWeight(char inputOperator) {
  int weight = 1;
  switch (inputOperator) {
    case '*':  // Kleene Star -- Unary Operator
      weight = 2;
      break;
    case '|':  // Union --> Binary Operator
      weight = 0;
      break;
    case '+':  // Concatenation. --> Binary Operator
      weight = 1;
      break;
  }
  return weight;
}

bool isHigherPresedence(char a, char b) {
  return getOperatorWeight(a) > getOperatorWeight(b) ? true : false;
}

//Preprocess the Regex to change the concatenation Operator.
/**
 * <summary>Converting the base regex string to a new formatted string. The New string contains | for Union and + for concatenation instead of + for Union and nothing for concatenation</summary>
 * <param name="regex">Regular Expression to change</param>
*/
string changeRegexOperators(string regex){
    string replacedRegex="";
    for (int i = 0; i < regex.length(); i++) {
        if (i == 0 || i== static_cast<int>(regex.length())-1) {
            replacedRegex +=(regex[i]);
            continue;
        }else if (regex[i] =='+'){
            replacedRegex += '|';
        }else if(isOperand(regex[i]) && isOperand(regex[i+1])){
            replacedRegex +=(regex[i]);
            replacedRegex +=('+');
        }else if(regex[i] == ')' && (regex[i+1]=='(' || isOperand(regex[i+1]))){
            replacedRegex +=(regex[i]);
            replacedRegex +=('+');
        }else{
            replacedRegex+= regex[i];
        } 
    }
    return replacedRegex;
}



/**
 * <summary>Creates a Vector of strings to match for the Regular Expression Provided</summary>
 * <param name='expression'>Regualar expression of the type "a+be*" or "(ab)+(c)"</param>
 */
string convertRegexToPostfix(string expression) {
    stack<char> operatorStack;
    // Only Operators(+ or *) will have a single Character Operand Unless
    // Otherwise. Concatenated Strings stored togather.
    string output;
    cout << "Starting Post Fix Notation " + expression << endl;
    // Convert the expression From an Infix Notation to a Post Fix notation.
    for (int i = 0; i < expression.length(); i++) {
        if (isOperand(expression[i])) {
            output += (expression[i]);
        } else if (isOperator(expression[i])) {
            if (!operatorStack.empty()) {
                while (!operatorStack.empty() && operatorStack.top() != '(' && isHigherPresedence(operatorStack.top(), expression[i])) {
                    output += (operatorStack.top());
                    operatorStack.pop();
                }
            }
            operatorStack.push(expression[i]);
        } else if (expression[i] == '(') {
            operatorStack.push(expression[i]);
        } else if (expression[i] == ')') {
            while (!operatorStack.empty() && operatorStack.top() != '(') {
                output += (operatorStack.top());
                operatorStack.pop();
            }
            // Removes the "(" from the Stack.
            operatorStack.pop();
        }
    }
    // Compelete Post Fix Queue created Here.
    while (!operatorStack.empty()) {
        output += (operatorStack.top());
        operatorStack.pop();
    }
    return output;
}
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
    string newRegex = changeRegexOperators(regularExpression);

    string postfixRegex = convertRegexToPostfix(newRegex);
    cout << "Post Fix Regex : "<< postfixRegex << endl;
}