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
    CSVWriter(const std::string &filename);
    ~CSVWriter();

    void writeRow(const std::vector<std::string> &row);

    void writeValues(double time, Vector_3D vec_xl, Vector_3D vec_g);

private:
    std::ofstream outFile; // File stream
    std::string filename;  // File name
};

#endif // CSVWRITER_H