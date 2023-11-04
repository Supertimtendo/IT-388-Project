/*
 * Tim Buranicz
 * Template Matching Project
 * IT 388
 */

#ifndef PROJECT_PROJECT_H
#define PROJECT_PROJECT_H

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

class Project {
private:
    int type;
public:
    ofstream file;
    Project();
    ~Project();
    ofstream inputFile(string fileName);
    void setType(int type){
        type = this->type;
    }
    int getType(){
        return type;
    }

    int* parseTimeSeries(ofstream file);
    int** parseImage(ofstream file);

    int* timeSeriesMatch(int* input);
    int** imageMatch(int** input);
    int** grayScaleConvert(int** input);

};


#endif //PROJECT_PROJECT_H
