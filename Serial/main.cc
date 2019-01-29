#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <string>
#include <stack>
#include <queue> 
using namespace std;

void computeLPSArray(char* pattern, int patternLength, int* lps); 

/**
 *<summary>Check if a Character is an operator for a regular expression</summary> 
 *<param name="c">character to be evaluated</param> 
*/

bool isOperator(char c)
{
	if(c == '+' || c == '*')
		return true;

	return false;
}

/**
 *<summary>Check if a Character is an operand for a regular expression</summary> 
 *<param name="c">character to be evaluated</param> 
*/
bool isOperand(char c) 
{
	if(c >= '0' && c <= '9') return true;
	if(c >= 'a' && c <= 'z') return true;
	if(c >= 'A' && c <= 'Z') return true;
    if(c == '.') return true;
	return false;
}

/**
 *<summary>Find the Weight of the Operator to create the post fix notation</summary> 
 *<param name="inputOperator"> Operator whose weight needs to be evaluated for post fix notation evaluation.</param>
 *<return> The weight of the operator for determining precedence </return> 
*/
int getOperatorWeight(char inputOperator){
    int weight = 1;
    switch(inputOperator){
        case '*':
            weight=2;
            break;
        case '+':
            weight=0;
            break;
    }   
    return weight;
}

bool isHigherPresedence(char a, char b){
   return  getOperatorWeight(a) > getOperatorWeight(b) ? true : false;    
}


/**
 * <summary>Take a bunch of Filename Vectors and extract the contents of the files within those files.</summary>
 * <param name="fileNames"> A Constant Vector Reference Passed to the function. Constant because it will not change.</param>   
 * <returns> Vector Of Strings that have the contents of the file</returns>  
*/
vector <string> readFiles(const vector<string> & fileNames)
{
    //Use Vectors to Create an array of File Contents
    vector<string> files;
    //TODO : Might Change the string postion from here to inside the loop while executing in parallel. 
    string line;
    //Create A Constant Iterator to loop through the File Names Vector.
    vector<string>::const_iterator ptr; 
    for (ptr=fileNames.begin(); ptr < fileNames.end(); ptr++)
    {
        string fileContent = "";
        //As Reference is passed to the Function a pointer is used to reference of the actual index in the vector
        string fileLocation = "Files/"+ *ptr;
        ifstream myfile;
        myfile.open(fileLocation.c_str());
        //Open the File and Store the Contents in an Array. 
        if (myfile.is_open())
        {
            while (getline(myfile, line))
            {
                fileContent += line + '\n';
               // cout << line << '\n';
            }
            myfile.close();
            //Write the Content of the File to the Vector and pass the Vector back to the 
            files.push_back(fileContent);
        }
        else
        {
            cout << "Unable to open file" << endl;
        }
    }

    return files;
}

/**--------------------------------------- KMPS ALGORITHM IMPEMENTATION ENDS HERE---------------------------------------*/
/**
 * <summary>Implementation of the KMP Search Algorithm</summary>
 * <returns> Vector of the indexs matched during the search.</returns>  
*/
vector <int> patternSearch(char* pattern, char* txt) 
{ 
    vector <int> matchedIndexes;
	int patternLength = strlen(pattern); 
	int textLength = strlen(txt); 

	// create lps[] that will hold the longest proper prefix which is also suffix
	// values for pattern 
	int lps[patternLength]; 

	// Preprocess the pattern (calculate lps[] array) 
	computeLPSArray(pattern, patternLength, lps); 

	int i = 0; // index for txt[] 
	int j = 0; // index for pat[] 
	while (i < textLength) { 
		if (pattern[j] == txt[i]) { 
			j++; 
			i++; 
		} 
		if (j == patternLength) { 
			//printf("Found pattern at index %d ", i - j);
            matchedIndexes.push_back((i-j)); 
			j = lps[j - 1]; 
		} 

		// mismatch after j matches 
		else if (i < textLength && pattern[j] != txt[i]) { 
			// Do not match lps[0..lps[j-1]] characters, 
			// they will match anyway 
			if (j != 0) 
				j = lps[j - 1]; 
			else
				i = i + 1; 
		} 
	} 
} 

/**
 * <summary>Fills lps[] for given patttern pat[0..patternLength-1]. This Will be used by tht  </summary>
 * 
 * */
void computeLPSArray(char* pattern, int patternLength, int* lps) 
{ 
	// length of the previous longest prefix suffix 
	int longestPSLength = 0; 

	lps[0] = 0; // lps[0] is always 0 

	// the loop calculates lps[i] for i = 1 to M-1 
	int i = 1; 
	while (i < patternLength) { 
		if (pattern[i] == pattern[longestPSLength]) { 
			longestPSLength++; 
			lps[i] = longestPSLength; 
			i++; 
		} else{  
			if (longestPSLength != 0) { 
				longestPSLength = lps[longestPSLength - 1]; 
	        } else { 
				lps[i] = 0; 
				i++; 
			} 
		} 
	} 
} 

/**--------------------------------------- KMPS ALGORITHM IMPEMENTATION ENDS HERE---------------------------------------*/
//TODO : Fix the Regex Parsing Here. 
/**
 * <summary>Creates a Vector of strings to match for the Regular Expression Provided</summary>
 * <param name='expression'>Regualar expression of the type "a+be*" or "(ab)+(c)"</param> 
*/
//TODO : Unsure of the Vector Datastructure here. 
// vector<string> getStringsToMatch(string expression, int maxWordLength){
//     stack<char> operatorStack;
//     queue<char> postFixQueue;
//     vector <string> output;

//     cout << "Starting Post Fix Notation "+ expression << endl;
//     //Convert the expression From an Infix Notation to a Post Fix notation. 
//     for(int i=0;i<expression.length();i++){
//          cout << expression[i] << endl;
//         if(isOperand(expression[i])){
//             postFixQueue.push(expression[i]);
//         }
//         else if(isOperator(expression[i])){
//             while(!operatorStack.empty() && isHigherPresedence(operatorStack.top(),expression[i])){
//                 //
//                 postFixQueue.push(operatorStack.top()); 
//                 operatorStack.pop();
//             }
//             operatorStack.push(expression[i]);
//         }else if(expression[i] == '('){
//             operatorStack.push(expression[i]);
//         }else if(expression[i] == ')'){
//             while(!operatorStack.empty() && operatorStack.top()!='('){
//                 postFixQueue.push(operatorStack.top());
//                 operatorStack.pop();
//             }
//             //Removes the "(" from the Stack. 
//             operatorStack.pop();
//         }               
//     }
    
//     //Compelete Post Fix Queue created Here. 
//     while(!operatorStack.empty()){
//         postFixQueue.push(operatorStack.top());
//         operatorStack.pop();
//     }

    
//     //TODO: Creating the Vectors of Strings from post fix notation comes here 

//     return output;
// } 

void printVector(vector<int> const &input){
	for (int i = 0; i < input.size(); i++) {
		cout << input.at(i) << ' ';
	}
}

int main(int argc, char *argv[])
{   
    //Arguement 0 is the Name of the Binary so it is ignored.
    //First Arguement of the function is a regex to be Found Within the File. 
    string regularExpression = argv[1]; 
    vector <string> fileNames;
    
    //As the Number of Files that will be passed to the Code with be unknown a Vector is chosen. This is Inefficient. I can use strings Instead of Vectors. 
    for(int i=2;i<argc;i++){
        fileNames.push_back(argv[i]);
        //cout << argv[i] << endl;
    }
    //vector <string> fileContents = readFiles(fileNames);

    //TODO :Find the Largest word in a string. --> This is done so that the extent to which the * operator's evaluation needs to take place. 

    //TODO :TURN Regex Ex Into an array of strings that need to be evaluated. 
    
    //TODO:Match The File Contents Strings with the array of strings that were found. 

    // vector<string> matchingStrings = getStringsToMatch(regularExpression,10);

    // for(int i=0; i<fileContents.size(); ++i){
    //     cout << fileContents[i] << endl;
    // }
}