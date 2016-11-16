#pragma once

#include <boost/iostreams/concepts.hpp>  // output_filter
#include "Node.h"

namespace io = boost::iostreams;

class decompression_output_filter : public io::output_filter
{
public:
    decompression_output_filter(const Node *pRoot, unsigned long long uncompressedByteCount) :m_pRoot(pRoot), m_pCurNode(pRoot), m_uncompressedByteCount(uncompressedByteCount) {}

    template<typename Sink>
    bool put(Sink& dest, int c)
    {
        if (m_uncompressedByteCount == m_curByteCount)
        {
            return false;
        }

        for (int i = 0; i < 8; ++i, c <<= 1)
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
                ++m_curByteCount;
                if (!io::put(dest, res))
                {
                    return false;
                }
                if (m_uncompressedByteCount == m_curByteCount)
                {
                    break;
                }
            }
        }
        return true;
    }
private:
    const Node *m_pRoot;
    const Node *m_pCurNode;
    const unsigned long long m_uncompressedByteCount;
    unsigned long long m_curByteCount = 0;
};

