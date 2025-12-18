#include <string>

#include "Exceptions.h"
#include "Lexer.h"
#include "Parser.h"
#include "Token.h"

Parser::Parser() :
    m_pRoot { nullptr },
    m_pCurrToken { nullptr },
    m_pLexer { nullptr }
{
}

Parser::~Parser()
{
    delete m_pRoot;
    delete m_pLexer;
}

void Parser::parse(const std::string& asText)
{
    m_pLexer = new Lexer(asText);
    m_pCurrToken = m_pLexer->next();

    program();
}

const std::string& Parser::text() const
{
    return m_pLexer->text();
}

Node* Parser::root() const
{
    return m_pRoot;
}

void Parser::program()
{
    RuleNode* pRuleNode = new RuleNode(nullptr, __func__);

    m_pRoot = pRuleNode;

    if (m_pCurrToken->type() != Token::Type::END)
    {
        stmt_list(pRuleNode);
    }

    consume(pRuleNode, Token::Type::END);
}

void Parser::stmt_list(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    while (true)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::ELSEIF:
            case Token::Type::ELSE:
            case Token::Type::ENDIF:
            case Token::Type::ENDWHILE:
            case Token::Type::ENDFOR:
            case Token::Type::END:
            case Token::Type::ENDFUNCTION:
                return;
            default:
                stmt(pRuleNode);
                consume(pRuleNode, Token::Type::NEWLINE);
        }
    }
}

void Parser::stmt(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    switch (m_pCurrToken->type())
    {
        case Token::Type::IF:
            select_stmt(pRuleNode);
            break;
        case Token::Type::FOR:
        case Token::Type::WHILE:
            iteration_stmt(pRuleNode);
            break;
        case Token::Type::FUNCTION:
            function_stmt(pRuleNode);
            break;
        case Token::Type::BREAK:
        case Token::Type::CONTINUE:
            // TODO (gh-5): BREAK and CONTINUE are only allowed during an iteration
            consume(pRuleNode, m_pCurrToken->type());
            break;
        case Token::Type::CMD_ECHO:
            consume(pRuleNode, Token::Type::CMD_ECHO);
            expr1(pRuleNode);
            break;
        case Token::Type::CMD_LET:
            consume(pRuleNode, Token::Type::CMD_LET);
            consume(pRuleNode, Token::Type::IDENTIFIER);
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
                    consume(pRuleNode, m_pCurrToken->type());
                    expr1(pRuleNode);
                default:
                    break;
            }
        default:
            break;
    }
}

void Parser::select_stmt(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    consume(pRuleNode, Token::Type::IF);
    expr1(pRuleNode);
    stmt_list(pRuleNode);

    while (m_pCurrToken->type() == Token::Type::ELSEIF)
    {
        consume(pRuleNode, Token::Type::ELSEIF);
        expr1(pRuleNode);
        stmt_list(pRuleNode);
    }

    if (consume_optional(pRuleNode, Token::Type::ELSE))
    {
        stmt_list(pRuleNode);
    }

    consume(pRuleNode, Token::Type::ENDIF);
}

void Parser::iteration_stmt(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    if (m_pCurrToken->type() == Token::Type::WHILE)
    {
        consume(pRuleNode, Token::Type::WHILE);
        expr1(pRuleNode);
        stmt_list(pRuleNode);
        consume(pRuleNode, Token::Type::ENDWHILE);
    }
    else if (m_pCurrToken->type() == Token::Type::FOR)
    {
        consume(pRuleNode, Token::Type::FOR);
        consume(pRuleNode, Token::Type::IDENTIFIER);
        consume(pRuleNode, Token::Type::IN);

        switch (m_pCurrToken->type())
        {
            case Token::Type::STRING:
            case Token::Type::IDENTIFIER:
                consume(pRuleNode, m_pCurrToken->type());
                break;
            default:
                list_expr(pRuleNode);
        }

        stmt_list(pRuleNode);
        consume(pRuleNode, Token::Type::ENDFOR);
    }
}

void Parser::function_stmt(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    consume(pRuleNode, Token::Type::FUNCTION);
    consume_optional(pRuleNode, Token::Type::OP_BANG);
    consume(pRuleNode, Token::Type::IDENTIFIER);
    consume(pRuleNode, Token::Type::L_PAREN);

    if (m_pCurrToken->type() != Token::Type::R_PAREN)
    {
        arg_list(pRuleNode);
    }

    consume(pRuleNode, Token::Type::R_PAREN);

    bool lbLoop = true;
    while (lbLoop)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::FN_RANGE:
            case Token::Type::FN_ABORT:
            case Token::Type::FN_DICT:
            case Token::Type::FN_CLOSURE:
                consume(pRuleNode, m_pCurrToken->type());
                break;
            default:
                lbLoop = false;
        }
    }

    stmt_list(pRuleNode);
    consume(pRuleNode, Token::Type::ENDFUNCTION);
}

void Parser::arg_list(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    bool lbGotDefaultArg = false;
    while (m_pCurrToken->type() != Token::Type::R_PAREN)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::FN_ELLIPSES:
                consume(pRuleNode, Token::Type::FN_ELLIPSES);
                consume_optional(pRuleNode, Token::Type::COMMA);
                return;
            case Token::Type::IDENTIFIER:
                consume(pRuleNode, Token::Type::IDENTIFIER);

                if (consume_optional(pRuleNode, Token::Type::ASSIGN_EQ))
                {
                    lbGotDefaultArg = true;
                    expr1(pRuleNode);
                }
                else if (lbGotDefaultArg)
                {
                    throw VimError("E989");
                }

                consume_optional(pRuleNode, Token::Type::COMMA);
                break;
            default:
                throw VimError("E125");
        }
    }
}

void Parser::list_expr(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    consume(pRuleNode, Token::Type::L_BRACKET);

    while (true)
    {
        if (m_pCurrToken->type() == Token::Type::R_BRACKET)
        {
            break;
        }

        expr1(pRuleNode);

        if (m_pCurrToken->type() != Token::Type::R_BRACKET)
        {
            consume(pRuleNode, Token::Type::COMMA);
        }
    }

    consume(pRuleNode, Token::Type::R_BRACKET);
}

void Parser::expr1(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    expr2(pRuleNode);

    switch (m_pCurrToken->type())
    {
        case Token::Type::OP_FALSEY:
            consume(pRuleNode, Token::Type::OP_FALSEY);
            expr1(pRuleNode);
            break;
        case Token::Type::OP_TERNARY_IF:
            consume(pRuleNode, Token::Type::OP_TERNARY_IF);
            expr1(pRuleNode);
            consume(pRuleNode, Token::Type::OP_TERNARY_ELSE);
            expr1(pRuleNode);
            break;
        default:
            break;
    }
}

void Parser::expr2(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    expr3(pRuleNode);

    while (m_pCurrToken->type() == Token::Type::OP_OR)
    {
        consume(pRuleNode, Token::Type::OP_OR);
        expr3(pRuleNode);
    }
}

void Parser::expr3(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    expr4(pRuleNode);

    while (m_pCurrToken->type() == Token::Type::OP_AND)
    {
        consume(pRuleNode, Token::Type::OP_AND);
        expr4(pRuleNode);
    }
}

void Parser::expr4(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    expr5(pRuleNode);

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
            consume(pRuleNode, m_pCurrToken->type());
            switch (m_pCurrToken->type())
            {
                case Token::Type::OP_MATCH_CASE:
                case Token::Type::OP_IGNORE_CASE:
                    consume(pRuleNode, m_pCurrToken->type());
                    break;
                default:
                    break;
            }

            expr5(pRuleNode);
            break;
        default:
            break;
    }
}

void Parser::expr5(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    expr6(pRuleNode);

    while (true)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::OP_LSHIFT:
            case Token::Type::OP_RSHIFT:
                consume(pRuleNode, m_pCurrToken->type());
                expr6(pRuleNode);
                break;
            default:
                return;
        }
    }
}

void Parser::expr6(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    expr7(pRuleNode);

    while (true)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::OP_ADD:
            case Token::Type::OP_SUB:
            case Token::Type::OP_CAT_OLD:
            case Token::Type::OP_CAT_NEW:
                consume(pRuleNode, m_pCurrToken->type());
                expr7(pRuleNode);
                break;
            default:
                return;
        }
    }
}

void Parser::expr7(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    expr8(pRuleNode);

    while (true)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::OP_MUL:
            case Token::Type::OP_DIV:
            case Token::Type::OP_MODULO:
                consume(pRuleNode, m_pCurrToken->type());
                expr8(pRuleNode);
                break;
            default:
                return;
        }
    }
}

void Parser::expr8(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    expr9(pRuleNode);
}

void Parser::expr9(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    switch (m_pCurrToken->type())
    {
        case Token::Type::OP_LOGICAL_NOT:
        case Token::Type::OP_UNARY_MINUS:
        case Token::Type::OP_UNARY_PLUS:
            consume(pRuleNode, m_pCurrToken->type());
            expr9(pRuleNode);
            break;
        default:
            expr10(pRuleNode);
    }
}

void Parser::expr10(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    expr11(pRuleNode);
}

void Parser::expr11(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    switch (m_pCurrToken->type())
    {
        case Token::Type::INTEGER:
        case Token::Type::FLOAT:
        case Token::Type::STRING:
        case Token::Type::IDENTIFIER:
            consume(pRuleNode, m_pCurrToken->type());
            break;
        case Token::Type::L_PAREN:
            consume(pRuleNode, Token::Type::L_PAREN);
            expr1(pRuleNode);
            consume(pRuleNode, Token::Type::R_PAREN);
            break;
        case Token::Type::L_BRACKET:
            list_expr(pRuleNode);
            break;
        default:
            throw std::runtime_error("Invalid expression: " + m_pCurrToken->str());
    }
}

void Parser::consume(Node* apParent, const Token::Type aeType)
{
    if (m_pCurrToken->type() != aeType)
    {
        throw std::runtime_error("Expected " + Token::TypeToStr(aeType) + ", got "
                                 + Token::TypeToStr(m_pCurrToken->type()));
    }

    Node* pTokenNode = new TokenNode(apParent, m_pCurrToken);
    apParent->add(pTokenNode);

    while (true)
    {
        m_pCurrToken = m_pLexer->next();

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
