//
// Created by Tim on 11/29/2023.
//

#ifndef PROJECT_IMAGEMATCH_H
#define PROJECT_IMAGEMATCH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
//#include <opencv2/opencv.hpp>

using namespace std;

class ImageMatch {
    private:
        unsigned char** imageArr;
        unsigned char** templateArr;

    public:
        //Constructor
        ImageMatch();

        //Deconstructor
        ~ImageMatch();

        //Create time series from input file
        void parseImage(string fileName);

        //Create template series from input file
        void parseTemplate(string fileName);

        //Template match the series with a given template
        double* matchImage();
};


#endif //PROJECT_IMAGEMATCH_H
