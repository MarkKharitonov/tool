#include "stdafx.h"
#include "Header.h"
#include "Node.h"
#include "Bits.h"

using namespace std;

void Reverse(Bits& bits)
{
    auto sz = bits.size();
    for (size_t i = 0, j = sz - 1; i < j; ++i, --j)
    {
        if (bits.test(i) != bits.test(j))
        {
            bits.flip(i);
            bits.flip(j);
        }
    }
}

void PopulateBits(Bits bits[], const Node *pRoot, Bits& cur)
{
    if (pRoot->IsLeaf())
    {
        bits[pRoot->GetChar()] = cur;
        Reverse(bits[pRoot->GetChar()]);
    }
    else
    {
        auto sz = cur.size();
        cur.resize(sz + 1);
        cur.clear(sz);
        PopulateBits(bits, pRoot->GetLeft(), cur);
        cur.set(sz);
        PopulateBits(bits, pRoot->GetRight(), cur);
        cur.resize(sz);
    }
}

void DumpHistogramm(unsigned long long charMap[], Bits bits[])
{
    unsigned char ordered[256];
    iota(ordered, ordered + 256, 0);
    sort(ordered, ordered + 256, [&](unsigned char x, unsigned char y)
    {
        long long res = charMap[x] - charMap[y];
        return res ? res > 0 : x < y;
    });

    unsigned long long uncompressedSize = 0;
    unsigned long long compressedSize = 0;
    for (int i = 0; i < 256 && charMap[ordered[i]]; ++i)
    {
        auto c = ordered[i];
        uncompressedSize += charMap[c];
        compressedSize += charMap[c] * bits[c].size();

        if (isgraph(c))
        {
            cout << setw(4) << c;
        }
        else
        {
            cout << "0x" << setfill('0') << setw(2) << uppercase << hex << int(c) << dec << setfill('\x0');
        }
        cout << " : " << setw(8) << left << charMap[c] << setw(0) << right << " (" << bits[c] << ")" << endl;
    }
    compressedSize = (compressedSize + 7) / 8;
    cout << "Uncompressed size = " << uncompressedSize << endl;
    cout << "Expected compressed size = " << compressedSize << endl;
    cout << "Expected compression ratio = " << (compressedSize * 100) / uncompressedSize << "%" << endl;
}

void DumpNode(ostream& os, const Node *pNode)
{
    if (!pNode->IsLeaf())
    {
        os << "  \"" << pNode->GetNodeId() << "\" -> \"" << pNode->GetLeft()->GetNodeId() << "\" [ label=0 ]" << endl;
        DumpNode(os, pNode->GetLeft());
        os << "  \"" << pNode->GetNodeId() << "\" -> \"" << pNode->GetRight()->GetNodeId() << "\" [ label=1 ]" << endl;
        DumpNode(os, pNode->GetRight());
    }
}

void DumpGraph(const Node *pRoot)
{
    ofstream os("graph.txt");
    os << "digraph tree {" << endl;
    DumpNode(os, pRoot);
    os << "}" << endl;
}

const Node *BuildTree(unsigned long long charMap[], unsigned short& countEntries)
{
    vector<Node *> nodes;
    for (int c = 0; c < 256; ++c)
    {
        if (charMap[c])
        {
            ++countEntries;
            nodes.push_back(new Node(charMap[c], c));
        }
    }

    auto funcIsCountGreater = [](const Node *x, const Node * y) { return x->GetCount() > y->GetCount(); };
    sort(nodes.begin(), nodes.end(), funcIsCountGreater);

    while (nodes.size() > 1)
    {
        auto pRight = nodes.back();
        nodes.pop_back();
        auto pLeft = nodes.back();
        nodes.pop_back();
        auto p = new Node(pLeft, pRight);
        auto pos = lower_bound(nodes.begin(), nodes.end(), p, funcIsCountGreater);
        nodes.insert(pos, p);
    }
    return nodes[0];
}

Header::Header(basic_istream<char>& inFile, bool trace)
{
    unsigned long long charMap[256] = { 0 };
    int c;
    while ((c = inFile.get()) >= 0)
    {
        ++charMap[c];
    }

    inFile.clear();
    if (!inFile.tellg())
    {
        return;
    }

    inFile.seekg(0);

    unique_ptr<const Node> pRoot(BuildTree(charMap, m_data.m_countEntries));
    Bits cur;
    PopulateBits(m_bits, pRoot.get(), cur);
    
    if (trace)
    {
        DumpHistogramm(charMap, m_bits);

        DumpGraph(pRoot.get());
    }

    auto pCurEntry = m_data.m_entries;
    for (c = 0; c < 256; ++c)
    {
        if (charMap[c])
        {
            pCurEntry->Char = c;
            pCurEntry->Frequency = charMap[c];
            m_data.m_uncompressedByteCount += charMap[c];
            ++pCurEntry;
        }
    }
}

void write(unsigned char val, char*& buffer)
{
    buffer[0] = val;
    ++buffer;
}
void write(unsigned short val, char*& buffer)
{
    buffer[0] = val & 0xFF;
    buffer[1] = val >> 8;
    buffer += 2;
}
void write(unsigned long long val, char*& buffer)
{
    buffer[0] = val & 0xFF;
    buffer[1] = (val >> 8) & 0xFF;
    buffer[2] = (val >> 16) & 0xFF;
    buffer[3] = (val >> 24) & 0xFF;
    buffer[4] = (val >> 32) & 0xFF;
    buffer[5] = (val >> 40) & 0xFF;
    buffer[6] = (val >> 48) & 0xFF;
    buffer[7] = val >> 56;
    buffer += 8;
}
void read(unsigned char& val, char*& buffer)
{
    val = buffer[0];
    ++buffer;
}
void read(unsigned short& val, char*& buffer)
{
    val = unsigned short(buffer[1]) & 0xFF; val <<= 8;
    val |= unsigned short(buffer[0]) & 0xFF;
    buffer += 2;
}
void read(unsigned long long& val, char*& buffer)
{
    val = unsigned long long(buffer[7]) & 0xFF; val <<= 8;
    val |= unsigned long long(buffer[6]) & 0xFF; val <<= 8;
    val |= unsigned long long(buffer[5]) & 0xFF; val <<= 8;
    val |= unsigned long long(buffer[4]) & 0xFF; val <<= 8;
    val |= unsigned long long(buffer[3]) & 0xFF; val <<= 8;
    val |= unsigned long long(buffer[2]) & 0xFF; val <<= 8;
    val |= unsigned long long(buffer[1]) & 0xFF; val <<= 8;
    val |= unsigned long long(buffer[0]) & 0xFF;
    buffer += 8;
}

ostream& operator << (ostream& os, const Header& h)
{
    if (!h.m_data.m_countEntries)
    {
        return os;
    }
    char buffer[sizeof(Header::HeaderData)];
    auto pCurPos = buffer;
    write(h.m_data.m_countEntries, pCurPos);
    write(h.m_data.m_uncompressedByteCount, pCurPos);
    for (int i = 0; i < h.m_data.m_countEntries; ++i)
    {
        write(h.m_data.m_entries[i].Char, pCurPos);
        write(h.m_data.m_entries[i].Frequency, pCurPos);
    }
    return os.write(buffer, pCurPos - buffer);
}

istream& operator >> (istream& is, Header& h)
{
    auto curPos = is.tellg();
    if (!is.seekg(curPos + streamoff(1)))
    {
        return is;
    }
    is.seekg(curPos);

    char tmp[2];
    auto pCurPos = tmp;
    is.read(tmp, 2);
    read(h.m_data.m_countEntries, pCurPos);

    char buffer[sizeof(Header::HeaderData) - 2];
    is.read(buffer, sizeof(h.m_data.m_uncompressedByteCount) + h.m_data.m_countEntries * sizeof(Header::Entry));
    pCurPos = buffer;
    read(h.m_data.m_uncompressedByteCount, pCurPos);
    for (int i = 0; i < h.m_data.m_countEntries; ++i)
    {
        read(h.m_data.m_entries[i].Char, pCurPos);
        read(h.m_data.m_entries[i].Frequency, pCurPos);
    }

    unsigned long long charMap[256] = { 0 };
    for (int i = 0; i < h.m_data.m_countEntries; ++i)
    {
        auto entry = h.m_data.m_entries[i];
        charMap[entry.Char] = entry.Frequency;
    }

    unsigned short dummy;
    h.m_pRoot.reset(BuildTree(charMap, dummy));
    return is;
}
