#include "timeSeries.h"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

void TimeSeries::parseSeries(ifstream file){
    if(file.fail()){
        cout << "File could not be opened\n" << endl;
        exit(1);
    }

    //string variable
    string val;

    //While loop to read all values into the series vector
    while(getline(file, val))
    {
        double data = stod(val);
        this->series.push_back(data);
    }

    file.close();
}

void TimeSeries::parseTemplate(ifstream file){
    if(file.fail()){
        cout << "File could not be opened\n" << endl;
        exit(1);
    }

    //string variable
    string val;

    //While loop to read all values into the series vector
    while(getline(file, val))
    {
        double data = stod(val);
        this->template.push_back(data);
    }

    file.close();
}

/**
 * Template match series and template
 * @return Returns array with position and SAD values
 */
double[] TimeSeries::matchTemplate(){
    int position = -1;
    double[] arr = {1000, 1000};

    //Temp max value
	double minSAD = 10000;
    double SAD = 0.0;

    //Loop through series
	for(int i = 0; i < this->series.size() - this->template.size(); i++){
		SAD = 0.0;
		//Loop through template
		for(int j = 0; j <= template.size(); j++){
			SAD += abs(this->series[i+j] - this->template[j]);
		}

		if(SAD < minSAD){
			minSAD = SAD;
			//Stores index start of best matched template (go til length of template)
			position = i;
		}
	}
    printf("Best SAD: %f", SAD);
    arr[0] = position;
    arr[1] = SAD;
	return position;
}