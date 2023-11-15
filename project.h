/*
 * Tim Buranicz
 * Template Matching Project
 * IT 388
 */

#ifndef PROJECT_PROJECT_H
#define PROJECT_PROJECT_H

#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

class Project {
private:
    int type;
public:
    ofstream inputFile;
    ofstream templateFile;
    Project();
    ~Project();
    ofstream fileInput(string fileName);
    void setType(int type){
        type = this->type;
    }
    int getType(){
        return type;
    }

    int timeSeriesMatch(int* input);
    int** imageMatch(int** input, int** temp);

};


#endif //PROJECT_PROJECT_H
