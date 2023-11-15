/* File: mpi_array.c
 Processing array using point-to-point communication
 
 Name:
 
 Activity 03 - IT 388 - Fall 2023
 */

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

vector<double> createSeries(string fileName, vector<double> data);

vector<double> parseTemplate(string fileName, vector<double> data);

int matchTemplate(vector<double> series, vector<double> temp);

int main(int argc, char* argv[]){
    if(argc != 4){
        cout<< "Correct usage: ./project <input_file> <template_file> <type>\n Type: 0 -> Time series\n1 -> Image";
		exit(1);
    }

    //Variable setup
    string fileName = argv[1];
	string templateName = argv[2];
    int type = atoi(argv[3]);

    vector<double> series;
    vector<double> temp;

    // Create series
    series = createSeries(fileName, series);

    //Create template
    temp = parseTemplate(templateName, temp);

    //Match
    int pos = matchTemplate(series, temp);
    cout << "Best Position: " << pos << endl;

}

vector<double> createSeries(string fileName, vector<double> data){
    ifstream infile(fileName);
    if(infile.fail()){
        cout << "File could not be opened\n" << endl;
        exit(1);
    }

    //string variable
    string val;

    //While loop to read all values into the series vector
    while(getline(infile, val))
    {
        double num = stod(val);
        data.push_back(num);
        cout << num << " ";
    }

    cout << endl;
    infile.close();
    return data;
}

vector<double> parseTemplate(string fileName, vector<double> data){
    ifstream infile(fileName);
    if(infile.fail()){
        cout << "File could not be opened\n" << endl;
        exit(1);
    }

    //string variable
    string val;

    //While loop to read all values into the series vector
    while(getline(infile, val))
    {
        double num = stod(val);
        data.push_back(num);
        cout << num << " ";
    }

    cout << endl;
    infile.close();
    return data;
}

int matchTemplate(vector<double> series, vector<double> temp){
    int position = -1;

    //Temp max value
	double minSAD = 10000;
    double SAD = 0.0;

    //Loop through series
	for(int i = 0; i < series.size() - temp.size(); i++){
		SAD = 0.0;
		//Loop through template
		for(int j = 0; j < temp.size(); j++){
			SAD += abs(series[i+j] - temp[j]);
		}
        cout << SAD << endl;

		if(SAD < minSAD){
			minSAD = SAD;
			//Stores index start of best matched template (go til length of template)
			position = i + 1;
		}
	}
    printf("Best SAD: %f\n", minSAD);
	return position;
}