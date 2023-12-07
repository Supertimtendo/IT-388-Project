#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main() {
    srand(static_cast<unsigned int>(time(nullptr)));

    ofstream outFile("TimeSeries.txt");

    //Edit this loop to add more numbers
    //Dont forget to update the TimeSeriesParallelProgram.cpp if you do!
    for (int i = 0; i < 1000; i++){
        double num = rand()%100;
        num += (double)(rand() % 10) / 10.0;
        outFile << num << endl;
    }

    outFile.close();

    return 0;
}