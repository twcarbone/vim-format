#include <string>

#include "Parser.h"
#include "Token.h"
#include "Tokenizer.h"

Parser::Parser()
{
}

Parser::~Parser()
{
}

Node* Parser::parse(const std::string& asProgram)
{
    m_pTokenizer = new Tokenizer(asProgram);
    m_pLookAheadToken = m_pTokenizer->next();

    return Program();
}

/**
 *  Program
 *      : StatementList
 *      ;
 */
Node* Parser::Program()
{
    return new Node { Token(Token::Type::ROOT, "Program"), StatementList() };
}

/**
 *  StatementList
 *      : Statement
 *      | StatementList Statement -> Statement Statement Statement Statement
 */
Node* Parser::StatementList()
{
}

/**
 *  Statement
 *      : ExpressionStatement
 *      ;
 */
Node* Parser::Statement()
{
    return ExpressionStatement();
}

/**
 *  ExpressionStatement
 *      : Expression ';'
 *      ;
 */
Node* Parser::ExpressionStatement()
{
    Node* pExpression = Expression();
    consume(Token::Type::SPECIAL_SEMICOLON);
    return new Node { Token }
}

/**
 *  Expression
 *      : Literal
 *      ;
 */
Node* Parser::Expression()
{
    return Literal();
}

/**
 *  Literal
 *      : NumericLiteral
 *      | StringLiteral
 *      ;
 */
Node* Parser::Literal()
{
    switch (m_pLookAheadToken->type())
    {
        case Token::Type::LITERAL_INTEGER:
        {
            return IntegerLiteral();
        }
        case Token::Type::LITERAL_STRING:
        {
            return StringLiteral();
        }
        default:
        {
            // TODO: throw SyntaxError
        }
    }

    return nullptr;
}

Node* Parser::IntegerLiteral()
{
    const Token* pToken = consume(Token::Type::LITERAL_INTEGER);
}

const Token* Parser::consume(Token::Type aeType)
{
    const Token* pToken = m_pLookAheadToken;

    if (pToken == nullptr)
    {
        // TODO: throw SyntaxError: unexpected end of text, expected '{type}'
    }

    if (pToken->type() != aeType)
    {
        // TODO: throw SyntaxError: unexpected token: '{type}', expected: '{aeType}'
    }

    m_pLookAheadToken = m_pTokenizer->next();
    return pToken;
}
