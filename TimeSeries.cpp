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
    while(getline(file, val, ' '))
    {
        double data = stod(val);
        this->series.push_back(data);
    }

    file.close();
}

int TimeSeries::matchTemplate(vector<double> template){
    int position = -1;

    //Temp max value
	double minSAD = 10000;

    //Loop through series
	for(int i = 0; i < template.size(); i++){
		double SAD = 0.0;
		//Loop through template
		for(int j = 0; j <= template.size(); j++){
			SAD += abs(this->series[i+j] - template[j]);
		}

		if(SAD < minSAD){
			minSAD = SAD;
			//Stores index start of best matched template (go til length of template)
			position = i;
		}
	}
	cout<<"Best SAD: "<<SAD;
	return position;
}