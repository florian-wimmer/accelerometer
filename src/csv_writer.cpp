#include "csv_writer.h"
#include <cstring>

CSVWriter::CSVWriter(const std::string &filename)
    : filename(filename)
{
    // open file stream
    outFile.open(filename, std::ios::out | std::ios::binary);
    if (!outFile)
    {
        throw std::ios_base::failure("Failed to open file");
    }
}

CSVWriter::~CSVWriter()
{
    outFile.close();
}

// call write Row function with the sensor values
void CSVWriter::writeValues(double time, Vector_3D vec_xl, Vector_3D vec_g)
{
    writeRow({std::to_string(time), std::to_string(vec_xl.x), std::to_string(vec_xl.y), std::to_string(vec_xl.z), std::to_string(vec_g.x), std::to_string(vec_g.y), std::to_string(vec_g.z)});
}

// write a single Row into the buffer
void CSVWriter::writeRow(const std::vector<std::string> &row)
{
    std::string line;
    for (const auto &item : row)
    {
        if (!line.empty())
        {
            line += ',';
        }
        line += item;
    }
    line += '\n';

    size_t len = line.size();

    outFile.write(line.c_str(), len);
}