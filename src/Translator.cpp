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

void Translator::translate(Node* apNonTerminal)
{
    apNonTerminal->accept(*this);
}

AST* Translator::root() const
{
    return m_pRoot;
}

void Translator::visit(const NonTerminal* apNonTerminal)
{
    if (apNonTerminal->symbol() == "program")
    {
        AST* pAST = new Program(nullptr);

        m_pRoot = pAST;
        m_pCurrAST = pAST;
    }
    else if (apNonTerminal->symbol() == "stmt_list")
    {
        AST* pAST = new StmtList(m_pCurrAST);
        m_pCurrAST->add(pAST);
        m_pCurrAST = pAST;

        AST* pParent = m_pCurrAST;

        for (Node* pNode : apNonTerminal->children())
        {
            pNode->accept(*this);
            m_pCurrAST = pParent;
        }

        return;
    }
    else if (apNonTerminal->symbol() == "select_stmt" || apNonTerminal->symbol() == "iteration_stmt")
    {
        AST* pParent = m_pCurrAST;

        for (Node* pNode : apNonTerminal->children())
        {
            pNode->accept(*this);
            m_pCurrAST = pParent;
        }

        return;
    }
    else if (apNonTerminal->symbol() == "list_expr")
    {
        AST* pAST = new ListExpr(m_pCurrAST);
        m_pCurrAST->add(pAST);
        m_pCurrAST = pAST;

        AST* pParent = m_pCurrAST;

        for (Node* pNode : apNonTerminal->children())
        {
            pNode->accept(*this);
            m_pCurrAST = pParent;
        }

        return;
    }
    else if (apNonTerminal->symbol() == "stmt")
    {
        AST* pAST = new CmdExpr(m_pCurrAST);
        m_pCurrAST->add(pAST);
        m_pCurrAST = pAST;
    }
    else if (apNonTerminal->children().size() == 1)
    {
        apNonTerminal->children().at(0)->accept(*this);

        return;
    }
    else if (apNonTerminal->symbol() == "expr2" || apNonTerminal->symbol() == "expr3"
             || apNonTerminal->symbol() == "expr5" || apNonTerminal->symbol() == "expr6"
             || apNonTerminal->symbol() == "expr7")
    {
        AST* pParent = m_pCurrAST;

        binop_left_fold(apNonTerminal);

        m_pCurrAST->set_parent(pParent);
        pParent->add(m_pCurrAST);

        m_pCurrAST = pParent;

        return;
    }
    else if (apNonTerminal->symbol() == "expr9")
    {
        AST* pParent = m_pCurrAST;

        unary_left_fold(apNonTerminal);

        m_pCurrAST->set_parent(pParent);
        pParent->add(m_pCurrAST);

        m_pCurrAST = pParent;

        return;
    }

    for (Node* pNode : apNonTerminal->children())
    {
        pNode->accept(*this);
    }
}

void Translator::visit(const Terminal* apTerminal)
{
    AST* pAST = nullptr;

    switch (apTerminal->token()->type())
    {
        // Literal
        case Token::Type::INTEGER:
        case Token::Type::FLOAT:
        case Token::Type::STRING:
            pAST = new Literal(m_pCurrAST);
            break;
        // Var
        case Token::Type::IDENTIFIER:
            pAST = new Var(m_pCurrAST);
            break;
        // CmdExpr
        case Token::Type::CMD_ECHO:
        case Token::Type::CMD_LET:
        case Token::Type::ASSIGN_ADD:
        case Token::Type::ASSIGN_MINUS:
        case Token::Type::ASSIGN_MUL:
        case Token::Type::ASSIGN_DIV:
        case Token::Type::ASSIGN_EQ:
        case Token::Type::ASSIGN_MODULO:
        case Token::Type::ASSIGN_CAT_NEW:
        case Token::Type::ASSIGN_CAT_OLD:
        case Token::Type::IF:
        case Token::Type::FOR:
        case Token::Type::WHILE:
        // BinOp
        case Token::Type::OP_OR:
        case Token::Type::OP_AND:
        case Token::Type::OP_LSHIFT:
        case Token::Type::OP_RSHIFT:
        case Token::Type::OP_ADD:
        case Token::Type::OP_SUB:
        case Token::Type::OP_DOT:
        case Token::Type::OP_CAT_NEW:
        case Token::Type::OP_MUL:
        case Token::Type::OP_DIV:
        case Token::Type::OP_MODULO:
        // UnaryOp
        case Token::Type::OP_LOGICAL_NOT:
        case Token::Type::OP_UNARY_MINUS:
        case Token::Type::OP_UNARY_PLUS:
            m_pCurrAST->set_token(apTerminal->token());
            break;
        default:
            break;
    }

    if (pAST != nullptr)
    {
        pAST->set_token(apTerminal->token());
        m_pCurrAST->add(pAST);
    }
}

void Translator::binop_left_fold(const NonTerminal* apNonTerminal)
{
    m_pCurrAST = new BinOp(nullptr);
    apNonTerminal->children().at(0)->accept(*this);

    for (size_t i = 1; i < apNonTerminal->children().size() - 1; i += 2)
    {
        apNonTerminal->children().at(i)->accept(*this);
        apNonTerminal->children().at(i + 1)->accept(*this);

        if (i < apNonTerminal->children().size() - 2)
        {
            AST* pAST = new BinOp(nullptr);
            pAST->add(m_pCurrAST);

            m_pCurrAST->set_parent(pAST);
            m_pCurrAST = pAST;
        }
    }
}

void Translator::unary_left_fold(const NonTerminal* apNonTerminal)
{
    m_pCurrAST = new UnaryOp(nullptr);
    apNonTerminal->children().at(0)->accept(*this);

    for (size_t i = 1; i < apNonTerminal->children().size(); i++)
    {
        apNonTerminal->children().at(i)->accept(*this);

        if (i < apNonTerminal->children().size() - 1)
        {
            AST* pAST = new UnaryOp(nullptr);
            pAST->add(m_pCurrAST);

            m_pCurrAST->set_parent(pAST);
            m_pCurrAST = pAST;
        }
    }
}
