# IT-388-Project

ImageMatch.cpp - Implementation file for all Image Template matching methods.
ImageMatch.h - Header file for all Image Template matching methods and variables.
ImageProgram.cpp - Main that runs the Image Template matching - requires a image and a template image to run.
ImageMatchParallelProgram.cpp - Parallelized image matching program.

TimeSeriesProgram.cpp - Currently in use, contains all methods required for template matching, requires a time series and a template series to run.
TimeSeriesParallelProgram.cpp - Parallelized Time Series Program.

Compiling and running Time Series:
TimeSeriesProgram.cpp
 Compile: g++ TimeSeriesProgram.cpp
 Run: ./a.out <TimeSeries> <TemplateSeries>

TimeSeriesParallelProgram.cpp
 Compile: mpic++ -g -Wall -o series TimeSeriesParallelProgram.cpp
 Run: mpiexec -n 4 ./series <fileA> <fileB>

Compiling and running Image Matching:
ImageMatchProgram.cpp
 Compile: g++ ImageMatch.cpp ImageMatchProgram.cpp
 Run: ./a.out <ImageFile.txt> <ImageTemplate.txt>

ImageMatchParallelProgram.cpp
 Compile - mpic++ -g -Wall -o image ImageMatchParallelProgram.cpp
 Run - mpiexec -n <nproc> ./image <fileA> <fileB>