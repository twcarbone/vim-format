#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include "AST.h"
#include "Context.h"
#include "Node.h"
#include "NodeVisitor.h"

class Translator : public NodeVisitor
{
public:
    Translator(const Context& context);
    virtual ~Translator();

    void translate(Node* root);

    AST* root() const;

    virtual void visit(const NonTerminal* rule_node);
    virtual void visit(const Terminal* rule_node);

private:
    AST* m_pRoot;
    AST* m_pCurrAST;
    const Context& m_cContext;

    void binop_left_fold(const NonTerminal* rule_node);
    void unary_left_fold(const NonTerminal* rule_node);
};

#endif  // TRANSLATOR_H
