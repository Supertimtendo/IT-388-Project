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

vector<vector<int>> imageVec;
vector<vector<int>> templateVec;

void parseImage(string fileName);

void parseTemplate(string fileName);

vector<double> matchImage();

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

    //TODO Convert vectors to arrays
    double** serArr;
    double** tempArr;

    int serSize = imageVec.size();
    int tempSize = templateVec.size();


    MPI_Bcast(&serSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tempSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(serArr, serSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(tempArr, tempSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //Match
    int work = (serSize - tempSize) / nproc;
    int local_x = work * rank;

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    vector<double> local_Result = matchImage(serArr, tempArr, tempSize, local_x, work);


    // Each process inserts its local vector into the global vector
    for (int i = 0; i < nproc; ++i) {
        if (rank == i) {
            global_Result.push_back(local_Result);
        }
        MPI_Barrier(MPI_COMM_WORLD);  // Ensure they wait their turn
    }



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
vector<double> matchImage(double** series, double** temp, int tempSize, int local_x, int work){
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
    for(int x = 0; x < baseCol - tempCol; x++){
        for(int y = 0; y < baseRow - tempRow; y++){
            SAD = 0.0;
            //Loop through template image
            for(int j = 0; j < tempCol; j++){
                for(int i = 0; i < tempRow; i++){
                    int p_SearchIMG = imageVec[y+i][x+j];
                    int p_TemplateIMG = templateVec[i][j];

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