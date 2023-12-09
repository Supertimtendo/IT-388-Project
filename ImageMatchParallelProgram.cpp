/*
Parallelized Image Matching Program with MPI
compile - mpi++ -g -Wall -o image ImageMatchParallel.cpp
run - mpiexec -n 4 ./image <fileA> <fileB>
*/
#include "ImageMatch.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <mpi.h>

using namespace std;

vector<vector<double>> imageVec;
vector<vector<double>> templateVec;

//Read a .txt file containing grayscale pixel values into the image vector
void parseImage(string fileName);

//Read a .txt file containing grayscale pixel values into an template vector
void parseTemplate(string fileName);

/**
 * Template match image and template
 * @return Returns array with position and SAD values
 */
vector<double> matchImage(int local_x, int work);

int main(int argc, char* argv[]) {
   // MPI Initialize
   int rank, nproc;
   MPI_Init(&argc, &argv);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Comm_size(MPI_COMM_WORLD, &nproc);

   // Other variables
   vector<vector<double>> global_Result;
   int bestPos;
   double minSAD = 10000000;

   if (argc != 3) {
       if (rank == 0)
           cout << "Correct usage: mpirun -n <num_processes> ./project <input_file> <template_file> \n";
       MPI_Abort(MPI_COMM_WORLD, 1);
       MPI_Finalize();
       return 0;
   }

    //File names
   string fileName = argv[1];
   string templateName = argv[2];

   if (rank == 0) {
       // Create series
       parseImage(fileName);

       // Create template
       parseTemplate(templateName);
   }

   // broadcast imagevec and templatevec to all processes
   int rows, cols, tempRows, tempCols;
   if (rank==0)
   {
       rows = imageVec.size();
       cols = imageVec[0].size();
   }
   MPI_Bcast(&rows, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&cols, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&tempRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&tempCols, 1, MPI_INT, 0, MPI_COMM_WORLD);

   if (rank!=0)
   {
       imageVec.resize(rows, vector<double>(cols,0));
       templateVec.resize(tempRows, vector<double>(tempCols,0));
   }
  
   for (int i = 0; i < rows; i++)
   {
   MPI_Bcast(imageVec[i].data(), cols, MPI_DOUBLE, 0, MPI_COMM_WORLD);      
   }
   for (int i = 0; i < tempRows; i++)
   {
   MPI_Bcast(templateVec[i].data(), tempCols, MPI_DOUBLE, 0, MPI_COMM_WORLD);      
   }
  
   int serSize = imageVec.size() * imageVec[0].size();
   int tempSize = templateVec.size() * templateVec[0].size();

   MPI_Bcast(&serSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
   MPI_Bcast(&tempSize, 1, MPI_INT, 0, MPI_COMM_WORLD);

   double* serArr = new double[serSize];
   double* tempArr = new double[tempSize];

   if (rank == 0) {
       // Flatten the series and template vectors for broadcasting
       int idx = 0;
       for (int i = 0; i < imageVec.size(); ++i) {
           for (int j = 0; j < imageVec[0].size(); ++j) {
               serArr[idx++] = imageVec[i][j];
           }
       }

       idx = 0;
       for (int i = 0; i < templateVec.size(); ++i) {
           for (int j = 0; j < templateVec[0].size(); ++j) {
               tempArr[idx++] = templateVec[i][j];
           }
       }
   }

   MPI_Bcast(serArr, serSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
   MPI_Bcast(tempArr, tempSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);

   // Match
   int work = (serSize - tempSize) / nproc;
   int remainder = (serSize-tempSize) % nproc;
   int local_x = work * rank;

   MPI_Barrier(MPI_COMM_WORLD);
   double start = MPI_Wtime();

   vector<double> local_Result = matchImage(local_x, work);

   // Each process inserts its local vector into the global vector
   for (int i = 0; i < nproc; ++i) {
       if (rank == i) {
           global_Result.push_back(local_Result);
       }
       MPI_Barrier(MPI_COMM_WORLD); // Ensure they wait their turn
   }

   if (rank==0)
   {
       delete[] serArr;
       delete[] tempArr;
   }

   MPI_Barrier(MPI_COMM_WORLD);
   double elapsedTime = MPI_Wtime() - start;

   // Host gets the best vector
   if (rank == 0) {
       for (int i = 0; i < global_Result.size(); i++) {
           if (global_Result[i][2] < minSAD) {
               minSAD = global_Result[i][2];
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

void parseImage(string fileName) {
   ifstream file(fileName);

   if (!file.is_open()) {
       cout << "Error: Unable to open the file." << endl;
       MPI_Abort(MPI_COMM_WORLD, 1);
   }

   // Create a 2D vector to store doubles
   vector<vector<double>> image;

   // Read each line from the file
   string line;
   while (getline(file, line)) {
       vector<double> row;
       istringstream iss(line);

       // Read doubles from the line
       double num;
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

void parseTemplate(string fileName) {
   ifstream file(fileName);

   if (!file.is_open()) {
       cout << "Error: Unable to open the file." << endl;
       MPI_Abort(MPI_COMM_WORLD, 1);
   }

   // Create a 2D vector to store doubles
   vector<vector<double>> image;

   // Read each line from the file
   string line;
   while (getline(file, line)) {
       vector<double> row;
       istringstream iss(line);

       // Read doubles from the line
       double num;
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
vector<double> matchImage(int local_x, int work) {
   // Stores rows, cols value for both arrays
   int baseRow = imageVec.size();
   int baseCol = imageVec[0].size();
   int tempRow = templateVec.size();
   int tempCol = templateVec[0].size();

   int xPos = -1;
   int yPos = -1;
   vector<double> arr = {0, 0, 0};


   // Temp max value
   double minSAD = 255 * imageVec.size();
   double SAD = 0.0;
   double bestSAD = minSAD;

   // Loop through base image
   for (int x = local_x; x < local_x + (work/baseCol); x++) {
       for (int y = 0; y < baseCol; y++){
           SAD = 0.0;
           // Loop through template image
           for (int j = 0; j < tempRow; j++) {
               for (int i = 0; i < tempCol; i++) {
                   if (y+i<baseCol&&x+j<baseRow)
                   {
                   int p_SearchIMG = imageVec[x + j][y + i];
                   int p_TemplateIMG = templateVec[j][i];




                   SAD += abs(p_SearchIMG - p_TemplateIMG);
                   }
               }
           }

           if (SAD < minSAD) {
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