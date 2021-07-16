#include "csv.h"

#include <exception>
#include <sstream>
namespace csv {

std::istream &CsvFileStream::open() {
    fileStream.open(source);
    if (!fileStream.is_open()) {
        throw std::runtime_error("failed to open file: " + source);
    }
    return fileStream;
}

void CsvFileStream::close() {
    fileStream.close();
}

bool CsvFileStream::is_open() {
    return fileStream.is_open();
}

std::ifstream &CsvFileStream::getStream() {
    return fileStream;
}

void get_line(CsvFileStream &csv, std::vector<double> &vector) {
    std::istream &stream = csv.getStream();
    if (csv.is_open() && !stream.eof()) {
        std::stringstream buffer;
        std::string line;
        std::string data;
        std::getline(stream, line);
        buffer << line;
        while (!buffer.eof()) {
            std::getline(buffer, data, ',');
            vector.push_back(std::stod(data));
        }
    }
}

void skip_line(CsvFileStream &csv) {
    std::istream &stream = csv.getStream();
    if (csv.is_open() && !stream.eof()) {
        std::string line;
        std::getline(stream, line);
    }
}

} // namespace csv