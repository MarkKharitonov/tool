#pragma once
#include <ostream>
#include <string>

using namespace std;

class Bits
{
public:
    typedef unsigned long long data_type;
    typedef unsigned char size_type;
    static const size_type capacity = sizeof(data_type) * 8;

    class resize_error : runtime_error
    {
        static string s_msg;
    public:
        resize_error() :runtime_error(s_msg)
        {
        }
    };

    Bits() {}
    void init(data_type data, size_type size)
    {
        m_data = data;
        resize(size);
    }
    size_type size() const { return m_size; }
    size_type byteSize() const { return (m_size + 7) / 8; }
    void resize(size_type newSize)
    {
        if (newSize > capacity)
        {
            throw resize_error();
        }
        m_size = newSize;
        m_data &= ~(~0ULL << m_size);
    }
    void set(size_t index)
    {
        if (index >= m_size)
        {
            throw out_of_range("index out of range");
        }
        m_data |= 1ULL << index;
    }
    void clear(size_t index)
    {
        if (index >= m_size)
        {
            throw out_of_range("index out of range");
        }
        m_data &= ~(1ULL << index);
    }
    bool test(size_t index)
    {
        if (index >= m_size)
        {
            throw out_of_range("index out of range");
        }
        return (m_data >> index) & 0x1;
    }
    void flip(size_t index)
    {
        if (index >= m_size)
        {
            throw out_of_range("index out of range");
        }
        m_data ^= 1ULL << index;
    }
    data_type data() const { return m_data; }
    Bits& operator<<= (size_type shift)
    {
        m_data <<= shift;
        m_size += shift;
        if (m_size > capacity)
        {
            m_size = capacity;
        }
        return *this;
    }
    Bits& operator|=(data_type data)
    {
        m_data |= data;
        resize(m_size);
        return *this;
    }
private:
    data_type m_data = 0;
    size_type m_size = 0;
};

ostream& operator<<(ostream& os, const Bits& value);
