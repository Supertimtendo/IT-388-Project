/*
Parallelized Time Series Program with MPI
compile - mpi++ -g -Wall -o series TimeSeriesParallelProgram.cpp
run - mpiexec -n 4 ./series <fileA> <fileB>
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>

using namespace std;

int createSeries(string fileName, double*& data);

int parseTemplate(string fileName, double*& data);

vector<int> matchTemplate(double* series, double* temp, int tempSize, int local_x, int work);

int main(int argc, char* argv[]){
    //MPI Initialize
    int rank, nproc;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);

    //Other variables
    vector<vector<int>> global_Result;
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

    double* serArr;
    double* tempArr;

    int serSize;
    int tempSize;

    if(rank == 0){
        // Create series
        serSize = createSeries(fileName, serArr);

        //Create template
        tempSize = parseTemplate(templateName, tempArr);
    }

    MPI_Bcast(&serSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&tempSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(serArr, serSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(tempArr, tempSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    //Match
    int work = (serSize - tempSize) / nproc;
    int local_x = work * rank;

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    vector<int> local_Result = matchTemplate(serArr, tempArr, tempSize, local_x, work);

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

vector<int> matchTemplate(double* series, double* temp, int tempSize, int local_x, int work){
    int position = -1;

    //Temp max value
	double minSAD = tempSize * 10;
    double SAD = 0.0;

    //return value
    vector<int> result = {0, 0};

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
    result[0] = position;
    result[1] = int(minSAD);
	return result;
}

