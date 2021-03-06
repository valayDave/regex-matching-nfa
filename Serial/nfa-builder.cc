#include <bits/stdc++.h>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <stdlib.h>
#include <string>
//#include <chrono>

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

struct dfa_graph_trans {
    bool trans_found;
    int destination;
    string symbol;
    dfa_graph_trans(){
        trans_found = false;
    }
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

string DEFAULT_FILE_PATH = "";
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
    vector< vector<dfa_graph_trans> > dfa_node_graph; //dfa_node_graph[STATE_NUMBER] --> The Vector Inside this contains the transitions available from this State 
    vector<DFA_trans> dfa_transtions;
    vector<DFA_trans_mark> Dstates;
    vector<int> dfa_final_states; //The Final States Here are the renamed_vertexes present in the DFA Datastructure.
    set<int> dfa_states; // Contains the DFA states
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

    bool check_for_final_state(int state){
        vector<int>::iterator ptr;
        for(ptr=final_states.begin();ptr< final_states.end();ptr++){
            if(state == *ptr){
                return true;
            }
        }
        return false;
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------
    
    //-----------------------------------DFA BUILDING METHODS----------------------------------------------------------------
    
    
    void convert_to_dfa(){
        construct_nfa_graph();
        construct_alphabet();
        set<int> start_state;start_state.insert(0);
        //Take Epsilon Closure of 0 State.
        set<int> start_states = epsilon_closure(start_state);
        insert_to_d_states(start_states);
        //Subset Construction Algorithm From an NFA.
        do{
            set<int> currentStateIds = get_unmarked_state(); // This Method Also marks that StateId
            for (set<string>::iterator symbol=alphabet.begin();symbol != alphabet.end();++symbol) {
                //Perform the Move Operation to find the Reachable Nodes.
                set<int> newStateIds = move(currentStateIds,*symbol);
                //Perform Epsilon Closure of the move OP to create the consequent state in the DFA.
                set<int> epsilson_trans_ids = epsilon_closure(newStateIds);
                if(!epsilson_trans_ids.empty()){
                    if(is_new_dfa_state(epsilson_trans_ids)){
                        insert_to_d_states(epsilson_trans_ids);
                    }
                }
                if(!newStateIds.empty()){
                    DFA_trans newState;
                    newState.vertex_start = currentStateIds;
                    newState.vertex_end = epsilson_trans_ids;
                    newState.renamed_vertex_start = get_d_state_id(currentStateIds); //TODO Get a way to get the new Renamed DFA Counter.
                    newState.renamed_vertex_end = get_d_state_id(epsilson_trans_ids);
                    newState.trans_symbol = *symbol;
                    dfa_transtions.push_back(newState);
                }
            }
        }while(any_umarked_states());
        //Set the Final States Here.
        for(vector<DFA_trans_mark>::iterator ptr=Dstates.begin();ptr < Dstates.end();ptr++){
            dfa_states.insert(ptr->renamed_vertex_id);
            for(set<int>::iterator state_ptr = ptr->vertex.begin();state_ptr != ptr->vertex.end();++state_ptr){
                if(check_for_final_state(*state_ptr)){
                    //Add the Name of the Renamed VertexIds to the Vertex of final state
                    dfa_final_states.push_back(ptr->renamed_vertex_id);
                    break;
                }
            }
        }
        construct_dfa_graph();
    }

    void construct_dfa_graph(){
        vector<DFA_trans>::iterator ptr;
        dfa_node_graph.resize(dfa_states.size());
        //Iterate through the node transitions and construct the graph 
        for(ptr = dfa_transtions.begin();ptr < dfa_transtions.end();ptr++){
            dfa_graph_trans node_connect;
            node_connect.trans_found = true;
            node_connect.destination = ptr->renamed_vertex_end;
            node_connect.symbol = ptr->trans_symbol;
            dfa_node_graph[ptr->renamed_vertex_start].push_back(node_connect);
        }
    }

    //For a particular State in the DFA yield the available transition
    dfa_graph_trans available_dfa_state_transitions(int state, string transition_symbol) {
        dfa_graph_trans available_transition;
        vector<dfa_graph_trans>::iterator state_ptr;
        for (state_ptr = dfa_node_graph[state].begin(); state_ptr < dfa_node_graph[state].end(); state_ptr++) {
            //State Has an Allowed Transtion or An Epsilon Transtion than send that to the available transtions.
            if (state_ptr->symbol == transition_symbol) {
                available_transition = *state_ptr;
                return available_transition;
            }
        }
        return available_transition;
    }

    //String Matching Algorithm Using the DFA Graph.
    vector<matched_symbol> match_string(string text) {
        vector<matched_symbol> symbols;
        symbols = traverse_dfa_graph(text,"",0,0,symbols);
        return symbols;
    }

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
    vector<matched_symbol> traverse_dfa_graph(string text, string buffer, int currentCharPosition, int currentState, vector<matched_symbol> symbols) {
        //Crossed the Length of the String so Return back to the main function.
        if (currentCharPosition >= text.length()) {
            return symbols;
        }
        string token = string(1, text[currentCharPosition]); //Token is the Individual Character that needs to be evaluated.
        if (check_for_dfa_final_state(currentState)) {
            if(buffer != ""){
                matched_symbol symbol;
                symbol.start_position = currentCharPosition;
                symbol.token = buffer;
                symbols.push_back(symbol);
            }
        }
        //Available Transitions from the current Node.
        dfa_graph_trans available_transtion = available_dfa_state_transitions(currentState, token);
        if (available_transtion.trans_found) { // available_transtion.trans_found initialised when the graph is created.
            // If one of the available transtions is a Final State then We need to print the Token we have found.
                int newCharPosition = ++currentCharPosition;;
                buffer += token;
                int newState = available_transtion.destination;
                return traverse_dfa_graph(text, buffer, newCharPosition, newState, symbols);
        } else {
            int newCharPosition = currentCharPosition;
            //This is done Because a string can match at a state is and stop matching at another state which may be the final state.
            if(currentState ==0){
                 newCharPosition = ++currentCharPosition;
            }
            //Reset the state back to 0 So that New characters can be traversed through this.
            int newStartState = 0;
            string newBuffer = "";
            return traverse_dfa_graph(text, newBuffer, newCharPosition, newStartState, symbols);
        }
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
        //Currently state transitions are only taking place from startStates to states directly reached via epsilon from that state.
        //Epsilon transitons from states which have further epsilon trans from those states were not considered ealier. Added as the tweek to correct the algorithm.
        set<int> epsilonReachableStates = states;
        for (set<int>::iterator it = epsilonReachableStates.begin(); it != epsilonReachableStates.end(); ++it) {
            vector<trans>::iterator node_ptr;
            for(node_ptr = nfa_node_graph[*it].begin();node_ptr< nfa_node_graph[*it].end();node_ptr++){
                if(node_ptr->symbol == EPSILON_TRANSITION){
                    epsilonReachableStates.insert(node_ptr->destination);
                }
            }
        }
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

//UTILITY FN
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
            result.set_transition(b.node_trans.at(i).vertex_end+1+a.get_node_count(),a.get_node_count()+b.get_node_count()+1,EPSILON_TRANSITION);
        }
    }

    result.set_final_state(a.get_node_count()+b.get_node_count()+1);
    return result;
}


//NFA Created On Basis Of :: https://medium.com/@DmitrySoshnikov/building-a-regexp-machine-part-2-finite-automata-nfa-fragments-5a7c5c005ef0
NFA KleeneNFA(NFA a){
    NFA result;
    result.set_nodes(a.get_node_count()+2);
    result.set_transition(0,1,EPSILON_TRANSITION);
    
    for(int i=0;i<a.node_trans.size();i++){
        transition newTrans = a.node_trans.at(i);
        result.set_transition(newTrans.vertex_start+1,newTrans.vertex_end+1,newTrans.trans_symbol);    
    }

    result.set_transition(a.get_node_count(), a.get_node_count() + 1, EPSILON_TRANSITION);
	result.set_transition(a.get_node_count(), 1, EPSILON_TRANSITION);
	result.set_transition(0, a.get_node_count() + 1, EPSILON_TRANSITION);

	result.set_final_state(a.get_node_count() + 1);

    return result;
}

//This is to Print the Entire NFA so that There is some Reference on what is getting Constructed 
void printNFA(NFA a){//UTILITY FN
    transition tran;
    cout << '\n';
    for(int i=0;i<a.node_trans.size();i++){
        tran = a.node_trans.at(i);
        cout<<"q"<<tran.vertex_start<<" --> q"<<tran.vertex_end<<" : Symbol - "<<tran.trans_symbol<<endl;    
    }
    cout<<"\nThe final state is q"<<a.get_final_state()<<endl;
}

void printDFA(NFA a){//UTILITY FN
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
                //Evaluate the Kleene star Operator Here.
                NFA operand = evaluationStack.top();
                evaluationStack.pop();
                evaluationStack.push(KleeneNFA(operand));                
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
        }else if(isOperand(regex[i]) && isOperand(regex[i-1])){ // eg : "aa"
            replacedRegex +=('+');
            replacedRegex +=(regex[i]);
        }else if(regex[i] == ')' && (regex[i+1]=='(' || isOperand(regex[i+1]))){ // eg1 : ")a"  eg2 : ")("
            replacedRegex +=(regex[i]);
            replacedRegex +=('+');
        }else if(isOperand(regex[i]) && regex[i+1] =='(' ){ //eg :"r("
            replacedRegex +=(regex[i]);
            replacedRegex +=('+');
        }else{
            if(isOperator(regex[i])){
                if(i!=regex.length()-1){
                    if(isOperand(regex[i+1]) && !checkBinaryOperation(regex[i])){ //eg: "*w"
                        replacedRegex+= regex[i];
                        replacedRegex+= '+';
                    }else{
                        replacedRegex+= regex[i];
                    }
                }else{
                    replacedRegex+= regex[i];
                }
            }else{
                if(regex[i] =='(' && isOperand(regex[i-1])){ //eg : "r("
                    replacedRegex +=('+');
                    replacedRegex +=(regex[i]);
                }else{
                    replacedRegex+= regex[i];                
                }
            }
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
    file.open(filePath.c_str());    
    if (file.is_open()) {
        while (getline(file, line)) {
            if(line.length() != 0){
                vector<matched_symbol> indexMatches = regexEvaluator.match_string(line);
                if (!indexMatches.empty()) {
                    for (int i = 0; i < indexMatches.size(); i++) {
                        printOutput(fileName, lineNumber, indexMatches.at(i).start_position, indexMatches.at(i).token);
                    }
                }
            }
            lineNumber++;
        }
    } else {
        cout << "File Didn't Open" << endl;
    }
}

void printOutput(string fileName, int lineNumber, int startIndex,string patternMatched){
    if(patternMatched.length() == 1){
        cout << fileName + ", " << lineNumber << ", " << startIndex  << ", "<<  startIndex << ": "+patternMatched << endl;
    }else{
        cout << fileName + ", " << lineNumber << ", " << startIndex -static_cast<int>(patternMatched.length())+1 << ", " <<  startIndex << ": "+patternMatched << endl;
    }
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
    //cout << "Starting Post Fix Notation " + expression << endl;
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

int main(int argc, char* argv[]){
    // Arguement 0 is the Name of the Binary so it is ignored.
    // First Arguement of the function is a regex to be Found Within the File.
    string regularExpression = argv[1];
    vector<string> fileNames;

    // As the Number of Files that will be passed to the Code with be unknown a
    // Vector is chosen. This is Inefficient. I can use strings Instead of
    // Vectors.
    for (int i = 2; i < argc; i++) {
        fileNames.push_back(argv[i]);
    }
    string newRegex = changeRegexOperators(regularExpression);
    string postfixRegex = convertRegexToPostfix(newRegex);
    NFA resultantNFA = postFixNFABuilder(postfixRegex);
    resultantNFA.convert_to_dfa();
    //chrono::steady_clock::time_point begin = chrono::steady_clock::now();
    for(int i=0;i<fileNames.size();i++){
        searchFile(resultantNFA,fileNames.at(i));
    }
    //chrono::steady_clock::time_point end = chrono::steady_clock::now();
    //cout << "TIME TAKEN BY THE CODE : " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << endl;
}