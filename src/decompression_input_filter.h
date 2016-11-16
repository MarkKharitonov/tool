#pragma once

#include <boost/iostreams/concepts.hpp>  // input_filter
#include "Node.h"

namespace io = boost::iostreams;

class decompression_input_filter : public io::input_filter
{
public:
    decompression_input_filter(const Node *pRoot, unsigned long long uncompressedByteCount) :m_pRoot(pRoot), m_pCurNode(pRoot), m_uncompressedByteCount(uncompressedByteCount) {}

    template<typename Source>
    int get(Source& src)
    {
        if (m_uncompressedByteCount == m_curByteCount)
        {
            return EOF;
        }

        int c = m_c;
        while (true)
        {
            for (int i = m_i; i < 8; ++i, c <<= 1)
            {
                if (c & 0x80)
                {
                    m_pCurNode = m_pCurNode->GetRight();
                }
                else
                {
                    m_pCurNode = m_pCurNode->GetLeft();
                }
                if (m_pCurNode->IsLeaf())
                {
                    int res = m_pCurNode->GetChar();
                    m_pCurNode = m_pRoot;
                    m_i = i + 1;
                    m_c = c << 1;
                    ++m_curByteCount;
                    return res;
                }
            }

            if ((c = io::get(src)) < 0)
            {
                return c;
            }
            m_i = 0;
        }
    }
private:
    const Node *m_pRoot;
    const Node *m_pCurNode;
    const unsigned long long m_uncompressedByteCount;
    unsigned long long m_curByteCount = 0;
    int m_i = 8;
    int m_c;
};

