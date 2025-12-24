#include "Translator.h"

Translator::Translator(const Context& acContext) :
    m_pRoot { nullptr },
    m_pCurrAST { nullptr },
    m_cContext { acContext }
{
}

Translator::~Translator()
{
    delete m_pRoot;
}

void Translator::translate(Node* apRuleNode)
{
    apRuleNode->accept(*this);
}

AST* Translator::root() const
{
    return m_pRoot;
}

void Translator::visit(const RuleNode* apRuleNode)
{
    if (apRuleNode->children().size() == 1)
    {
        apRuleNode->children().at(0)->accept(*this);
    }
#if 0
    else if (apRuleNode->symbol() == "program")
    {
    }
    else if (apRuleNode->symbol() == "stmt_list")
    {
    }
    else if (apRuleNode->symbol() == "stmt")
    {
    }
    else if (apRuleNode->symbol() == "select_stmt")
    {
    }
    else if (apRuleNode->symbol() == "iteration_stmt")
    {
    }
    else if (apRuleNode->symbol() == "function_stmt")
    {
    }
    else if (apRuleNode->symbol() == "arg_list")
    {
    }
    else if (apRuleNode->symbol() == "list_expr")
    {
    }
    else if (apRuleNode->symbol() == "expr1")
    {
    }
    else if (apRuleNode->symbol() == "expr2")
    {
    }
    else if (apRuleNode->symbol() == "expr3")
    {
    }
    else if (apRuleNode->symbol() == "expr4")
    {
    }
    else if (apRuleNode->symbol() == "expr5")
    {
    }
#endif
    else if (apRuleNode->symbol() == "expr6")
    {
        AST* pParent = m_pCurrAST;

        m_pCurrAST = new BinOp(nullptr);
        apRuleNode->children().at(0)->accept(*this);

        for (size_t i = 1; i < apRuleNode->children().size() - 1; i += 2)
        {
            apRuleNode->children().at(i)->accept(*this);
            apRuleNode->children().at(i + 1)->accept(*this);

            if (i < apRuleNode->children().size() - 2)
            {
                AST* pAST = new BinOp(nullptr);
                pAST->add(m_pCurrAST);

                m_pCurrAST->set_parent(pAST);
                m_pCurrAST = pAST;
            }
        }

        m_pCurrAST->set_parent(pParent);

        pParent->add(m_pCurrAST);
    }
#if 0
    else if (apRuleNode->symbol() == "expr7")
    {
    }
    else if (apRuleNode->symbol() == "expr8")
    {
    }
    else if (apRuleNode->symbol() == "expr9")
    {
    }
    else if (apRuleNode->symbol() == "expr10")
    {
    }
    else if (apRuleNode->symbol() == "expr11")
    {
    }
#endif
    else
    {
        for (Node* pNode : apRuleNode->children())
        {
            pNode->accept(*this);
        }
    }
}

void Translator::visit(const TokenNode* apTokenNode)
{
    AST* pAST = nullptr;

    switch (apTokenNode->token()->type())
    {
        case Token::Type::CMD_ECHO:
            pAST = new CmdExpr(m_pCurrAST);
            break;
        case Token::Type::INTEGER:
            pAST = new Literal(m_pCurrAST);
            break;
        case Token::Type::OP_ADD:
        case Token::Type::OP_SUB:
            m_pCurrAST->set_token(apTokenNode->token());
            return;
        default:
            return;
    }

    pAST->set_token(apTokenNode->token());

    if (m_pCurrAST == nullptr)
    {
        m_pRoot = pAST;
        m_pCurrAST = pAST;
    }
    else
    {
        m_pCurrAST->add(pAST);
    }
}
