#include "stdafx.h"
#include "Bits.h"
#include "Node.h"

using namespace std;

int Node::s_count = 0;

const Node *Node::GetNode(Bits::data_type data, int& pos) const
{
    if (IsLeaf() || !pos)
    {
        return this;
    }
    --pos;
    if ((data >> pos) & 1)
    {
        return m_pRight->GetNode(data, pos);
    }
    return m_pLeft->GetNode(data, pos);
}

ostream& operator<<(ostream& os, const NodeId& nodeId)
{
    if (!nodeId.m_origin.IsLeaf())
    {
        return os << "n" << nodeId.m_origin.GetId();
    }
    auto c = nodeId.m_origin.GetChar();
    if (isgraph(c))
    {
        if ('\\' == c || '"' == c)
        {
            os << '\\';
        }
        return os << c;
    }
    return os << "0x" << hex << int(c) << dec;
}
