#pragma once
#include <fstream>
#include <string>
#include <vector>

class CsvFileStream {
  private:
    std::string source;
    std::ifstream fileStream;

  public:
    CsvFileStream(const std::string &source) : source(source) {};
    std::ifstream &getStream();
    std::istream &open();
    bool is_open();
    void close();
};
std::vector<double> get_line(CsvFileStream &csv);