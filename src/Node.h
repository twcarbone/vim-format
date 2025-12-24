#ifndef NODE_H
#define NODE_H

#include <vector>

#include "NodeVisitor.h"
#include "Token.h"

//
// Node
//

class Node
{
public:
    Node(Node* parent);
    virtual ~Node();

    int level() const;

    Node* parent() const;
    void set_parent(Node* parent);

    std::vector<Node*> children() const;
    void add(Node* node);

    virtual std::string toString() const = 0;

    friend bool operator==(const Node& lhs, const Node& rhs);

    virtual void accept(NodeVisitor& visitor) const = 0;

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

    friend bool operator==(const RuleNode& lhs, const RuleNode& rhs);

    virtual void accept(NodeVisitor& visitor) const override;

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

    friend bool operator==(const TokenNode& lhs, const TokenNode& rhs);

    virtual void accept(NodeVisitor& visitor) const override;

private:
    Token* m_pToken;
};

#endif  // NODE_H
