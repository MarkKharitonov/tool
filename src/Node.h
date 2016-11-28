#pragma once
#include <iosfwd>

using namespace std;

class Node;

class NodeId
{
public:
    NodeId(const Node& origin) :
        m_origin(origin)
    {
    }
    friend ostream& operator<<(ostream& os, const NodeId& nodeId);
private:
    const Node& m_origin;
};

class Node
{
    static int s_count;
public:
    Node(unsigned long long count, char c) :
        m_count(count), m_char(c), m_pLeft(NULL), m_pRight(NULL), m_id(-1)
    {
    }
    Node(const Node *pLeft, const Node *pRight) :
        m_pLeft(pLeft), m_pRight(pRight), m_count(pLeft->m_count + pRight->m_count), m_char(0), m_id(++s_count)
    {
        assert(pLeft->m_count >= pRight->m_count);
    }
    ~Node()
    {
        delete m_pLeft;
        delete m_pRight;
    }
    const Node *GetLeft() const { return m_pLeft; }
    const Node *GetRight() const { return m_pRight; }
    unsigned char GetChar() const { return m_char; }
    const Node *GetNode(Bits::data_type data, int& pos) const;
    bool IsLeaf() const { return !m_pLeft; }
    int GetId() const { return m_id; }
    unsigned long long GetCount() const { return m_count; }
    NodeId GetNodeId() const { return NodeId(*this); }
private:
    const Node *m_pLeft;
    const Node *m_pRight;
    const unsigned long long m_count;
    const unsigned char m_char;
    const int m_id;
};
