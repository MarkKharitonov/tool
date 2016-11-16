#pragma once
#include <iosfwd>
#include "Bits.h"
#include "Node.h"

using namespace std;

class Header
{
#pragma pack(push, 1)
    struct Entry
    {
        unsigned char Char = 0;
        unsigned long long Frequency = 0;
    };
#pragma pack(pop)
public:
    class buffer_size_error : runtime_error
    {
        static const string s_msg;
    public:
        buffer_size_error() :runtime_error(s_msg) {}
    };
    Header() {}
    Header(istream& is, streamoff fileLength, char *buf, size_t bufferSize);
    const Bits& GetBits(unsigned char c) const { return m_bits[c]; }
    const Bits *GetBits() const { return m_bits; }
    const Node *GetRoot() const { return m_pRoot.get(); }
    unsigned long long GetUncompressedByteCount() const { return m_uncompressedByteCount; }
    friend ostream& operator<<(ostream& os, const Header& h);
    friend istream& operator >> (istream& os, Header& h);
private:
    Bits m_bits[256];
    unique_ptr<const Node> m_pRoot;
#pragma pack(push, 1)
    struct
    {
        unsigned char m_countEntries = 0;
        unsigned long long m_uncompressedByteCount = 0;
        Entry m_entries[256];
    };
#pragma pack(pop)
    const static int s_maxBufferSize = 1 + sizeof(Entry) * 256;
};