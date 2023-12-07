/*
Parallelized Time Series Program with MPI
compile - mpic++ -g -Wall -o series TimeSeriesParallelProgram.cpp
run - mpiexec -n 4 ./series <fileA> <fileB>
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>

using namespace std;

int createSeries(string fileName, double*& data);

int parseTemplate(string fileName, double*& data);


void matchTemplate(double* series, double* temp, int tempSize, int local_x, int work, int& position, int& minSAD);

int main(int argc, char* argv[]){
    //MPI Initialize
    int rank, nproc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    //Other variables
    int bestPos;
    int minSAD = 10000000;

    if (argc != 3){
        if (rank == 0)
            cout << "Correct usage: mpirun -n <num_processes> ./project <input_file> <template_file> \n";
        MPI_Finalize();
        return 0;
    }

    //Variable setup
    string fileName = argv[1];
    string templateName = argv[2];
    double* serArr = new double[1000];
    double* tempArr = new double[1000];
    int serSize;
    int tempSize;

    if(rank == 0){
        // Create series
        serSize = createSeries(fileName, serArr);
        //Create template
        tempSize = parseTemplate(templateName, tempArr);
        cout << "Check 1" << endl;
    }

    MPI_Bcast(&serSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tempSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(serArr, serSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(tempArr, tempSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //Match
    int work = (serSize - tempSize) / nproc;
    int local_x = work * rank;

    int* positions = new int[nproc];
    int* minSADs = new int[nproc];
    int local_position;
    int local_minSAD;

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    matchTemplate(serArr, tempArr, tempSize, local_x, work, local_position, local_minSAD);
    cout << local_position << endl;

    MPI_Gather(&local_position, 1, MPI_INT, positions, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(&local_minSAD, 1, MPI_INT, minSADs, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double elapsedTime = MPI_Wtime() - start;

    //Host gets the best vector
    if(rank == 0){
        for (int i = 0; i < nproc; i++){
            if(minSADs[i] < minSAD){
                minSAD = minSADs[i];
                bestPos = positions[i];
            }
        }
        cout << "Best Position: " << bestPos << endl;
        cout << "SAD Value: " << minSAD << endl;
        cout << "Time: " << elapsedTime << endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;

}

 

int createSeries(string fileName, double* &data){
    ifstream infile(fileName);
    if(infile.fail()){
        cout << "File could not be opened\n" << endl;
        exit(1);
    }

    //string variable
    string val;
    int count = 0;

    //While loop to read all values into the series vector
    while(getline(infile, val))
    {
        double num = stod(val);
        data[count] = num;
        count++;
    }
    infile.close();
    return count;
}

int parseTemplate(string fileName, double*& data){
    ifstream infile(fileName);
    if(infile.fail()){
        cout << "File could not be opened\n" << endl;
        exit(1);
    }

    //string variable
    string val;
    int count = 0;

    //While loop to read all values into the series vector
    while(getline(infile, val))
    {
        double num = stod(val);
        data[count] = num;
        count++;
    }
    infile.close();
    return count;
}

void matchTemplate(double* series, double* temp, int tempSize, int local_x, int work, int& position, int& minSAD){
    position = -1;

    //Temp max value
    minSAD = tempSize * 1000;
    double SAD = 0.0;

    //Loop through series
    for(int i = local_x; i < local_x + work; i++){
        SAD = 0.0;
        //Loop through template
        for(int j = 0; j < tempSize; j++){
            SAD += abs(series[i+j] - temp[j]);
        }

        if(SAD < minSAD){
            minSAD = SAD;
            //Stores index start of best matched template (go til length of template)
            position = i + 1;
        }
    }
}