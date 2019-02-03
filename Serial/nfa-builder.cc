#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <stdlib.h>
#include <string>
using namespace std;

struct DFA_trans{
    set<int> vertex_start;
    set<int> vertex_end;
    string trans_symbol;
    int renamed_vertex_start;
    int renamed_vertex_end;
};

struct DFA_trans_mark{
    set<int> vertex;
    bool marked;
    int renamed_vertex_id;
    DFA_trans_mark(){
        marked = false;
    }
};

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

struct graph_state{
    trans avail_trans;
    int char_position;
    string buffer;
};

string DEFAULT_FILE_PATH = "Files/";
string EPSILON_TRANSITION = "\u03B5";

bool isOperand(char c);

bool isOperator(char c);

bool checkBinaryOperation(char c);

bool isHigherPresedence(char a, char b);

int getOperatorWeight(char inputOperator);

string changeRegexOperators(string regex);

string convertRegexToPostfix(string expression);

void print_set(set<int> a); 
void printOutput(string fileName, int lineNumber, int startIndex,string patternMatched);
class NFA {
public:

    //--------NFA PROPERTIES---------------------------------------------    
    vector< vector<trans> > nfa_node_graph; //nfa_node_graph[STATE_NUMBER] --> The Vector Inside this contains the transitions available from this State 
    vector<int> vertices;
    vector<transition> node_trans;
    vector<int> final_states;
    int final_state;//TODO : Need to Evaluate if this is Correct Or Not.
    //-----------------------------------------------------

    //---------------DFA IMPORTANT PROPERTIES--------------------------------------
    vector<DFA_trans> dfa_transtions;
    vector<DFA_trans_mark> Dstates;
    vector<int> dfa_final_states; //The Final States Here are the renamed_vertexes present in the DFA Datastructure.
    set<string> alphabet;
    //-----------------------------------------------------

    //---------------------------NFA BUILDING AND TRAVERSING METHODS---------------------------------------------------
    void set_transition(int start_vertex, int end_vertex, string trans_symbol) {
      transition newTrans;
      newTrans.trans_symbol = trans_symbol;
      newTrans.vertex_start = start_vertex;
      newTrans.vertex_end = end_vertex;
      node_trans.push_back(newTrans);
    }

    //Populates the Node Graph For Traversal and returns the Same Graph.
    vector< vector<trans> > construct_nfa_graph(){
        vector<transition>::iterator ptr;
        nfa_node_graph.resize(get_node_count());
        //Iterate through the node transitions and construct the graph 
        for(ptr = node_trans.begin();ptr < node_trans.end();ptr++){
            trans node_connect;
            node_connect.destination = ptr->vertex_end;
            node_connect.symbol = ptr->trans_symbol;
            nfa_node_graph[ptr->vertex_start].push_back(node_connect);
        }
        return nfa_node_graph;
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
        for(state_ptr=nfa_node_graph[state].begin();state_ptr<nfa_node_graph[state].end();state_ptr++){
            //State Has an Allowed Transtion or An Epsilon Transtion than send that to the available transtions.
            if(state_ptr->symbol == transition_symbol || state_ptr->symbol == EPSILON_TRANSITION){
                available_transitions.push_back(*state_ptr);
            }
        }
        return available_transitions;
    }

    vector<matched_symbol> match_string(string text) {
        construct_nfa_graph();
        vector<matched_symbol> symbols;
        symbols = traverse_graph(text,"",0,0,symbols);
        //This will be the Method through which the state transitions will happen.
        //The Nodes in an NFA can be visulised as a graph where one can iterate through the graph.
        //Start from the 0th Node and check take each character and see if it is going to the other States. 
        //Take Epsilon transitions to reach other states =
        //Or DFA Can be constructed from this. Once DFA is constructed traverse its's Nodal Graph to reach other states that address each token
        return symbols;
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

    //DOUBT : CAN EPSILON LOOPING BE A PROBLEM --> This Literally Just Traverses the State Machine. --> May it be NFA or DFA.
    /**
     * <summary>
     * This function Evaluates a path from a current Node to the Other possible Nodes and finds if that path is going to reach the Final state
     * .If it reaches the final State then it will store the values in the symbols vector. 
     * </summary>
     * <param name="text">The actual Text on which the Regex needs to be evaluated</param>
     * <param name="buffer"> The buffer in which the past characters that got traversed through the states are stored</param>
     * <param name="currentCharPosition">The position of the character in the text that is being evaluated</param>
     * <param name="currentState">The Current state in the graph that from which the traversal will be done.</param>
     * <param name="symbols">The Vector which will hold all the matched Symbols it from using this NFA.</param>
     * <returns name="symbols"> The vector Of the matched_symbols</returns>
     * */ 
    vector<matched_symbol> traverse_graph(string text,string buffer,int currentCharPosition,int currentState,vector<matched_symbol>symbols){
        //Crossed the Length of the String so Return back to the main function.
        if(currentCharPosition>=text.length()){
            cout << "Reached The End of Chars " << symbols.size() << endl;
            return symbols;
        } 
        string token = string(1,text[currentCharPosition]); //Token is the Individual Character that needs to be evaluated.
        if (check_for_final_state(currentState)) {
            //TODO/DOUBT : Should I add the strings here and store it is the mapped_symbol Vector Arr or should it be done when the currentState == final_state.
            cout << "Adding Buffer to Matched Symbols " << buffer << endl;
            cout << endl;
            matched_symbol symbol;
            symbol.start_position = currentCharPosition;
            symbol.token = buffer;
            symbols.push_back(symbol);
        }
        
        // if(!cameViaEpsilon){
        //     buffer+=token; // TODO : Evaluate Weather this line is correct or not.
        // }
        cout << "Evaluating Symbol :" <<token << " "  << "On State : " << currentState << " With Buffer: " << buffer << endl;
        //Available Transitions from the current Node. 
        vector<trans> available_transtions = available_state_transitions(currentState,token);

        if (available_transtions.size() > 0) {
            cout << "Available Transtions :" << available_transtions.size() << endl;
            // If one of the available transtions is a Final State then We need to print the Token we have found.
            for (int i = 0; i < available_transtions.size(); i++) {
                trans newTranstion =available_transtions.at(i);
                int newCharPosition = currentCharPosition;
                if(newTranstion.symbol != EPSILON_TRANSITION){
                    buffer+=token;
                    newCharPosition = ++currentCharPosition;
                }
                int newState = newTranstion.destination ;
                cout << "Traversing To Destination : " << newTranstion.destination << " With the Symbol :" << newTranstion.symbol << endl;
                return traverse_graph(text, buffer, newCharPosition, newState,symbols);
                //vector<matched_symbol> newMatchedSymbols = traverse_graph(text, buffer, newCharPosition, newState,epsilon,symbols);
              //  symbols.insert(symbols.end(),newMatchedSymbols.begin(),newMatchedSymbols.end());
            }
            //return symbols;
        } else {
            //Reset the state back to 0 So that New characters can be traversed through this.
            int newCharPosition = ++currentCharPosition;
            int newStartState = 0;
            string newBuffer = "";
            return traverse_graph(text, newBuffer, newCharPosition, newStartState,symbols);
        }
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------
    
    //-----------------------------------DFA BUILDING METHODS----------------------------------------------------------------
    
    
    void convertToDFA(){
        construct_nfa_graph();
        construct_alphabet();
        set<int> set_of_states(vertices.begin(),vertices.end());
        set<int> start_state;start_state.insert(0);
        set<int> start_states = epsilon_closure(start_state);
        insert_to_d_states(start_states);
        do{
            set<int> currentStateIds = get_unmarked_state(); // This Method Also marks that StateId
            //Mark The State Here.
            cout <<"---------------------------------------------------------" << endl;
            cout << "Marking State Transitions for The Following : "<<endl;
            print_set(currentStateIds);
            for (set<string>::iterator symbol=alphabet.begin();symbol != alphabet.end();++symbol) {
                set<int> newStateIds = move(currentStateIds,*symbol);
                set<int> epsilson_trans_ids = epsilon_closure(newStateIds);
                if(!epsilson_trans_ids.empty()){
                    if(is_new_dfa_state(epsilson_trans_ids)){
                        insert_to_d_states(epsilson_trans_ids);
                    }
                }
                cout << "Transition Found for Symbol " << *symbol << endl;
                print_set(newStateIds);
                cout << "Epsilon Closure For Above Transition " <<endl;
                print_set(epsilson_trans_ids);
                if(!newStateIds.empty()){
                    DFA_trans newState;
                    newState.vertex_start = currentStateIds;
                    newState.vertex_end = epsilson_trans_ids;
                    newState.renamed_vertex_start = get_d_state_id(currentStateIds); //TODO Get a way to get the new Renamed DFA Counter.
                    newState.renamed_vertex_end = get_d_state_id(epsilson_trans_ids);
                    cout << "Transition Marked From " <<newState.renamed_vertex_start << " TO " <<newState.renamed_vertex_end << endl;
                    newState.trans_symbol = *symbol;
                    dfa_transtions.push_back(newState);
                }
            }
        cout <<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
        }while(any_umarked_states());
        
        cout << "The Final State of the NFA is :" << get_final_state() << endl;
        //Set the Final States Here.
        for(vector<DFA_trans_mark>::iterator ptr=Dstates.begin();ptr < Dstates.end();ptr++){
            cout << "evaluating the DFA Vertex " << ptr->renamed_vertex_id << endl;
            print_set(ptr->vertex);
            for(set<int>::iterator state_ptr = ptr->vertex.begin();state_ptr != ptr->vertex.end();++state_ptr){
                cout << "Checking For FS in Vertex Set with :" << *state_ptr << " " <<check_for_final_state(*state_ptr)<< endl;
                if(check_for_final_state(*state_ptr)){
                    //Add the Name of the Renamed VertexIds to the Vertex of final state
                    dfa_final_states.push_back(ptr->renamed_vertex_id);
                    break;
                }
            }
        }
        cout << "DFA Is Constructed" << endl;
    }

    bool check_for_dfa_final_state(int state){
        vector<int>::iterator ptr;
        for(ptr=dfa_final_states.begin();ptr< dfa_final_states.end();ptr++){
            if(state == *ptr){
                return true;
            }
        }
        return false;
    }

    vector<int> get_dfa_final_state(){

    }

    //Gets the New Vertex IDs of the DFA 
    int get_d_state_id(set<int> state){
        int id;
        for(vector<DFA_trans_mark>::iterator ptr = Dstates.begin();ptr < Dstates.end();ptr++){
            if(ptr->vertex == state){
                return ptr->renamed_vertex_id;
            }
        }
        return id;//This Line should Never Be Reached otherwise code is not working properly    
    }

    //Marks the new state in the DStates Vector --> NOT IN USE.
    void mark_d_state(set<int> state){
        for(vector<DFA_trans_mark>::iterator ptr = Dstates.begin();ptr < Dstates.end();ptr++){
            if(ptr->vertex == state){
                ptr->marked = true;
                return;
            }
        }
    }

    //Insert to the Vector of unmarked States.
    void insert_to_d_states(set<int>states){
        DFA_trans_mark transMark;
        transMark.vertex = states;
        transMark.renamed_vertex_id = Dstates.size();
        Dstates.push_back(transMark);
    }

    //Check if it is a new DFA State in the DStates.
    bool is_new_dfa_state(set<int>states){
        for(vector<DFA_trans_mark>::iterator ptr = Dstates.begin();ptr < Dstates.end();ptr++){
            if(ptr->vertex == states){
                return false;
            }
        }
        return true;    
    }

    //Finds an Unmarked state in the DStates Vector which holds all the Vertex Information for the DFA
    set<int> get_unmarked_state(){
        set<int> returnStateIds;
        for(vector<DFA_trans_mark>::iterator ptr = Dstates.begin();ptr < Dstates.end();ptr++){
            if(!ptr->marked){
                returnStateIds= ptr->vertex;
                ptr->marked = true;
                return returnStateIds;
            }
        }
        return returnStateIds;  
    }
    
    //Checks the Dstates Vector for Unmarked States.
    bool any_umarked_states(){
        for(vector<DFA_trans_mark>::iterator ptr = Dstates.begin();ptr < Dstates.end();ptr++){
            if(!ptr->marked){
                return true;
            }
        }
        return false;        
    }

    void construct_alphabet(){
        vector<transition>::iterator node_ptr;
        for(node_ptr = node_trans.begin();node_ptr < node_trans.end();node_ptr++){
            if(node_ptr->trans_symbol!=EPSILON_TRANSITION)
            alphabet.insert(node_ptr->trans_symbol);
        }
        return;
    }

    //DFA Building Function
    //Takes a Set of States and Returns the States that can be reached via Epsilon transitions for each of those Individual states
    set<int> epsilon_closure(set<int>states){
        set<int> epsilonReachableStates;
        for (set<int>::iterator it = states.begin(); it != states.end(); ++it) {
            vector<trans>::iterator node_ptr;
            for(node_ptr = nfa_node_graph[*it].begin();node_ptr< nfa_node_graph[*it].end();node_ptr++){
                if(node_ptr->symbol == EPSILON_TRANSITION){
                    epsilonReachableStates.insert(node_ptr->destination);
                }
            }
        }
        epsilonReachableStates.insert(states.begin(),states.end());
        return epsilonReachableStates;
    }

    //DFA Building Function
    // Move takes a set of states T and input character a and returns all the states that can be reached on a given input character from all states in T.
    set<int> move(set<int>states,string symbol){
        set<int> movableStates;
        for (set<int>::iterator it = states.begin(); it != states.end(); ++it) {
            vector<trans>::iterator node_ptr;
            for(node_ptr = nfa_node_graph[*it].begin();node_ptr< nfa_node_graph[*it].end();node_ptr++){
                if(node_ptr->symbol == symbol){
                    movableStates.insert(node_ptr->destination);
                }
            }
        }
        return movableStates;
    }

    //---------------------------------------------------------------------------------------------------
};

void print_set(set<int> a){
    for(set<int>::iterator itr=a.begin();itr != a.end();++itr){
        cout << *itr << " ";
    }
    cout << endl;
}

//Create Base NFA constructions for Concatenation
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

//Create Base NFA constructions for Union
NFA unionNFAs(NFA a, NFA b){
    NFA result;

    result.set_nodes(a.get_node_count()+b.get_node_count()+2);

    for(int i=0;i<a.node_trans.size();i++){
        if(i==0){
            result.set_transition(0,1,EPSILON_TRANSITION);
        }
        transition newTrans = a.node_trans.at(i);
        result.set_transition(newTrans.vertex_start+1,newTrans.vertex_end+1,newTrans.trans_symbol);
        if(i== a.node_trans.size()-1){
            cout << "setting Epsilon Trans From "<< a.node_trans.at(i).vertex_end+1 << " " << a.get_node_count()+b.get_node_count()+1 << endl; 
            result.set_transition(a.node_trans.at(i).vertex_end+1,a.get_node_count()+b.get_node_count()+1,EPSILON_TRANSITION);
        }
    }

    for(int i=0;i<b.node_trans.size();i++){
        if(i==0){
            result.set_transition(0,a.get_node_count()+1,EPSILON_TRANSITION);
        }
        transition newTrans = b.node_trans.at(i);
        result.set_transition(newTrans.vertex_start+1 + a.get_node_count(),newTrans.vertex_end+1 + a.get_node_count(),newTrans.trans_symbol);
        if(i== b.node_trans.size()-1){
            cout << "setting Epsilon Trans From "<< b.node_trans.at(i).vertex_end+1+a.get_node_count() << " " << a.get_node_count()+b.get_node_count()+1 << endl;
            result.set_transition(b.node_trans.at(i).vertex_end+1+a.get_node_count(),a.get_node_count()+b.get_node_count()+1,EPSILON_TRANSITION);
        }
    }

    result.set_final_state(a.get_node_count()+b.get_node_count()+1);
    return result;
}

NFA KleeneNFA(NFA a){

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

void printDFA(NFA a){
    DFA_trans tran;
    cout << '\n';
    for(int i=0;i<a.dfa_transtions.size();i++){
        tran = a.dfa_transtions.at(i);
        cout<<"q"<<tran.renamed_vertex_start<<" --> q"<<tran.renamed_vertex_end<<" : Symbol - "<<tran.trans_symbol<<endl;    
    }
    cout<< "\n The final state are " <<a.dfa_final_states.size() <<endl;
    
    if(a.dfa_final_states.size() > 0){
        for(int i=0;i<a.dfa_final_states.size();i++){
            cout << "q" << a.dfa_final_states.at(i) << " ";
        }
    }
    cout<<endl;
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
                    // Handle Union Operator Here.
                    NFA resultNFA = unionNFAs(operand1,operand2);
                    evaluationStack.push(resultNFA);
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

void searchFile(NFA regexEvaluator, string fileName) {
    //string fileContent = readFile(fileName);
    int lineNumber = 1;
    string filePath = DEFAULT_FILE_PATH + fileName;
    ifstream file;
    string line;
    cout << "Testing File" << endl;
    file.open(filePath.c_str());
    if (file.is_open()) {
        while (getline(file, line)) {
            vector<matched_symbol> indexMatches = regexEvaluator.match_string(line);
            if (!indexMatches.empty()) {
                for (int i = 0; i < indexMatches.size(); i++) {
                    printOutput(fileName, lineNumber, indexMatches.at(i).start_position, indexMatches.at(i).token);
                }
            }
            lineNumber++;
        }
    } else {
        cout << "File Didn't Open" << endl;
    }
}

void printOutput(string fileName, int lineNumber, int startIndex,string patternMatched){
    cout << fileName + ", " << lineNumber << ", " << startIndex << ", "<<  static_cast<int>(patternMatched.length()) + startIndex << ": "+patternMatched << endl;
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

    NFA resultantNFA = postFixNFABuilder(postfixRegex);
    
    printNFA(resultantNFA);

    resultantNFA.convertToDFA();
    printDFA(resultantNFA);
    // for(int i=0;i<fileNames.size();i++){
    //     searchFile(resultantNFA,fileNames.at(i));
    // }



}