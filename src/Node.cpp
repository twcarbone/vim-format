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

void Node::add(Node* apChild)
{
    m_lChildren.push_back(apChild);
}

Node* Node::parent() const
{
    return m_pParent;
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
    // TODO: Who owns the Token?
    // If calling Tokenizer::tokenize(), the Tokenizer destructor deletes all of the
    // buffered Tokens. But it calling Parser::parse(...), The Tokenizer has no buffered
    // Tokens and we need this delete.
    delete m_pToken;
}

std::string TokenNode::toString() const
{
    return "[TokenNode] " + m_pToken->toString();
}
