#include "csv_writer.h"
#include <cstring>

CSVWriter::CSVWriter(const std::string &filename, size_t bufferSize)
    : filename(filename), bufferSize(bufferSize), buffer(new char[bufferSize]), bufferIndex(0)
{
    outFile.open(filename, std::ios::out | std::ios::binary);
    if (!outFile)
    {
        throw std::ios_base::failure("Failed to open file");
    }
}

CSVWriter::~CSVWriter()
{
    flushBuffer();
    outFile.close();
    delete[] buffer;
}

void CSVWriter::writeValues(double time, Vector_3D vec_xl, Vector_3D vec_g, int toe, int heel)
{
    writeRow({std::to_string(time), std::to_string(vec_xl.x), std::to_string(vec_xl.y), std::to_string(vec_xl.z), std::to_string(vec_g.x), std::to_string(vec_g.y), std::to_string(vec_g.z), std::to_string(toe), std::to_string(heel)});
}

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

    if (bufferIndex + len >= bufferSize)
    {
        flushBuffer();
    }

    std::memcpy(buffer + bufferIndex, line.c_str(), len);
    bufferIndex += len;
}

void CSVWriter::writeLine(std::string line)
{
    size_t len = line.size();

    outFile.write(line.c_str(), len);
}

void CSVWriter::flushBuffer()
{
    if (bufferIndex > 0)
    {
        outFile.write(buffer, bufferIndex);
        bufferIndex = 0;
    }
}