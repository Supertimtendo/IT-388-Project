/*
 * Time Series Class
 * IT 388
 */

#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

Class TimeSeries {
    private:
        int size;
        vector<double> series;

    public:
        //Constructor
        TimeSeries();

        //Deconstructor
        ~TimeSeries();

        //Create time series from input file
        void parseSeries(ifstream file);

        //Template match the series with a given template
        int[] matchSeries(vector<double> template);
}

#endif //TIMESERIES_H_