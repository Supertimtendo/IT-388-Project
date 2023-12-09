//Sequential File

#include "ImageMatch.h"
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

ImageMatch::ImageMatch(){

}

ImageMatch::~ImageMatch(){

}

//Read a .txt file containing grayscale pixel values into an array
void ImageMatch::parseImage(string fileName){
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Unable to open the file." << endl;
        exit(1);
    }

    // Create a 2D vector to store integers
    vector<vector<int>> image;

    // Read each line from the file
    string line;
    while (getline(file, line)) {
        vector<int> row;
        istringstream iss(line);

        // Read integers from the line
        int num;
        while (iss >> num) {
            row.push_back(num);
        }

        // Add the row to the image
        image.push_back(row);
    }

    this->imageVec = image;

    // Close the file
    file.close();
}

//Read a .txt file containing grayscale pixel values into an array
void ImageMatch::parseTemplate(string fileName){
    ifstream file(fileName);

    if (!file.is_open()) {
        cout << "Error: Unable to open the file." << endl;
        exit(1);
    }

    // Create a 2D vector to store integers
    vector<vector<int>> image;

    // Read each line from the file
    string line;
    while (getline(file, line)) {
        vector<int> row;
        istringstream iss(line);

        // Read integers from the line
        int num;
        while (iss >> num) {
            row.push_back(num);
        }

        // Add the row to the image
        image.push_back(row);
    }

    this->templateVec = image;

    // Close the file
    file.close();
}

/**
 * Template match image and template
 * @return Returns array with position and SAD values
 */
vector<double> ImageMatch::matchImage(){
    //Stores rows, cols value for both arrays
    int baseRow = this->imageVec.size();
    int baseCol = this->imageVec[0].size();
    int tempRow = this->templateVec.size();
    int tempCol = this->templateVec[0].size();

    int xPos = -1;
    int yPos = -1;
    vector<double> arr = {0, 0, 0};

    //Temp max value
    double minSAD = 255 * this->imageVec.size();
    double SAD = 0.0;
    double bestSAD = minSAD;

    //Loop through base image
    for(int x = 0; x < baseCol - tempCol; x++){
        for(int y = 0; y < baseRow - tempRow; y++){
            SAD = 0.0;
            //Loop through template image
            for(int j = 0; j < tempCol; j++){
                for(int i = 0; i < tempRow; i++){
                    int p_SearchIMG = this->imageVec[y+i][x+j];
                    int p_TemplateIMG = this->templateVec[i][j];

                    SAD += abs(p_SearchIMG - p_TemplateIMG);
                }
                if(x % 10 == 0){
                    cout << SAD << endl;
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