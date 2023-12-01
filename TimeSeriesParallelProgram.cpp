/*
Parallelized Time Series Program with MPI
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <mpi.h>

using namespace std;

vector<double> createSeries(string fileName, vector<double> data);

vector<double> parseTemplate(string fileName, vector<double> data);

vector<int> matchTemplate(vector<double> series, vector<double> temp, int local_x, int work);

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
        if (world_rank == 0)
            cout << "Correct usage: mpirun -n <num_processes> ./project <input_file> <template_file> \n";
        MPI_Finalize();
        exit(1);
    }

    //Variable setup
    string fileName = argv[1];
	string templateName = argv[2];

    vector<double> series;
    vector<double> temp;

    if(rank == 0){
        // Create series
        series = createSeries(fileName, series);

        //Create template
        temp = parseTemplate(templateName, temp);
    }

    MPI_Bcast(&series, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&temp, 1, MPI_INT, 0, MPI_COMM_WORLD);

    //Match
    int work = (series.size() - temp.size()) / nproc;
    int local_x = work * rank;

    MPI_Barrier(MPI_COMM_WORLD);
    double start = MPI_Wtime();

    vector<int> local_Result = matchTemplate(series, temp, local_x, work);

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

vector<int> matchTemplate(vector<double> series, vector<double> temp, int local_x, int work){
    int position = -1;

    //Temp max value
	int minSAD = temp.size() * 255;
    int SAD = 0.0;

    //return value
    vector<int> result = {position, minSAD};

    //Loop through series
	for(int i = local_x; i < local_x + work; i++){
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
    result[0] = position;
    result[1] = minSAD;
	return result;
}