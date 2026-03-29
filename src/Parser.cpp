#include <string>

#include "Exceptions.h"
#include "Parser.h"
#include "Token.h"

Parser::Parser() :
    m_pRoot { nullptr },
    m_pCurrToken { nullptr }
{
}

Parser::Parser(const Context& acContext, std::vector<Token*> alTokens) :
    m_cSource { acContext.source() },
    m_lTokens { std::move(alTokens) }
{
}

Parser::~Parser()
{
    delete m_pRoot;
}

void Parser::parse()
{
    next();
    program();
}

Node* Parser::root() const
{
    return m_pRoot;
}

void Parser::program()
{
    NonTerminal* pNonTerminal = new NonTerminal(nullptr, __func__);

    m_pRoot = pNonTerminal;

    if (m_pCurrToken->type() != Token::Type::END)
    {
        stmt_list(pNonTerminal);
    }

    consume(pNonTerminal, Token::Type::END);
}

void Parser::stmt_list(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    while (true)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::NEWLINE:
                consume(pNonTerminal, Token::Type::NEWLINE);
                break;
            case Token::Type::ELSEIF:
            case Token::Type::ELSE:
            case Token::Type::ENDIF:
            case Token::Type::ENDWHILE:
            case Token::Type::ENDFOR:
            case Token::Type::END:
            case Token::Type::ENDFUNCTION:
                return;
            default:
                stmt(pNonTerminal);
                consume(pNonTerminal, Token::Type::NEWLINE);
        }
    }
}

void Parser::stmt(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    switch (m_pCurrToken->type())
    {
        case Token::Type::IF:
            select_stmt(pNonTerminal);
            break;
        case Token::Type::FOR:
        case Token::Type::WHILE:
            iteration_stmt(pNonTerminal);
            break;
        case Token::Type::FUNCTION:
            fn_stmt(pNonTerminal);
            break;
        case Token::Type::BREAK:
        case Token::Type::CONTINUE:
            // TODO (gh-5): BREAK and CONTINUE are only allowed during an iteration
            consume(pNonTerminal, m_pCurrToken->type());
            break;
        case Token::Type::CMD_ECHO:
            consume(pNonTerminal, Token::Type::CMD_ECHO);
            expr1(pNonTerminal);
            break;
        case Token::Type::CMD_LET:
            consume(pNonTerminal, Token::Type::CMD_LET);
            consume(pNonTerminal, Token::Type::IDENTIFIER);
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
                    consume(pNonTerminal, m_pCurrToken->type());
                    expr1(pNonTerminal);
                default:
                    break;
            }
        default:
            break;
    }
}

void Parser::select_stmt(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    consume(pNonTerminal, Token::Type::IF);
    expr1(pNonTerminal);
    stmt_list(pNonTerminal);

    while (m_pCurrToken->type() == Token::Type::ELSEIF)
    {
        consume(pNonTerminal, Token::Type::ELSEIF);
        expr1(pNonTerminal);
        stmt_list(pNonTerminal);
    }

    if (consume_optional(pNonTerminal, Token::Type::ELSE))
    {
        stmt_list(pNonTerminal);
    }

    consume(pNonTerminal, Token::Type::ENDIF);
}

void Parser::iteration_stmt(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    if (m_pCurrToken->type() == Token::Type::WHILE)
    {
        consume(pNonTerminal, Token::Type::WHILE);
        expr1(pNonTerminal);
        stmt_list(pNonTerminal);
        consume(pNonTerminal, Token::Type::ENDWHILE);
    }
    else if (m_pCurrToken->type() == Token::Type::FOR)
    {
        consume(pNonTerminal, Token::Type::FOR);
        consume(pNonTerminal, Token::Type::IDENTIFIER);
        consume(pNonTerminal, Token::Type::IN);

        switch (m_pCurrToken->type())
        {
            case Token::Type::STRING:
            case Token::Type::IDENTIFIER:
                consume(pNonTerminal, m_pCurrToken->type());
                break;
            default:
                list_expr(pNonTerminal);
        }

        stmt_list(pNonTerminal);
        consume(pNonTerminal, Token::Type::ENDFOR);
    }
}

void Parser::fn_stmt(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    consume(pNonTerminal, Token::Type::FUNCTION);
    consume_optional(pNonTerminal, Token::Type::OP_BANG);
    consume(pNonTerminal, Token::Type::IDENTIFIER);
    consume(pNonTerminal, Token::Type::L_PAREN);

    if (m_pCurrToken->type() != Token::Type::R_PAREN)
    {
        param_list(pNonTerminal);
    }

    consume(pNonTerminal, Token::Type::R_PAREN);

    bool lbLoop = true;
    while (lbLoop)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::FN_RANGE:
            case Token::Type::FN_ABORT:
            case Token::Type::FN_DICT:
            case Token::Type::FN_CLOSURE:
                consume(pNonTerminal, m_pCurrToken->type());
                break;
            default:
                lbLoop = false;
        }
    }

    stmt_list(pNonTerminal);
    consume(pNonTerminal, Token::Type::ENDFUNCTION);
}

void Parser::param_list(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    bool lbGotDefaultParam = false;
    while (m_pCurrToken->type() != Token::Type::R_PAREN)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::FN_ELLIPSES:
                consume(pNonTerminal, Token::Type::FN_ELLIPSES);
                consume_optional(pNonTerminal, Token::Type::COMMA);
                return;
            case Token::Type::IDENTIFIER:
                consume(pNonTerminal, Token::Type::IDENTIFIER);

                if (consume_optional(pNonTerminal, Token::Type::ASSIGN_EQ))
                {
                    lbGotDefaultParam = true;
                    expr1(pNonTerminal);
                }
                else if (lbGotDefaultParam)
                {
                    m_cSource.seek(m_pCurrToken->source_pos());
                    throw VimError("E989", m_cSource.context());
                }

                consume_optional(pNonTerminal, Token::Type::COMMA);
                break;
            default:
                m_cSource.seek(m_pCurrToken->source_pos());
                throw VimError("E125", m_cSource.context());
        }
    }
}

void Parser::list_expr(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    consume(pNonTerminal, Token::Type::L_BRACKET);

    while (true)
    {
        if (m_pCurrToken->type() == Token::Type::R_BRACKET)
        {
            break;
        }

        expr1(pNonTerminal);

        if (m_pCurrToken->type() != Token::Type::R_BRACKET)
        {
            consume(pNonTerminal, Token::Type::COMMA);
        }
    }

    consume(pNonTerminal, Token::Type::R_BRACKET);
}

void Parser::expr1(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    expr2(pNonTerminal);

    switch (m_pCurrToken->type())
    {
        case Token::Type::OP_FALSEY:
            consume(pNonTerminal, Token::Type::OP_FALSEY);
            expr1(pNonTerminal);
            break;
        case Token::Type::OP_TERNARY_IF:
            consume(pNonTerminal, Token::Type::OP_TERNARY_IF);
            expr1(pNonTerminal);
            consume(pNonTerminal, Token::Type::COLON);
            expr1(pNonTerminal);
            break;
        default:
            break;
    }
}

void Parser::expr2(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    expr3(pNonTerminal);

    while (m_pCurrToken->type() == Token::Type::OP_OR)
    {
        consume(pNonTerminal, Token::Type::OP_OR);
        expr3(pNonTerminal);
    }
}

void Parser::expr3(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    expr4(pNonTerminal);

    while (m_pCurrToken->type() == Token::Type::OP_AND)
    {
        consume(pNonTerminal, Token::Type::OP_AND);
        expr4(pNonTerminal);
    }
}

void Parser::expr4(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    expr5(pNonTerminal);

    switch (m_pCurrToken->type())
    {
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
            consume(pNonTerminal, m_pCurrToken->type());
            switch (m_pCurrToken->type())
            {
                case Token::Type::OP_MATCH_CASE:
                case Token::Type::OP_IGNORE_CASE:
                    consume(pNonTerminal, m_pCurrToken->type());
                    break;
                default:
                    break;
            }

            expr5(pNonTerminal);
            break;
        default:
            break;
    }
}

void Parser::expr5(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    expr6(pNonTerminal);

    while (true)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::OP_LSHIFT:
            case Token::Type::OP_RSHIFT:
                consume(pNonTerminal, m_pCurrToken->type());
                expr6(pNonTerminal);
                break;
            default:
                return;
        }
    }
}

void Parser::expr6(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    expr7(pNonTerminal);

    while (true)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::OP_ADD:
            case Token::Type::OP_SUB:
            case Token::Type::OP_CAT_OLD:
            case Token::Type::OP_CAT_NEW:
                consume(pNonTerminal, m_pCurrToken->type());
                expr7(pNonTerminal);
                break;
            default:
                return;
        }
    }
}

void Parser::expr7(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    expr8(pNonTerminal);

    while (true)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::OP_MUL:
            case Token::Type::OP_DIV:
            case Token::Type::OP_MODULO:
                consume(pNonTerminal, m_pCurrToken->type());
                expr8(pNonTerminal);
                break;
            default:
                return;
        }
    }
}

void Parser::expr8(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    expr9(pNonTerminal);
}

void Parser::expr9(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    switch (m_pCurrToken->type())
    {
        case Token::Type::OP_LOGICAL_NOT:
        case Token::Type::OP_UNARY_MINUS:
        case Token::Type::OP_UNARY_PLUS:
            consume(pNonTerminal, m_pCurrToken->type());
            expr9(pNonTerminal);
            break;
        default:
            expr10(pNonTerminal);
    }
}

void Parser::expr10(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    expr11(pNonTerminal);
}

void Parser::expr11(Node* apParent)
{
    NonTerminal* pNonTerminal = new NonTerminal(apParent, __func__);
    apParent->add(pNonTerminal);

    switch (m_pCurrToken->type())
    {
        case Token::Type::INTEGER:
        case Token::Type::FLOAT:
        case Token::Type::STRING:
        case Token::Type::IDENTIFIER:
            consume(pNonTerminal, m_pCurrToken->type());
            break;
        case Token::Type::L_PAREN:
            consume(pNonTerminal, Token::Type::L_PAREN);
            expr1(pNonTerminal);
            consume(pNonTerminal, Token::Type::R_PAREN);
            break;
        case Token::Type::L_BRACKET:
            list_expr(pNonTerminal);
            break;
        default:
            m_cSource.seek(m_pCurrToken->source_pos());
            throw std::runtime_error("Invalid expression.\n\n" + m_cSource.context());
    }
}

void Parser::next()
{
    m_pCurrToken = m_lTokens.at(m_nCurrTokenIdx++);
}

void Parser::consume(Node* apParent, const Token::Type aeType)
{
    if (m_pCurrToken->type() != aeType)
    {
        m_cSource.seek(m_pCurrToken->source_pos());
        throw std::runtime_error("Expected " + Token::TypeToStr(aeType) + ".\n\n" + m_cSource.context());
    }

    Node* pTerminal = new Terminal(apParent, m_pCurrToken);
    apParent->add(pTerminal);

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

bool Parser::consume_optional(Node* apParent, const Token::Type aeType)
{
    if (m_pCurrToken->type() == aeType)
    {
        consume(apParent, aeType);
        return true;
    }

    return false;
}
