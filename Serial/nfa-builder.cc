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
    string trans_symbol;
};

struct trans {
    int destination;
    string symbol;
};

struct matched_symbol{
    int start_position;
    string token;
};

string EPSILON_TRANSITION = "\u03B5";

bool isOperand(char c);

bool isOperator(char c);

bool checkBinaryOperation(char c);

bool isHigherPresedence(char a, char b);

int getOperatorWeight(char inputOperator);

string changeRegexOperators(string regex);

string convertRegexToPostfix(string expression);

class NFA {
public:

    //node_graph[STATE_NUMBER] --> The Vector Inside this contains the transitions available from this State 
    vector<vector<trans>> node_graph;
    vector<int> vertices;
    vector<transition> node_trans;
    vector<int> final_states;
    int final_state;//TODO : Need to Evaluate if this is Correct Or Not. 

    void set_transition(int start_vertex, int end_vertex, string trans_symbol) {
      transition newTrans;
      newTrans.trans_symbol = trans_symbol;
      newTrans.vertex_start = start_vertex;
      newTrans.vertex_end = end_vertex;
      node_trans.push_back(newTrans);
    }

    //Populates the Node Graph For Traversal and returns the Same Graph.
    vector<vector<trans>> construct_node_graph(){
        vector<transition>::iterator ptr;
        //Iterate through the node transitions and construct the graph 
        for(ptr = node_trans.begin();ptr < node_trans.end();ptr++){ 
            trans node_connect;
            node_connect.destination = ptr->vertex_end;
            node_connect.symbol = ptr->trans_symbol;
            node_graph[ptr->vertex_start].push_back(node_connect);
        }
        return node_graph;
    }

    int get_node_count() {
      return vertices.size();
    }

    void set_nodes(int no_of_vertices) {
      for (int i = 0; i < no_of_vertices; i++) {
        vertices.push_back(i);
      }
    }

    void set_final_state(int vertex){
        final_state = vertex;
        final_states.push_back(vertex);
    }

    vector<int> get_final_states(){
        return final_states;
    }

    //TODO : Need to Evaluate if this is Correct Or Not. 
    int get_final_state() {
      return final_state;
    }

    vector<trans> available_state_transitions(int state, string transition_symbol){
        vector<trans> available_transitions;
        vector<trans>::iterator state_ptr;
        for(state_ptr=node_graph[state].begin();state_ptr<node_graph[state].end();state_ptr++){
            //State Has an Allowed Transtion or An Epsilon Transtion than send that to the available transtions.
            if(state_ptr->symbol == transition_symbol || state_ptr->symbol == EPSILON_TRANSITION){
                available_transitions.push_back(*state_ptr);
            }
        }
        return available_transitions;
    }

    void match_string(string text) {

        construct_node_graph();

        string evaluatedTxt = "";
        int i=0,current_state = 0;
        while(evaluatedTxt!=text){
            evaluatedTxt+=text[i];

        }
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
    //token should be a single character String or EPSILON
    void traverse_graph(string buffer,string token,int currentState){
        vector<trans> available_transtions = available_state_transitions(currentState,token);
        if(available_transtions.size() > 0){

        }else{  
            //TODO : Need to Figure what we need to do.
        }
    }

    //DOUBT : CAN EPSILON LOOPING BE A PROBLEM --> This Literally Just Traverses the State Machine. --> May it be NFA or DFA.
    //This function Evaluates a path from a current Node to the Other possible Nodes and finds if that path is going to have any 
    vector<matched_symbol> evaluate_path(string text,string buffer,int currentCharPosition,int currentState,vector<matched_symbol>symbols){
        //Crossed the Length of the String so Return back to the main function.
        if(currentCharPosition>=text.length()){
            return symbols;
        }
        string token = string(1,text[currentCharPosition]); //Token is the Individual Character that needs to be evaluated.
        buffer+=token; // TODO : Evaluate Weather this line is correct or not.

        if (check_for_final_state(currentState)) {
            //TODO/DOUBT : Should I add the strings here and store it is the mapped_symbol Vector Arr or should it be done when the currentState == final_state.
            matched_symbol symbol;
            symbol.start_position = currentCharPosition;
            symbol.token = buffer;
            symbols.push_back(symbol);
        }
        //Available Transitions from the current Node. 
        vector<trans> available_transtions = available_state_transitions(currentState,token);

        if (available_transtions.size() > 0) {
            // If one of the available transtions is a Final State then We need to print the Token we have found.
            for (int i = 0; i < available_transtions.size(); i++) {
                int newCharPosition = currentCharPosition;
                if(available_transtions.at(i).symbol != EPSILON_TRANSITION){
                    newCharPosition = ++currentCharPosition;
                }
                int newState = available_transtions.at(i).destination ;
                evaluate_path(text, buffer, newCharPosition, newState,symbols);
            }
        } else {
            //TODO : Need to Figure what we need to do.
            int newCharPosition = ++currentCharPosition;
            int newStartState = 0;
            string newBuffer = "";
            evaluate_path(text, newBuffer, newCharPosition, newStartState,symbols);
        }
    }

    bool check_for_final_state(int state){
        vector<int>::iterator ptr;
        for(ptr=final_states.begin();ptr< final_states.end();ptr++){
            if(state == *ptr){
                return true;
            }
        }
        return false;
    }

    void tranverse_path(){
        
    }
};




NFA concatNFAs(NFA a,NFA b){
    NFA result;
    //Set Total Vertices.
    result.set_nodes(a.get_node_count()+b.get_node_count());

    //Set Transitions of NFA A into the consolidating NFA;
    for(int i=0;i<a.node_trans.size();i++){
        transition newTrans = a.node_trans.at(i);
        result.set_transition(newTrans.vertex_start,newTrans.vertex_end,newTrans.trans_symbol);
    }

    //Set EPSILION_TRANSITIONS from all Final States in A to the Start States of B; : https://math.stackexchange.com/questions/349994/concatenation-of-2-finite-automata
    vector<int> finalStateOfA = a.get_final_states();
    for(int i=0;i<finalStateOfA.size();i++){
        result.set_transition(finalStateOfA.at(i),a.get_node_count(),EPSILON_TRANSITION);
    }

    //setting Transtions of NFA B
    for(int i=0;i<b.node_trans.size();i++){
        transition newTrans = b.node_trans.at(i);
        result.set_transition(newTrans.vertex_start+a.get_node_count(),newTrans.vertex_end+a.get_node_count(),newTrans.trans_symbol);
    }

    //Ideally Should Have One Final State; Still Skeptical about this.
    result.set_final_state(a.get_node_count() + b.get_node_count()-1);

    return result;
}

//This is to Print the Entire NFA so that There is some Reference on what is getting Constructed 
void printNFA(NFA a){
    transition tran;
    cout << '\n';
    for(int i=0;i<a.node_trans.size();i++){
        tran = a.node_trans.at(i);
        cout<<"q"<<tran.vertex_start<<" --> q"<<tran.vertex_end<<" : Symbol - "<<tran.trans_symbol<<endl;    
    }
    cout<<"\nThe final state is q"<<a.get_final_state()<<endl;
}

NFA createSingleSymbolNFA(char symbol){
    NFA result;
    result.set_nodes(2);
    result.set_transition(0,1,string(1,symbol));
    result.set_final_state(1);
    return result;
}

NFA postFixNFABuilder(string postFixExpr){
    stack<NFA> evaluationStack;
    NFA result;
    for(int i=0;i<postFixExpr.length();i++){
        if(isOperand(postFixExpr[i])){
            NFA operand = createSingleSymbolNFA(postFixExpr[i]);
            //printNFA(operand);
            evaluationStack.push(operand);
        }else{ // It is an Operator
            if(checkBinaryOperation(postFixExpr[i])){ 
                NFA operand2 = evaluationStack.top();
                evaluationStack.pop();
                NFA operand1 = evaluationStack.top();
                evaluationStack.pop();
                if(postFixExpr[i] == '+'){
                    NFA resultNFA = concatNFAs(operand1,operand2);
                    evaluationStack.push(resultNFA);
                }else{ // Union Because its not Concatenation
                    cout << "Kleene Star Not Handled." << endl;
                    //TODO: Handle Union Operator Here.
                }
            }else{
                cout << "Kleene Star Not Handled." << endl;
                //TODO : Evaluate the Kleene star Operator Here.
            }
        }
    }
    if(evaluationStack.size() > 0){
        if(evaluationStack.size() == 1){
            result = evaluationStack.top();
            evaluationStack.pop();
        }else{
            cout << "Something Messed Up. The Stack is of Size Greater Than 1" << endl;
        }
    }else{
        cout << "Something Messed Up. The Stack is of Size Zero" << endl;
    }
    return result;
}

/**
 *<summary>Check if a Character is an operator for a regular expression</summary> 
 *<param name="c">character to be evaluated</param>
 */

bool isOperator(char c) {
  if (c == '+' || c == '*' || c == '|')
    return true;

  return false;
}

bool checkBinaryOperation(char c){
    if(c == '+' || c == '|'){
        return true;
    }
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
        if (i == 0) {
            replacedRegex +=(regex[i]);
            continue;
        }else if (regex[i] =='+'){
            replacedRegex += '|';
        }else if(isOperand(regex[i]) && isOperand(regex[i-1])){
            replacedRegex +=('+');
            replacedRegex +=(regex[i]);
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

    NFA finalResult = postFixNFABuilder(postfixRegex);
    
    printNFA(finalResult);
}