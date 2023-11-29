//
// Created by Tim on 10/25/2023.
// ./project <input_file> <template_file> <type>
//

#include "project.h"
#include "TimeSeries.h"
#include <opencv2/opencv.hpp>

using namespace std;

int main(int argc, char **argv){
    if(argc != 4){
        cout<< "Correct usage: ./project <input_file> <template_file> <type>\n Type: 0 -> Time series\n1 -> Image";
		exit(1);
    }
    Project project;
    string fileName = argv[1];
	string templateName = argv[2];
    int type = atoi(argv[3]);

    //Check for invalid type
    if(type<0 || type>1){
        cout<< "Invalid type, 0 or 1 only";
        exit(1);
    }
    project.setType(type);
    project.inputFile = project.fileInput(fileName);
	project.templateFile = project.fileInput(templateName);

    int N;
    int nproc;
    int rank;
	/*
    MPI_Init(&argc, &argv);
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &nproc);
    MPI_Comm_rank(comm, &rank);

    MPI_Barrier(comm);
    double start = MPI_Wtime();
    */

	//Time series
    if(project.getType()==0){
		//Time Series creation
		TimeSeries timeSer;
		timeSer.parseSeries(project.inputFile);

		//Get template data
		timeSer.parseTemplate(project.templateFile);

		//Perform TemplateMatching
		double* results;
		results = timeSer.matchSeries();

		printf("Best Position for matching with template: %f\n", results[0]);
		printf("SAD value of best position: %f\n", results[1]);
	}

    //Image
    else{
		//Convert both files to grayscale
		Mat imageData = imread(fileName, IMREAD_GRAYSCALE);
		Mat imageTemplate = imread(templateName, IMREAD_GRAYSCALE);

		//Get files as arrays
		std::vector<uchar> imageVec(imageData.rows*imageData.cols*imageData.channels());
		imageVec = imageData.data;

		//Convert to 2d Array
		uchar imageArr[imageData.rows][imageData.cols];
		for(int i=0;i<imageData.rows;i++){
			for(int j=0;j<imageData.cols;j++){
				imageArr[i][j] = imageVec.at(i*j);
			}
		}


		std::vector<uchar> templateVec(imageTemplate.rows*imageTemplate.cols*imageTemplate.channels());
		templateVec = imageTemplate.data;

		//Convert to 2d array
		uchar templateArr[imageTemplate.rows][imageTemplate.cols];

		for(int i=0;i<imageTemplate.rows;i++){
			for(int j=0;j<imageTemplate.cols;j++){
				templateArr[i][j] = templateVec.at(i*j);
			}
		}
    }

    /*
	double end = MPI_Wtime() - start;

    cout<<"Time: "<<fabs(end);

    MPI_Finalize();
    */
	return 0;
}

/**
 * Opens, tests and returns file
 * @param fileName Name of file to open
 * @return Returns file
 */
ifstream Project::fileInput(string fileName){
	ifstream file(fileName);
    file.open(fileName);
    if(file.fail()){
        cout<< "File cannot be opened";
		exit(2);
    }
	return file;
}



/**
 * Processes a time series input
 * @return Returns 1D array
 */
int Project::timeSeriesMatch(double* input, double* temp){
	int position = -1;
    double SAD = 0.0;
	//Temp max value
	double minSAD = 10000;
	//Loop through input
	for(int i=0;i<=(sizeof(input) / sizeof(double) - sizeof(temp) / sizeof(double));i++){
		SAD = 0.0;
		//Loop through template
		for(int j=0;j<=sizeof(temp) / sizeof(double);j++){
			SAD += abs(input[i+j] - temp[j]);
		}

		if(minSAD > SAD){
			minSAD = SAD;
			//Stores index start of best matched template (go til length of template)
			position = i;
		}
	}
	cout<<"Best SAD "<<SAD;
	return position;
}

/**
 * Processes an image input
 * @return Returns 2D array
 */
int** Project::imageMatch(int** input, int** temp){


}