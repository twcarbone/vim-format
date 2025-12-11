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

/**
 *  program
 *          : statement_list END
 */
void Parser::program()
{
    RuleNode* pRuleNode = new RuleNode(nullptr, __func__);

    m_pRoot = pRuleNode;

    statement_list(pRuleNode);
    consume(pRuleNode, Token::Type::END);
}

/**
 *  statement_list
 *          : statement
 *          | statement_list statement
 */
void Parser::statement_list(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    statement(pRuleNode);

    while (m_pCurrToken->type() != Token::Type::END)
    {
        statement_list(pRuleNode);
    }
}

/**
 *  statement
 *          : additive_expr SEMI
 */
void Parser::statement(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    additive_expr(pRuleNode);

    consume(pRuleNode, Token::Type::SEMICOLON);
}

/**
 *  additive_expr
 *          : multiplicative_expr
 *          | additive_expr OP_ADD multiplicative_expr
 *          | additive_expr OP_SUB multiplicative_expr
 */
void Parser::additive_expr(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    multiplicative_expr(pRuleNode);

    while (true)
    {
        if (m_pCurrToken->type() == Token::Type::OP_ADD)
        {
            consume(pRuleNode, Token::Type::OP_ADD);
        }
        else if (m_pCurrToken->type() == Token::Type::OP_SUB)
        {
            consume(pRuleNode, Token::Type::OP_SUB);
        }
        else
        {
            break;
        }

        additive_expr(pRuleNode);
    }
}

/**
 *  multiplicative_expr
 *          : factor
 *          | multiplicative_expr OP_MUL factor
 *          | multiplicative_expr OP_DIV factor
 */
void Parser::multiplicative_expr(Node* apParent)
{
    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    factor(pRuleNode);

    while (true)
    {
        if (m_pCurrToken->type() == Token::Type::OP_MUL)
        {
            consume(pRuleNode, Token::Type::OP_MUL);
        }
        else if (m_pCurrToken->type() == Token::Type::OP_DIV)
        {
            consume(pRuleNode, Token::Type::OP_DIV);
        }
        else
        {
            break;
        }

        multiplicative_expr(pRuleNode);
    }
}

/**
 *  factor
 *          : OP_PLUS factor
 *          | OP_MINUS factor
 *          | INTEGER
 *          | FLOAT
 *          | L_PAREN additive_expr R_PAREN
 */
void Parser::factor(Node* apParent)
{

    RuleNode* pRuleNode = new RuleNode(apParent, __func__);
    apParent->add(pRuleNode);

    if (m_pCurrToken->type() == Token::Type::OP_MINUS)
    {
        consume(pRuleNode, Token::Type::OP_MINUS);
        factor(pRuleNode);
    }
    else if (m_pCurrToken->type() == Token::Type::OP_PLUS)
    {
        consume(pRuleNode, Token::Type::OP_PLUS);
        factor(pRuleNode);
    }
    else if (m_pCurrToken->type() == Token::Type::INTEGER)
    {
        consume(pRuleNode, Token::Type::INTEGER);
    }
    else if (m_pCurrToken->type() == Token::Type::FLOAT)
    {
        consume(pRuleNode, Token::Type::FLOAT);
    }
    else
    {
        consume(pRuleNode, Token::Type::L_PAREN);
        additive_expr(pRuleNode);
        consume(pRuleNode, Token::Type::R_PAREN);
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

    m_pCurrToken = m_pTokenizer->next();

    while (m_pCurrToken->whitespace())
    {
        m_pCurrToken = m_pTokenizer->next();
    }
}
