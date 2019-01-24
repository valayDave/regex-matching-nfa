#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <string>
using namespace std;

/**
 * <summary>Take a bunch of Filename Vectors and extract the contents of the files within those files.</summary>
 * <param name="fileNames"> A Constant Vector Reference Passed to the function. Constant because it will not change.</param>   
 * <returns> Vector Of Strings that have the contents of the file</returns>  
*/
std::vector <string> readFile(const vector<string> & fileNames)
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
    vector <string> fileContents = readFile(fileNames);
    for(int i=0; i<fileContents.size(); ++i){
        cout << fileContents[i] << endl;
    }
}