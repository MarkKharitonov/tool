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

Header::Header(istream& inFile)
{
    unsigned long long charMap[256] = { 0 };
    int c;
    while ((c = inFile.get()) >= 0)
    {
        ++charMap[c];
    }

    inFile.clear();
    inFile.seekg(0);

    unique_ptr<const Node> pRoot(BuildTree(charMap, m_countEntries));
    Bits cur;
    PopulateBits(m_bits, pRoot.get(), cur);

    DumpHistogramm(charMap, m_bits);

    DumpGraph(pRoot.get());

    Entry *pCurEntry = m_entries;
    for (int c = 0; c < 256; ++c)
    {
        if (charMap[c])
        {
            pCurEntry->Char = c;
            pCurEntry->Frequency = charMap[c];
            m_uncompressedByteCount += charMap[c];
            ++pCurEntry;
        }
    }
}

ostream& operator << (ostream& os, const Header& h)
{
    auto data = reinterpret_cast<const char *>(&h.m_countEntries);
    auto size = sizeof(h.m_countEntries) + sizeof(h.m_uncompressedByteCount) + h.m_countEntries * sizeof(Header::Entry);
    return os.write(data, size);
}

istream& operator >> (istream& is, Header& h)
{
    is >> h.m_countEntries;
    auto data = reinterpret_cast<char *>(&h.m_uncompressedByteCount);
    is.read(data, sizeof(h.m_uncompressedByteCount) + h.m_countEntries * sizeof(Header::Entry));

    unsigned long long charMap[256] = { 0 };
    for (int i = 0; i < h.m_countEntries; ++i)
    {
        auto entry = h.m_entries[i];
        charMap[entry.Char] = entry.Frequency;
    }

    unsigned short dummy;
    h.m_pRoot.reset(BuildTree(charMap, dummy));
    return is;
}
