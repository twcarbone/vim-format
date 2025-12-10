#ifndef NODE_H
#define NODE_H

#include <vector>

#include "Token.h"

//
// Node
//

class Node
{
public:
    Node(Node* parent);
    virtual ~Node();

    void add(Node* node);

    Node* parent() const;
    std::vector<Node*> children() const;

    virtual std::string toString() const = 0;

protected:
    Node* m_pParent;
    std::vector<Node*> m_lChildren;
};

//
// RuleNode
//

class RuleNode : public Node
{
public:
    RuleNode(Node* parent, const std::string& symbol);
    virtual ~RuleNode();

    std::string symbol() const;

    virtual std::string toString() const override;

private:
    std::string m_sSymbol;
};

//
// TokenNode
//

class TokenNode : public Node
{
public:
    TokenNode(Node* parent, Token* token);
    virtual ~TokenNode();

    Token* token() const;

    virtual std::string toString() const override;

private:
    Token* m_pToken;
};

#endif  // NODE_H
