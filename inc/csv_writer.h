#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "acc_sensor.h"

class CSVWriter
{
public:
    CSVWriter(const std::string &filename, size_t bufferSize = 1024 * 1024);
    ~CSVWriter();

    void writeRow(const std::vector<std::string> &row);
    void writeValues(double time, Vector_3D vec_xl, Vector_3D vec_g);
    void writeLine(std::string line);
    void flushBuffer();

private:
    std::ofstream outFile;
    std::string filename;
    size_t bufferSize;
    char *buffer;
    size_t bufferIndex;
};

#endif // CSVWRITER_H