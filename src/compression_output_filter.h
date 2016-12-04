#pragma once

#include <boost/iostreams/concepts.hpp>  // output_filter
#include "Bits.h"

namespace io = boost::iostreams;

class compression_output_filter : public io::output_filter
{
public:
    compression_output_filter(const Bits *pBits) :m_pBits(pBits) {}

    template<typename Sink>
    bool put(Sink& dest, int c)
    {
        const Bits *pCurBits = m_pBits + unsigned char(c);
        int curBitsPos = pCurBits->size();

        while (true)
        {
            // Needs - resPos bits
            // Has available in m_curBits - m_curBitsPos bits
            if (m_resPos <= curBitsPos)
            {
                auto bits = (pCurBits->data() >> (curBitsPos - m_resPos)) & ~(~0 << m_resPos);
                m_res |= bits;
                if (!io::put(dest, m_res))
                {
                    return false;
                }
                curBitsPos -= m_resPos;
                m_res = 0;
                m_resPos = 8;
                continue;
            }

            auto bits = (pCurBits->data() << (m_resPos - curBitsPos)) & ~(~0 << m_resPos);
            m_res |= bits;
            m_resPos -= curBitsPos;
            break;
        }
        return true;
    }
    template<typename Sink>
    void close(Sink& dest) 
    { 
        if (m_resPos < 8)
        {
            io::put(dest, m_res);
        }
    }
private:
    const Bits *m_pBits;
    int m_res = 0;
    int m_resPos = 8;
};

