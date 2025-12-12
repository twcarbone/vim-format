#include <string>

#include "Parser.h"
#include "Token.h"
#include "Tokenizer.h"

Parser::Parser() :
    m_pRoot { nullptr },
    m_pCurrToken { nullptr },
    m_pTokenizer { nullptr }
{
}

Parser::~Parser()
{
    delete m_pRoot;
    delete m_pTokenizer;
}

void Parser::parse(const std::string& asText)
{
    m_pTokenizer = new Tokenizer(asText);
    m_pCurrToken = m_pTokenizer->next();

    program();
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

    while (m_pCurrToken->type() != Token::Type::END)
    {
        if (m_pCurrToken->type() != Token::Type::NEWLINE)
        {
            stmt(pRuleNode);
        }

        consume(pRuleNode, Token::Type::NEWLINE);
    }
}

void Parser::stmt(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    if (m_pCurrToken->type() == Token::Type::CMD_ECHO)
    {
        consume(pRuleNode, Token::Type::CMD_ECHO);
        expr6(pRuleNode);
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
            consume(pRuleNode, m_pCurrToken->type());
            break;
        case Token::Type::L_PAREN:
            consume(pRuleNode, m_pCurrToken->type());
            expr6(pRuleNode);
            consume(pRuleNode, m_pCurrToken->type());
        default:
            throw std::runtime_error("Expected expr11, got " + Token::TypeToStr(m_pCurrToken->type()));
    }
}

void Parser::consume(Node* apParent, const Token::Type aeType)
{
    if (m_pCurrToken->type() != aeType)
    {
        throw std::runtime_error("Expected " + Token::TypeToStr(aeType) + ", got "
                                 + Token::TypeToStr(m_pCurrToken->type()));
    }

    apParent->add(new TokenNode(apParent, m_pCurrToken));

    while (true)
    {
        m_pCurrToken = m_pTokenizer->next();

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
