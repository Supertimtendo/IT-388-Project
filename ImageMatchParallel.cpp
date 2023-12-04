/*
Parallelized Time Series Program with MPI
compile - mpi++ -g -Wall -o series ImageMatchParallel.cpp
run - mpiexec -n 4 ./series <fileA> <fileB>
*/

#include "ImageMatch.h"
#include <vector>
#include <iostream>
#include <sstream>
#include <mpi.h>

using namespace std;

vector<vector<double>> imageVec;
vector<vector<double>> templateVec;

void parseImage(string fileName);

void parseTemplate(string fileName);

vector<double> matchImage(double** series, double** temp, int tempSize, int local_x, int local_y, int work);

int main(int argc, char* argv[]) {
    //MPI Initialize
    int rank, nproc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    //Other variables
    vector<vector<double>> global_Result;
    int bestPos;
    int minSAD = 10000000;

    if (argc != 3) {
        if (rank == 0)
            cout << "Correct usage: mpirun -n <num_processes> ./project <input_file> <template_file> \n";
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 0;
    }

    string fileName = argv[1];
    string templateName = argv[2];


    if(rank == 0){
        // Create series
        parseImage(fileName);

        //Create template
        parseTemplate(templateName);
    }

    double** serArr;
    serArr = new double*[imageVec.size()];
    for(int i=0;i<imageVec.size();i++){
        serArr[i] = new double[imageVec[0].size()];
        for(int j=0;j<imageVec[0].size();j++){
            serArr[i][j] = imageVec.at(i).at(j);
        }
    }

    double** tempArr;
    tempArr = new double*[templateVec.size()];
    for(int i=0;i<templateVec.size();i++){
        tempArr[i] = new double[templateVec[0].size()];
        for(int j=0;j<templateVec[0].size();j++){
            tempArr[i][j] = templateVec.at(i).at(j);
        }
    }

    int serSize = imageVec.size()*imageVec[0].size();
    int tempSize = templateVec.size()*templateVec[0].size();


    MPI_Bcast(&serSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tempSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(serArr, serSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(tempArr, tempSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //Match
    int work = (serSize - tempSize) / nproc;
    int local_x = work * rank;
    int local_y = work * rank;

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    vector<double> local_Result = matchImage(serArr, tempArr, tempSize, local_x, local_y, work);


    // Each process inserts its local vector into the global vector
    for (int i = 0; i < nproc; ++i) {
        if (rank == i) {
            global_Result.push_back(local_Result);
        }
        MPI_Barrier(MPI_COMM_WORLD);  // Ensure they wait their turn
    }

    free(serArr);
    free(tempArr);

    MPI_Barrier(MPI_COMM_WORLD);
    double elapsedTime = MPI_Wtime() - start;

    //Host gets the best vector
    if(rank == 0){
        for (int i = 0; i < global_Result.size(); i++){
            if(global_Result[i][1] < minSAD){
                minSAD = global_Result[i][1];
                bestPos = global_Result[i][0];
            }
        }
        cout << "Best Position: " << bestPos << endl;
        cout << "SAD Value: " << minSAD << endl;
        cout << "Time: " << elapsedTime << endl;
    }

    MPI_Finalize();
    return 0;
}


void parseImage(string fileName){
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

    imageVec = image;

    // Close the file
    file.close();
}

void parseTemplate(string fileName){
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

    templateVec = image;

    // Close the file
    file.close();
}

/**
 * Template match series and template
 * @return Returns array with position and SAD values
 */
vector<double> matchImage(double** series, double** temp, int tempSize, int local_x, int local_y, int work){
    //Stores rows, cols value for both arrays
    int baseRow = imageVec.size();
    int baseCol = imageVec[0].size();
    int tempRow = templateVec.size();
    int tempCol = templateVec[0].size();

    int xPos = -1;
    int yPos = -1;
    vector<double> arr = {0, 0, 0};

    //Temp max value
    double minSAD = tempSize * 10;
    double SAD = 0.0;
    double bestSAD = minSAD;

    //Loop through base image
    for(int x = local_x; x < local_x + work; x++){
        for(int y = local_y; y < local_y + work; y++){
            SAD = 0.0;
            //Loop through template image
            for(int j = 0; j < tempCol; j++){
                for(int i = 0; i < tempRow; i++){
                    int p_SearchIMG = imageVec[y+i][x+j];
                    int p_TemplateIMG = templateVec[i][j];

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