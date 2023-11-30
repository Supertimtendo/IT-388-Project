//
// Created by Tim on 10/25/2023.
// ./project <input_file> <template_file> <type>
//

#include "project.h"

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

        ImageMatch imgMatch;
        imgMatch.parseImage(fileName);
        imgMatch.parseTemplate(fileName);

        double* results;
        results = imgMatch.matchImage();

        printf("Best Position for matching with template: X: %f, Y: %f\n", results[0], results[1]);
        printf("SAD value of best position: %f\n", results[2]);

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