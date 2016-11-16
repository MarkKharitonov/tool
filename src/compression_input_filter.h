#pragma once

#include <boost/iostreams/concepts.hpp>  // input_filter
#include "Bits.h"

namespace io = boost::iostreams;

class compression_input_filter : public io::input_filter
{
public:
    compression_input_filter(const Bits *pBits) :m_pBits(pBits), m_pCurBits(pBits) {}

    template<typename Source>
    int get(Source& src)
    {
        int res = 0, resPos = 8;
        if (m_done)
        {
            res = m_done;
            m_done = 0;
            return res;
        }

        while (true)
        {
            if (!m_curBitsPos)
            {
                int c;
                if ((c = io::get(src)) < 0)
                {
                    if (resPos < 8)
                    {
                        m_done = c;
                        return res;
                    }
                    return c;
                }
                m_pCurBits = m_pBits + c;
                m_curBitsPos = m_pCurBits->size();
            }

            // Needs - resPos bits
            // Has available in m_curBits - m_curBitsPos bits
            if (resPos <= m_curBitsPos) 
            {
                auto bits = (m_pCurBits->data() >> (m_curBitsPos - resPos)) & ~(~0 << resPos);
                res |= bits;
                m_curBitsPos -= resPos;
                resPos = 0;
                return res;
            }

            auto bits = (m_pCurBits->data() << (resPos - m_curBitsPos)) & ~(~0 << resPos);
            res |= bits;
            resPos -= m_curBitsPos;
            m_curBitsPos = 0;
        }
    }
private:
    const Bits *m_pBits;
    const Bits *m_pCurBits;
    int m_done = 0;
    int m_curBitsPos = 0;
};

