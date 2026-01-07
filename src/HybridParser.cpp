#include "HybridParser.h"

HybridParser::HybridParser(const Context& acContext, std::vector<Token*> alTokens) :
    m_pRoot { nullptr },
    m_pCurrToken { nullptr },
    m_cSource { acContext.source() },
    m_nPos { 0 },
    m_lTokens { std::move(alTokens) },
    m_mOpBindingPower {
        // 0
        { Token::Type::OP_OR, { 0, 1 } },
        // 10
        { Token::Type::OP_AND, { 10, 11 } },
        // 20
        { Token::Type::OP_EQUAL, { 20, 21 } },
        { Token::Type::OP_NEQUAL, { 20, 21 } },
        { Token::Type::OP_GT, { 20, 21 } },
        { Token::Type::OP_GTE, { 20, 21 } },
        { Token::Type::OP_LT, { 20, 21 } },
        { Token::Type::OP_LTE, { 20, 21 } },
        { Token::Type::OP_MATCH, { 20, 21 } },
        { Token::Type::OP_NMATCH, { 20, 21 } },
        { Token::Type::OP_ISNOT, { 20, 21 } },
        // 30
        { Token::Type::OP_LSHIFT, { 30, 31 } },
        { Token::Type::OP_RSHIFT, { 30, 31 } },
        // 40
        { Token::Type::OP_ADD, { 40, 41 } },
        { Token::Type::OP_SUB, { 40, 41 } },
        { Token::Type::OP_CAT_OLD, { 40, 41 } },
        { Token::Type::OP_CAT_NEW, { 40, 41 } },
        // 50
        { Token::Type::OP_MUL, { 50, 51 } },
        { Token::Type::OP_DIV, { 50, 51 } },
        { Token::Type::OP_MODULO, { 50, 51 } },
        // 60
        { Token::Type::OP_LOGICAL_NOT, { 60, 61 } },
        { Token::Type::OP_UNARY_MINUS, { 60, 61 } },
        { Token::Type::OP_UNARY_PLUS, { 60, 61 } },
        // 70
        { Token::Type::L_BRACKET, { 70, 0 } },
        { Token::Type::OP_SLICE, { 70, 71 } },
        { Token::Type::OP_DOT, { 70, 71 } },
    }
{
}

HybridParser::~HybridParser()
{
    delete m_pRoot;
}

void HybridParser::parse()
{
    next();
    m_pRoot = program();
}

ast::Program* HybridParser::root() const
{
    return m_pRoot;
}

ast::Program* HybridParser::program()
{
    ast::Program* pProgram = new ast::Program(stmt_list());
    consume(Token::Type::END);
    return pProgram;
}

ast::StmtList* HybridParser::stmt_list()
{
    ast::StmtList* pStmtList = new ast::StmtList();

    while (true)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::ELSE:
            case Token::Type::END:
            case Token::Type::ENDIF:
                return pStmtList;
            case Token::Type::NEWLINE:
                consume(Token::Type::NEWLINE);
                break;
            default:
                pStmtList->push(stmt());
                consume(Token::Type::NEWLINE);
        }
    }
}

ast::Stmt* HybridParser::stmt()
{
    ast::Stmt* pStmt;

    switch (m_pCurrToken->type())
    {
        case Token::Type::IF:
            pStmt = if_stmt();
            break;
        case Token::Type::CMD_ECHO:
            pStmt = expr_cmd();
            break;
        default:
            break;
    }

    return pStmt;
}

ast::IfStmt* HybridParser::if_stmt()
{
    ast::Expr* pExpr = nullptr;
    ast::StmtList* pThenStmts = nullptr;
    ast::StmtList* pElseStmts = nullptr;

    consume(Token::Type::IF);
    pExpr = expr(0);

    pThenStmts = stmt_list();

    if (m_pCurrToken->type() == Token::Type::ELSE)
    {
        consume(Token::Type::ELSE);
    }

    pElseStmts = stmt_list();

    consume(Token::Type::ENDIF);
    return new ast::IfStmt(pExpr, pThenStmts, pElseStmts);
}

ast::ExprCmd* HybridParser::expr_cmd()
{
    Token* pCmd = m_pCurrToken;
    consume(m_pCurrToken->type());
    ast::Expr* pExpr = expr(0);
    return new ast::ExprCmd(pCmd, pExpr);
}

ast::Expr* HybridParser::expr(int anMinBindingPower)
{
    ast::Expr* pLhs = nullptr;
    ast::Expr* pRhs = nullptr;
    ast::Expr* pStart = nullptr;
    ast::Expr* pStop = nullptr;
    Token* pOp = nullptr;

    int lnLhsOpBindingPower;
    int lnRhsOpBindingPower;

    // 1. Parse left-hand side
    switch (m_pCurrToken->type())
    {
        case Token::Type::INTEGER:
        case Token::Type::FLOAT:
            pLhs = new ast::Literal(m_pCurrToken);
            consume(m_pCurrToken->type());
            break;
        case Token::Type::IDENTIFIER:
            pLhs = new ast::Var(m_pCurrToken);
            consume(Token::Type::IDENTIFIER);
            break;
        case Token::Type::L_PAREN:
            consume(Token::Type::L_PAREN);
            pLhs = expr(0);
            consume(Token::Type::R_PAREN);
            break;
        case Token::Type::OP_LOGICAL_NOT:
        case Token::Type::OP_UNARY_MINUS:
        case Token::Type::OP_UNARY_PLUS:
            pOp = m_pCurrToken;
            lnRhsOpBindingPower = m_mOpBindingPower.at(pOp->type()).second;
            consume(m_pCurrToken->type());
            pRhs = expr(lnRhsOpBindingPower);
            pLhs = new ast::UnaryOp(pOp, pRhs);
            break;
        default:
            throw_unexpected_token();
    }

    while (true)
    {
        // 2. Parse operator
        switch (m_pCurrToken->type())
        {
            case Token::Type::NEWLINE:
            case Token::Type::R_PAREN:
            case Token::Type::R_BRACKET:
            case Token::Type::OP_SLICE:
            case Token::Type::END:
                return pLhs;
            case Token::Type::OP_OR:
            case Token::Type::OP_AND:
            case Token::Type::OP_LSHIFT:
            case Token::Type::OP_RSHIFT:
            case Token::Type::OP_ADD:
            case Token::Type::OP_SUB:
            case Token::Type::OP_DOT:
            case Token::Type::OP_CAT_OLD:
            case Token::Type::OP_CAT_NEW:
            case Token::Type::OP_MUL:
            case Token::Type::OP_DIV:
            case Token::Type::OP_MODULO:
            case Token::Type::OP_LOGICAL_NOT:
            case Token::Type::OP_UNARY_MINUS:
            case Token::Type::OP_UNARY_PLUS:
            case Token::Type::L_BRACKET:
                pOp = m_pCurrToken;
                break;
            default:
                throw_unexpected_token();
        }

        // 3. Parse right-hand side
        lnLhsOpBindingPower = m_mOpBindingPower.at(pOp->type()).first;
        lnRhsOpBindingPower = m_mOpBindingPower.at(pOp->type()).second;

        if (lnLhsOpBindingPower < anMinBindingPower)
        {
            return pLhs;
        }

        consume(pOp->type());

        if (pOp->type() == Token::Type::L_BRACKET)
        {
            if (m_pCurrToken->type() != Token::Type::OP_SLICE)
            {
                pStart = expr(0);
            }

            if (m_pCurrToken->type() == Token::Type::OP_SLICE)
            {
                // Note: 'pOp' is declared again in this scope as a slice operator to not
                // shadow the outer scope 'pOp' left-bracket operator.
                Token* pOp = m_pCurrToken;
                consume(Token::Type::OP_SLICE);

                if (m_pCurrToken->type() != Token::Type::R_BRACKET)
                {
                    pStop = expr(0);
                }

                consume(Token::Type::R_BRACKET);
                pRhs = new ast::SliceExpr(pOp, pStart, pStop);
            }
            else
            {
                consume(Token::Type::R_BRACKET);
                pRhs = pStart;
            }
        }
        else
        {
            pRhs = expr(lnRhsOpBindingPower);
        }

        pLhs = new ast::BinaryOp(pOp, pLhs, pRhs);
    }
}

void HybridParser::next()
{
    m_pCurrToken = m_lTokens.at(m_nPos++);
}

void HybridParser::consume(const Token::Type aeType)
{
    if (m_pCurrToken->type() != aeType)
    {
        throw_unexpected_token();
    }

    if (m_pCurrToken->type() == Token::Type::END)
    {
        return;
    }

    while (true)
    {
        next();

        switch (m_pCurrToken->type())
        {
            case Token::Type::TAB:
            case Token::Type::SPACE:
                break;
            default:
                return;
        }
    }
}

void HybridParser::throw_unexpected_token()
{
    m_cSource.seek(m_pCurrToken->source_pos());
    throw std::runtime_error("Unexpected token.\n\n" + m_cSource.traceback());
}
