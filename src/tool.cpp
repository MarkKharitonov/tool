// tool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Header.h"
#include "compression_input_filter.h"
#include "decompression_input_filter.h"
#include "compression_output_filter.h"
#include "decompression_output_filter.h"

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/log/trivial.hpp>

namespace io = boost::iostreams;
namespace logging = boost::log;

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

void Uncompress(istream & inFile, ostream& outFile, Mode mode)
{
    Header h;
    inFile >> h;

    if (Mode::InputFilter == mode)
    {
        BOOST_LOG_TRIVIAL(trace) << "Using decompression input filter";
        io::filtering_istream in;
        in.push(decompression_input_filter(h.GetRoot(), h.GetUncompressedByteCount()));
        in.push(inFile);

        CopyStream(in, outFile);
    }
    else
    {
        BOOST_LOG_TRIVIAL(trace) << "Using decompression output filter";
        io::filtering_ostream out;
        out.push(decompression_output_filter(h.GetRoot(), h.GetUncompressedByteCount()));
        out.push(outFile);

        CopyStream(inFile, out);
    }
}

void Compress(basic_istream<char>& inFile, ostream& outFile, Mode mode)
{
    Header h(inFile);
    outFile << h;

    if (Mode::InputFilter == mode)
    {
        BOOST_LOG_TRIVIAL(trace) << "Using compression input filter";
        io::filtering_istream in;
        in.push(compression_input_filter(h.GetBits()));
        in.push(inFile);

        CopyStream(in, outFile);
    }
    else
    {
        BOOST_LOG_TRIVIAL(trace) << "Using compression output filter";
        io::filtering_ostream out;
        out.push(compression_output_filter(h.GetBits()));
        out.push(outFile);

        CopyStream(inFile, out);
    }
}

int run(const string& inFilePath, const string& outFilePath, size_t bufferSize, bool compress, Mode mode)
{
    ifstream inFile(inFilePath, ios::binary);
    if (inFile.fail())
    {
        stringstream ss;
        ss << "Input file " << inFilePath << " could not be open";
        perror(ss.str().c_str());
        return 1;
    }

    ofstream outFile(outFilePath, ios::binary);
    if (outFile.fail())
    {
        stringstream ss;
        ss << "Output file " << outFilePath << " could not be created";
        perror(ss.str().c_str());
        return 1;
    }

    unique_ptr<char[]> inbuf(new char[bufferSize]);
    unique_ptr<char[]> outbuf(new char[bufferSize]);
    inFile.rdbuf()->pubsetbuf(inbuf.get(), bufferSize);
    outFile.rdbuf()->pubsetbuf(outbuf.get(), bufferSize);

    inFile.exceptions(ifstream::badbit);
    outFile.exceptions(ifstream::badbit);

    if (compress)
    {
        Compress(inFile, outFile, mode);
    }
    else
    {
        Uncompress(inFile, outFile, mode);
    }
    outFile.flush();
    return 0;
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

        auto bufferSize = bufferSizeArg.getValue();

        return run(inputArg.getValue(), outputArg.getValue(), bufferSizeArg.getValue(), compressArg.getValue(), (Mode)modeArg.getValue());
    }
    catch (ArgException &e)  // catch any exceptions
    {
        cerr << "error: " << e.error() << " for arg " << e.argId() << endl;
    }
    return 0;
}