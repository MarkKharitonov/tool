// tool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Header.h"
#include "compression_input_filter.h"
#include "decompression_input_filter.h"
#include "compression_output_filter.h"
#include "decompression_output_filter.h"

#include <boost/iostreams/filtering_stream.hpp>

namespace io = boost::iostreams;

using namespace TCLAP;
using namespace std;

enum class Mode
{
    InputFilter,
    OutputFilter
};

void CopyStream(istream& src, ostream& dst)
{
    int c;
    while ((c = src.get()) >= 0)
    {
        dst.put(c);
    }
}

void Uncompress(istream & inFile, streamoff fileLength, ostream& outFile, size_t bufferSize, Mode mode)
{
    Header h;
    inFile >> h;

    if (Mode::InputFilter == mode)
    {
        io::filtering_istream in;
        in.push(decompression_input_filter(h.GetRoot(), h.GetUncompressedByteCount()));
        in.push(inFile);

        CopyStream(in, outFile);
    }
    else
    {
        io::filtering_ostream out;
        out.push(decompression_output_filter(h.GetRoot(), h.GetUncompressedByteCount()));
        out.push(outFile);

        CopyStream(inFile, out);
    }
}

void Compress(istream& inFile, streamoff fileLength, ostream& outFile, size_t bufferSize, Mode mode)
{
    unique_ptr<char[]> buf(new char[bufferSize]);

    Header h(inFile, fileLength, buf.get(), bufferSize);
    outFile << h;

    if (Mode::InputFilter == mode)
    {
        io::filtering_istream in;
        in.push(compression_input_filter(h.GetBits()));
        in.push(inFile);

        CopyStream(in, outFile);
    }
    else
    {
        io::filtering_ostream out;
        out.push(compression_output_filter(h.GetBits()));
        out.push(outFile);

        CopyStream(inFile, out);
    }
}

int main(int argc, char** argv)
{
    // Wrap everything in a try block.  Do this every time, 
    // because exceptions will be thrown for problems. 
    try
    {
        // Define the command line object.
        CmdLine cmd("Compression tool", ' ', "0.1");
        SwitchArg compressArg("c", "compress", "Compress the input file.");
        SwitchArg uncompressArg("u", "uncompress", "Uncompress the input file.");
        ValueArg<string> inputArg("i", "input", "Input file", true, "", "string", cmd);
        ValueArg<string> outputArg("o", "output", "Output file", true, "", "string", cmd);
        ValueArg<size_t> bufferSizeArg("s", "bufferSize", "Buffer size", false, 10240, "size_t", cmd);
        ValueArg<int> modeArg("m", "mode", "Mode, possible values 0 and 1", false, (int)Mode::InputFilter, "Mode", cmd);
        cmd.xorAdd(compressArg, uncompressArg);
        cmd.parse(argc, argv);

        ifstream inFile(inputArg.getValue(), ios::ate | ios::binary);
        ofstream outFile(outputArg.getValue(), ios::out | ios::binary);
        auto bufferSize = bufferSizeArg.getValue();

        auto fileLength = inFile.tellg();
        inFile.seekg(0);

        inFile.exceptions(ifstream::badbit);
        outFile.exceptions(ifstream::badbit);

        if (compressArg.getValue())
        {
            Compress(inFile, fileLength, outFile, bufferSize, (Mode)modeArg.getValue());
        }
        else
        {
            Uncompress(inFile, fileLength, outFile, bufferSize, (Mode)modeArg.getValue());
        }
    }
    catch (ArgException &e)  // catch any exceptions
    {
        cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    }
    return 0;
}
