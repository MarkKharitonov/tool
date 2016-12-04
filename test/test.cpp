#include "stdafx.h"
#define BOOST_TEST_MODULE Huffman
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/data/monomorphic.hpp>
#include "..\\src\\tool.h"
#include "../src/Header.h"

namespace bdata = boost::unit_test::data;
using namespace std;

BOOST_AUTO_TEST_CASE(compress_empty_with_input_filter_yields_empty) 
{
    cerr << boost::unit_test::framework::current_test_case().p_name << endl;
    char buf;
    bufferstream inputStream(&buf, 0);
    basic_vectorstream<vector<char>> outputStream;
    compress(inputStream, outputStream, Mode::InputFilter);
    BOOST_REQUIRE_EQUAL(0, outputStream.vector().size());
}
BOOST_AUTO_TEST_CASE(compress_empty_with_output_filter_yields_empty) 
{
    char buf;
    bufferstream inputStream(&buf, 0);
    basic_vectorstream<vector<char>> outputStream;
    compress(inputStream, outputStream, Mode::OutputFilter);
    BOOST_REQUIRE_EQUAL(0, outputStream.vector().size());
}
BOOST_AUTO_TEST_CASE(uncompress_empty_with_input_filter_yields_empty)
{
    char buf;
    bufferstream inputStream(&buf, 0);
    basic_vectorstream<vector<char>> outputStream;
    uncompress(inputStream, outputStream, Mode::InputFilter);
    BOOST_REQUIRE_EQUAL(0, outputStream.vector().size());
}
BOOST_AUTO_TEST_CASE(uncompress_empty_with_output_filter_yields_empty)
{
    char buf;
    bufferstream inputStream(&buf, 0);
    basic_vectorstream<vector<char>> outputStream;
    uncompress(inputStream, outputStream, Mode::OutputFilter);
    BOOST_REQUIRE_EQUAL(0, outputStream.vector().size());
}
BOOST_DATA_TEST_CASE(
    text_compress_uncompress,
    bdata::xrange(int(Mode::__Count__)) * bdata::xrange(int(Mode::__Count__)),
    compressionMode, uncompressionMode)
{
    char buf[] = "Hello World!";
    bufferstream rawInput(buf, sizeof(buf) - 1);
    basic_vectorstream<vector<char>> compressedOutput;
    compress(rawInput, compressedOutput, Mode(compressionMode));

    basic_vectorstream<vector<char>> uncompressedOutput;
    uncompress(compressedOutput, uncompressedOutput, Mode(uncompressionMode));

    BOOST_REQUIRE_EQUAL_COLLECTIONS(buf, buf + sizeof(buf) - 1, uncompressedOutput.vector().begin(), uncompressedOutput.vector().end());
}
void do_binary_compress_uncompress(char *buf, int len, Mode compressionMode, Mode uncompressionMode)
{
    iota(buf, buf + len, 0);
    bufferstream rawInput(buf, len);
    basic_vectorstream<vector<char>> compressedOutput;
    compress(rawInput, compressedOutput, Mode(compressionMode));

    basic_vectorstream<vector<char>> uncompressedOutput;
    uncompress(compressedOutput, uncompressedOutput, Mode(uncompressionMode));

    BOOST_REQUIRE_EQUAL_COLLECTIONS(buf, buf + len, uncompressedOutput.vector().begin(), uncompressedOutput.vector().end());
}

BOOST_DATA_TEST_CASE(
    binary_compress_uncompress,
    bdata::xrange(int(Mode::__Count__)) * bdata::xrange(int(Mode::__Count__)),
    compressionMode, uncompressionMode)
{
    char buf[256];
    do_binary_compress_uncompress(buf, sizeof(buf), Mode(compressionMode), Mode(uncompressionMode));
}

BOOST_DATA_TEST_CASE(
    large_binary_compress_uncompress,
    bdata::xrange(int(Mode::__Count__)) * bdata::xrange(int(Mode::__Count__)),
    compressionMode, uncompressionMode)
{
    char buf[256 * 1000];
    do_binary_compress_uncompress(buf, sizeof(buf), Mode(compressionMode), Mode(uncompressionMode));
}

#if 0
void trace_vector(const char *szPrompt, const vector<char>& v)
{
    cout << szPrompt << " = ";
    auto delim = "";
    for (auto ch : v)
    {
        cout << delim << setfill('0') << setw(2) << hex << (int(ch) & 0xFF);
        delim = " ";
    }
    cout << endl;
}
#endif

BOOST_DATA_TEST_CASE(
    double_compress,
    bdata::xrange(int(Mode::__Count__)) * bdata::xrange(int(Mode::__Count__)),
    compressionMode, uncompressionMode)
{
    char buf[] = "Hello World!";
    bufferstream rawInput(buf, sizeof(buf) - 1);
    basic_vectorstream<vector<char>> compressedOutput;
    basic_vectorstream<vector<char>> compressedOutput2;
    compress(rawInput, compressedOutput, Mode(compressionMode));
    compress(compressedOutput, compressedOutput2, Mode(compressionMode));

    basic_vectorstream<vector<char>> uncompressedOutput;
    basic_vectorstream<vector<char>> uncompressedOutput2;
    uncompress(compressedOutput2, uncompressedOutput2, Mode(uncompressionMode));
    uncompress(uncompressedOutput2, uncompressedOutput, Mode(uncompressionMode));

    BOOST_REQUIRE_EQUAL_COLLECTIONS(buf, buf + sizeof(buf) - 1, uncompressedOutput.vector().begin(), uncompressedOutput.vector().end());
}
