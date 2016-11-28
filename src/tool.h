#pragma once

#include <string>
using namespace std;

enum class Mode
{
    InputFilter,
    OutputFilter
};

int run(const string& inFilePath, const string& outFilePath, size_t bufferSize, bool compress, Mode mode, bool trace = false);
void compress(basic_istream<char>& inFile, ostream& outFile, Mode mode, bool trace = false);
void uncompress(istream & inFile, ostream& outFile, Mode mode);
