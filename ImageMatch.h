// Sequential File

#ifndef IMAGEMATCH_H
#define IMAGEMATCH_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
//#include <opencv2/opencv.hpp>

using namespace std;

class ImageMatch {
    private:
        vector<vector<int>> imageVec;
        vector<vector<int>> templateVec;

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
        vector<double> matchImage();

        vector<vector<int>> getImage(){ return this->imageVec;};

        vector<vector<int>> getTemplate(){ return this->templateVec; };
};


#endif //IMAGEMATCH_H
