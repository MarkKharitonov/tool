#pragma once

#include <string>
using namespace std;

enum class Mode
{
    InputFilter,
    OutputFilter,
    __Count__
};

class Header;

int run(const string& inFilePath, const string& outFilePath, size_t bufferSize, bool compress, Mode mode, bool trace = false);
void do_compress(const Header& h, basic_istream<char>& inFile, ostream& outFile, Mode mode);
void compress(basic_istream<char>& inFile, ostream& outFile, Mode mode, bool trace = false);
void do_uncompress(const Header& h, istream& inFile, ostream& outFile, Mode mode);
void uncompress(istream & inFile, ostream& outFile, Mode mode);
