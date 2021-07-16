#pragma once
#include <fstream>
#include <string>
#include <vector>
namespace csv {

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
void get_line(CsvFileStream &csv, std::vector<double> &vector);
void skip_line(CsvFileStream &csv);

} // namespace csv