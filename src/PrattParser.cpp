#include "PrattParser.h"

PrattParser::PrattParser(const Context& acContext, std::vector<Token*> alTokens) :
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
    }
{
}

void PrattParser::parse()
{
    next();

    m_pRoot = parse_expr(0);
}

ast::Node* PrattParser::root() const
{
    return m_pRoot;
}

size_t PrattParser::pos() const
{
    return m_nPos;
}

void PrattParser::seek(size_t anPos)
{
    m_nPos = anPos;
    next();
}

void PrattParser::next()
{
    m_pCurrToken = m_lTokens.at(m_nPos++);
}

void PrattParser::consume(Token::Type aeType)
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

void PrattParser::throw_unexpected_token()
{
    m_cSource.seek(m_pCurrToken->source_pos());
    throw std::runtime_error("Unexpected token.\n\n" + m_cSource.traceback());
}

ast::Node* PrattParser::parse_expr(int anMinBindingPower)
{
    ast::Node* pLhs = nullptr;
    ast::Node* pRhs = nullptr;
    ast::Node* pStart = nullptr;
    ast::Node* pStop = nullptr;
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
            pLhs = parse_expr(0);
            consume(Token::Type::R_PAREN);
            break;
        case Token::Type::OP_LOGICAL_NOT:
        case Token::Type::OP_UNARY_MINUS:
        case Token::Type::OP_UNARY_PLUS:
            pOp = m_pCurrToken;
            lnRhsOpBindingPower = m_mOpBindingPower.at(pOp->type()).second;
            consume(m_pCurrToken->type());
            pRhs = parse_expr(lnRhsOpBindingPower);
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
            case Token::Type::END:
            case Token::Type::R_PAREN:
            case Token::Type::R_BRACKET:
            case Token::Type::OP_SLICE:
                return pLhs;
            case Token::Type::OP_OR:
            case Token::Type::OP_AND:
            case Token::Type::OP_LSHIFT:
            case Token::Type::OP_RSHIFT:
            case Token::Type::OP_ADD:
            case Token::Type::OP_SUB:
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
                pStart = parse_expr(0);
            }

            if (m_pCurrToken->type() == Token::Type::OP_SLICE)
            {
                // Note: 'pOp' is declared again in this scope as a slice operator to not
                // shadow the outer scope 'pOp' left-bracket operator.
                Token* pOp = m_pCurrToken;
                consume(Token::Type::OP_SLICE);

                if (m_pCurrToken->type() != Token::Type::R_BRACKET)
                {
                    pStop = parse_expr(0);
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
            pRhs = parse_expr(lnRhsOpBindingPower);
        }

        pLhs = new ast::BinaryOp(pOp, pLhs, pRhs);
    }
}
