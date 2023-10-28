//
// Created by Tim on 10/25/2023.
//

#include "project.h"

int main(int argc, char **argv){
    if(argc<2){
        cout<< "Correct usage: ./project <filename> <type>\n Type: 0 -> Time series\n1 -> Image";
    }
    Project project;
    string fileName = argv[0];
    int type = atoi(argv[1]);

    //Check for invalid type
    if(type<0 || type>1){
        cout<< "Invalid type, 0 or 1 only";
        exit(1);
    }
    project.setType(type);
    project.file = project.inputFile(fileName);

    //Time series
    if(project.getType()==0){

    }

    //Image
    else{

    }

}

/**
 * Opens, tests and returns file
 * @param fileName Name of file to open
 * @return Returns file
 */
ofstream Project::inputFile(string fileName){
    file.open(fileName, ofstream::out);
    if(file.fail()){
        cout<< "File cannot be opened";
    }
}



/**
 * Processes a time series input
 * @return Returns 1D array
 */
int* Project::timeSeriesMatch(int* input){

}

/**
 * Processes an image input
 * @return Returns 2D array
 */
int** Project::imageMatch(int** input){

}

/**
 * Parses input file for time series data
 * @param file File input
 * @return Returns 1D of data
 */
int* Project::parseTimeSeries(ofstream file){

}

/**
 * Parses input file for image data
 * @param file File input
 * @return Returns 2D of data
 */
int** Project::parseImage(ofstream file){

}