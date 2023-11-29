/*
 * Time Series Class
 * IT 388
 */

#ifndef TIMESERIES_H_
#define TIMESERIES_H_

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class TimeSeries {
    private:
        int size;
        vector<double> series;
        vector<double> temp;

    public:
        //Constructor
        TimeSeries();

        //Deconstructor
        ~TimeSeries();

        //Create time series from input file
        void parseSeries(ifstream file);

        //Create template series from input file
        void parseTemplate(ifstream file);

        //Template match the series with a given template
        int matchSeries();
};

#endif //TIMESERIES_H_