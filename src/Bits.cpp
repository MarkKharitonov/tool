#include "stdafx.h"
#include "Bits.h"

using namespace std;

string Bits::resize_error::s_msg = string("Cannot resize beyond ") + to_string(Bits::capacity) + " bits";

ostream& operator<<(ostream& os, const Bits& value)
{
    auto size = value.size();
    if (size)
    {
        auto data = value.data();
        while (size)
        {
            --size;
            os << ((data >> size) & 0x1);
        }
    }
    return os;
}