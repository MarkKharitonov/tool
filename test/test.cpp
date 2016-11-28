#include "stdafx.h"
#define BOOST_TEST_MODULE Huffman
#include <boost/test/unit_test.hpp>
#include "..\\src\\tool.h"

BOOST_AUTO_TEST_CASE(compress_empty_with_input_filter_yields_empty)
{
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
BOOST_AUTO_TEST_CASE(text_compress_with_input_filter_uncompress_with_input_filter)
{
    char buf[] = "Hello World!";
    bufferstream rawInput(buf, sizeof(buf) - 1);
    basic_vectorstream<vector<char>> compressedOutput;
    compress(rawInput, compressedOutput, Mode::InputFilter);

    basic_vectorstream<vector<char>> uncompressedOutput;
    uncompress(compressedOutput, uncompressedOutput, Mode::InputFilter);

    BOOST_REQUIRE_EQUAL_COLLECTIONS(buf, buf + sizeof(buf) - 1, uncompressedOutput.vector().begin(), uncompressedOutput.vector().end());
}
