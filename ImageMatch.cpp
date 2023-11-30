#include "ImageMatch.h"
#include <iostream>

using namespace std;

void ImageMatch::parseImage(string fileName){
    ifstream file(fileName);
    if(file.fail()){
        cout<< "File cannot be opened";
        exit(2);
    }
    Mat imageData = imread(fileName, IMREAD_GRAYSCALE);

    //Get files as arrays
    std::vector<unsigned char> imageVec(imageData.rows*imageData.cols*imageData.channels());
    imageVec = imageData.data;

    //Convert to 2d Array
    this->imageArr[imageData.rows][imageData.cols];
    for(int i=0;i<imageData.rows;i++){
        for(int j=0;j<imageData.cols;j++){
            this->imageArr[i][j] = imageVec.at(i*j);
        }
    }

}

void ImageMatch::parseTemplate(string fileName){
    ifstream file(fileName);
    if(file.fail()){
        cout<< "File cannot be opened";
        exit(2);
    }

    Mat imageTemplate = imread(fileName, IMREAD_GRAYSCALE);

    //Get files as arrays
    std::vector<unsigned char> templateVec(imageTemplate.rows*imageTemplate.cols*imageTemplate.channels());
    templateVec = imageTemplate.data;

    //Convert to 2d Array
    this->templateArr[imageTemplate.rows][imageTemplate.cols];
    for(int i=0;i<imageTemplate.rows;i++){
        for(int j=0;j<imageTemplate.cols;j++){
            this->templateArr[i][j] = templateVec.at(i*j);
        }
    }
}

/**
 * Template match series and template
 * @return Returns array with position and SAD values
 */
double* ImageMatch::matchImage(){
    //Stores rows, cols value for both arrays
    int baseRow = sizeof this->imageArr / sizeof this->imageArr[0];
    int baseCol = sizeof this->imageArr[0] / sizeof this->imageArr[0][0];
    int tempRow = sizeof this->templateArr / sizeof this->templateArr[0];
    int tempCol = sizeof this->templateArr[0] / sizeof this->templateArr[0][0];

    int xPos = -1;
    int yPos = -1;
    double arr[] = {0, 0, 0};

    //Temp max value
    double minSAD = 10000;
    double SAD = 0.0;
    double bestSAD = minSAD;

    //Loop through base image
    for(int x = 0; x <= baseCol - tempCol; x++){
        for(int y = 0; y<= baseRow - tempRow; y++){
            SAD = 0.0;
            //Loop through template image
            for(int j = 0; j < tempCol; j++){
                for(int i = 0; i < tempRow; i++){
                    int p_SearchIMG = this->imageArr[y+i][x+j];
                    int p_TemplateIMG = this->templateArr[i][j];

                    SAD += abs(p_SearchIMG - p_TemplateIMG);
                }
            }

            if(SAD < minSAD){
                minSAD = SAD;
                xPos = x;
                yPos = y;
                bestSAD = SAD;
            }
        }

    }

    arr[0] = xPos;
    arr[1] = yPos;
    arr[2] = bestSAD;
    return arr;
}