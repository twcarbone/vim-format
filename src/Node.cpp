#include "Node.h"

//
// Node
//

Node::Node(Node* apParent) :
    m_pParent { apParent }
{
}

Node::~Node()
{
    for (Node* pChild : m_lChildren)
    {
        delete pChild;
    }

    m_lChildren.clear();
}

bool operator==(const Node& lhs, const Node& rhs)
{
    return lhs.m_pParent == rhs.m_pParent && lhs.m_lChildren == rhs.m_lChildren;
}

void Node::add(Node* apChild)
{
    m_lChildren.push_back(apChild);
}

int Node::level() const
{
    int lnLevel = 0;
    const Node* pCurrent = this;

    while (pCurrent->parent() != nullptr)
    {
        pCurrent = pCurrent->parent();
        lnLevel++;
    }

    return lnLevel;
}

Node* Node::parent() const
{
    return m_pParent;
}

void Node::set_parent(Node* apParent)
{
    m_pParent = apParent;
}

std::vector<Node*> Node::children() const
{
    return m_lChildren;
}

//
// RuleNode
//

RuleNode::RuleNode(Node* apParent, const std::string& asSymbol) :
    Node(apParent),
    m_sSymbol { asSymbol }
{
}

RuleNode::~RuleNode()
{
}

std::string RuleNode::symbol() const
{
    return m_sSymbol;
}

std::string RuleNode::toString() const
{
    return "[RuleNode] " + m_sSymbol;
}

bool operator==(const RuleNode& lhs, const RuleNode& rhs)
{
    if (lhs.m_sSymbol != rhs.m_sSymbol)
    {
        return false;
    }

    return dynamic_cast<const Node&>(lhs) == dynamic_cast<const Node&>(rhs);
}

void RuleNode::accept(NodeVisitor& acNodeVisitor)
{
    acNodeVisitor.visit(this);
}

//
// TokenNode
//

TokenNode::TokenNode(Node* apParent, Token* apToken) :
    Node(apParent),
    m_pToken { apToken }
{
}

Token* TokenNode::token() const
{
    return m_pToken;
}

TokenNode::~TokenNode()
{
}

std::string TokenNode::toString() const
{
    return "[TokenNode] " + m_pToken->toString();
}

bool operator==(const TokenNode& lhs, const TokenNode& rhs)
{
    if (lhs.m_pToken != rhs.m_pToken)
    {
        return false;
    }

    return dynamic_cast<const Node&>(lhs) == dynamic_cast<const Node&>(rhs);
}

void TokenNode::accept(NodeVisitor& acNodeVisitor)
{
    acNodeVisitor.visit(this);
}
