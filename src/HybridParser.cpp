#include "HybridParser.h"

HybridParser::HybridParser(const Context& acContext, std::vector<Token*> alTokens) :
    m_pRoot { nullptr },
    m_pCurrToken { nullptr },
    m_cSource { acContext.source() },
    m_nPos { 0 },
    m_lTokens { std::move(alTokens) },
    m_mOpBindingPower {
        // 10
        { Token::Type::OP_FALSEY, { 10, 11 } },
        { Token::Type::OP_TERNARY_IF, { 10, 0 } },
        // 20
        { Token::Type::OP_OR, { 20, 21 } },
        // 30
        { Token::Type::OP_AND, { 30, 31 } },
        // 40
        { Token::Type::OP_EQUAL, { 40, 41 } },
        { Token::Type::OP_NEQUAL, { 40, 41 } },
        { Token::Type::OP_GT, { 40, 41 } },
        { Token::Type::OP_GTE, { 40, 41 } },
        { Token::Type::OP_LT, { 40, 41 } },
        { Token::Type::OP_LTE, { 40, 41 } },
        { Token::Type::OP_MATCH, { 40, 41 } },
        { Token::Type::OP_NMATCH, { 40, 41 } },
        { Token::Type::OP_IS, { 40, 41 } },
        { Token::Type::OP_ISNOT, { 40, 41 } },
        // 50
        { Token::Type::OP_LSHIFT, { 50, 51 } },
        { Token::Type::OP_RSHIFT, { 50, 51 } },
        // 60
        { Token::Type::OP_ADD, { 60, 61 } },
        { Token::Type::OP_SUB, { 60, 61 } },
        { Token::Type::OP_CAT_OLD, { 60, 61 } },
        { Token::Type::OP_CAT_NEW, { 60, 61 } },
        // 70
        { Token::Type::OP_MUL, { 70, 71 } },
        { Token::Type::OP_DIV, { 70, 71 } },
        { Token::Type::OP_MODULO, { 70, 71 } },
        // 80
        { Token::Type::OP_LOGICAL_NOT, { 80, 81 } },
        { Token::Type::OP_UNARY_MINUS, { 80, 81 } },
        { Token::Type::OP_UNARY_PLUS, { 80, 81 } },
        // 90
        { Token::Type::L_BRACKET, { 90, 0 } },
        { Token::Type::OP_SLICE, { 90, 91 } },
        { Token::Type::OP_DOT, { 90, 91 } },
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
            case Token::Type::ELSEIF:
            case Token::Type::END:
            case Token::Type::ENDIF:
            case Token::Type::ENDWHILE:
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
        case Token::Type::WHILE:
            pStmt = while_stmt();
            break;
        case Token::Type::CMD_ECHO:
            pStmt = expr_cmd();
            break;
        case Token::Type::CMD_LET:
            pStmt = assign_stmt();
            break;
        default:
            break;
    }

    return pStmt;
}

ast::IfStmt* HybridParser::if_stmt()
{
    switch (m_pCurrToken->type())
    {
        case Token::Type::IF:
        case Token::Type::ELSEIF:
            consume(m_pCurrToken->type());
            break;
        default:
            break;
    }

    ast::Expr* pExpr = expr(0);
    ast::StmtList* pThenStmts = stmt_list();
    ast::StmtList* pElseStmts = nullptr;

    switch (m_pCurrToken->type())
    {
        case Token::Type::ELSEIF:
            pElseStmts = new ast::StmtList();
            pElseStmts->push(if_stmt());
            break;
        case Token::Type::ELSE:
            consume(Token::Type::ELSE);
            pElseStmts = stmt_list();
            consume(Token::Type::ENDIF);
            break;
        default:
            pElseStmts = new ast::StmtList();
            consume(Token::Type::ENDIF);
    }

    return new ast::IfStmt(pExpr, pThenStmts, pElseStmts);
}

ast::WhileStmt* HybridParser::while_stmt()
{
    consume(Token::Type::WHILE);
    ast::Expr* pExpr = expr(0);
    ast::StmtList* pStmts = stmt_list();
    consume(Token::Type::ENDWHILE);
    return new ast::WhileStmt(pExpr, pStmts);
}

ast::AssignStmt* HybridParser::assign_stmt()
{
    Token* pOp = nullptr;
    ast::Var* pVar = nullptr;
    ast::Expr* pExpr = nullptr;

    consume(Token::Type::CMD_LET);

    pVar = new ast::Var(m_pCurrToken);
    consume(Token::Type::IDENTIFIER);

    switch (m_pCurrToken->type())
    {
        case Token::Type::ASSIGN_ADD:
        case Token::Type::ASSIGN_MINUS:
        case Token::Type::ASSIGN_MUL:
        case Token::Type::ASSIGN_DIV:
        case Token::Type::ASSIGN_EQ:
        case Token::Type::ASSIGN_MODULO:
        case Token::Type::ASSIGN_CAT_NEW:
        case Token::Type::ASSIGN_CAT_OLD:
            pOp = m_pCurrToken;
            consume(m_pCurrToken->type());
            pExpr = expr(0);
            break;
        default:
            throw_unexpected_token();
    }

    return new ast::AssignStmt(pOp, pVar, pExpr);
}

ast::ExprCmd* HybridParser::expr_cmd()
{
    Token* pCmd = m_pCurrToken;
    consume(m_pCurrToken->type());
    ast::Expr* pExpr = expr(0);
    return new ast::ExprCmd(pCmd, pExpr);
}

ast::ListExpr* HybridParser::list_expr()
{
    ast::ListExpr* pListExpr = new ast::ListExpr();

    consume(Token::Type::L_BRACKET);

    while (true)
    {
        if (m_pCurrToken->type() == Token::Type::R_BRACKET)
        {
            break;
        }

        pListExpr->push(expr(0));

        if (m_pCurrToken->type() != Token::Type::R_BRACKET)
        {
            consume(Token::Type::COMMA);
        }
    }

    consume(Token::Type::R_BRACKET);
    return pListExpr;
}

ast::Expr* HybridParser::slice_expr()
{
    ast::Expr* pStart = nullptr;
    ast::Expr* pStop = nullptr;
    ast::Expr* pRhs = nullptr;

    if (m_pCurrToken->type() != Token::Type::OP_SLICE)
    {
        pStart = expr(0);
    }

    if (m_pCurrToken->type() == Token::Type::OP_SLICE)
    {
        Token* pOp = m_pCurrToken;
        consume(Token::Type::OP_SLICE);

        if (m_pCurrToken->type() != Token::Type::R_BRACKET)
        {
            pStop = expr(0);
        }

        pRhs = new ast::SliceExpr(pOp, pStart, pStop);
    }
    else
    {
        pRhs = pStart;
    }

    consume(Token::Type::R_BRACKET);
    return pRhs;
}

ast::Expr* HybridParser::expr(int anMinBindingPower)
{
    ast::Expr* pLhs = nullptr;
    ast::Expr* pRhs = nullptr;
    Token* pOp = nullptr;

    int lnLhsOpBindingPower;
    int lnRhsOpBindingPower;

    // 1. Parse left-hand side
    switch (m_pCurrToken->type())
    {
        case Token::Type::INTEGER:
        case Token::Type::FLOAT:
        case Token::Type::STRING:
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
        case Token::Type::L_BRACKET:
            pLhs = list_expr();
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
            case Token::Type::COMMA:
            case Token::Type::NEWLINE:
            case Token::Type::R_PAREN:
            case Token::Type::R_BRACKET:
            case Token::Type::OP_SLICE:
            case Token::Type::OP_TERNARY_ELSE:
            case Token::Type::END:
                return pLhs;
            // expr1
            case Token::Type::OP_FALSEY:
            case Token::Type::OP_TERNARY_IF:
            // expr2
            case Token::Type::OP_OR:
            // expr3
            case Token::Type::OP_AND:
            // expr4
            case Token::Type::OP_EQUAL:
            case Token::Type::OP_NEQUAL:
            case Token::Type::OP_GT:
            case Token::Type::OP_GTE:
            case Token::Type::OP_LT:
            case Token::Type::OP_LTE:
            case Token::Type::OP_MATCH:
            case Token::Type::OP_NMATCH:
            case Token::Type::OP_IS:
            case Token::Type::OP_ISNOT:
            // expr5
            case Token::Type::OP_LSHIFT:
            case Token::Type::OP_RSHIFT:
            // expr6
            case Token::Type::OP_ADD:
            case Token::Type::OP_SUB:
            case Token::Type::OP_DOT:
            case Token::Type::OP_CAT_OLD:
            case Token::Type::OP_CAT_NEW:
            // expr7
            case Token::Type::OP_MUL:
            case Token::Type::OP_DIV:
            case Token::Type::OP_MODULO:
            // expr8
            // expr9
            case Token::Type::OP_LOGICAL_NOT:
            case Token::Type::OP_UNARY_MINUS:
            case Token::Type::OP_UNARY_PLUS:
            // expr10
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

        switch (pOp->type())
        {
            case Token::Type::L_BRACKET:
                pRhs = slice_expr();
                pLhs = new ast::BinaryOp(pOp, pLhs, pRhs);
                break;
            case Token::Type::OP_TERNARY_IF:
            {
                Token* pLeftOp = pOp;
                ast::Expr* pMhs = expr(0);
                Token* pRightOp = m_pCurrToken;
                consume(Token::Type::OP_TERNARY_ELSE);
                pRhs = expr(lnRhsOpBindingPower);
                pLhs = new ast::TernaryOp(pLeftOp, pRightOp, pLhs, pMhs, pRhs);

                break;
            }
            default:
                pRhs = expr(lnRhsOpBindingPower);
                pLhs = new ast::BinaryOp(pOp, pLhs, pRhs);
        }
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
