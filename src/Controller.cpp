#include <iostream>

#include "ASTParser.h"
#include "Controller.h"
#include "Lexer.h"

Controller::Controller(Context acContext) :
    m_cContext(acContext)
{
}

Controller::~Controller()
{
}

void Controller::compile()
{
    //
    // Tokenizing
    //

    Lexer lcLexer(m_cContext);
    lcLexer.tokenize();

    if (m_cContext.settings().StopAfterTokenizing)
    {
        for (const Token* pToken : lcLexer.tokens())
        {
            std::cout << *pToken << std::endl;
        }

        return;
    }

    //
    // Parsing
    //

    ASTParser lcParser(m_cContext, lcLexer.tokens());
    lcParser.parse();

    if (m_cContext.settings().StopAfterParsing)
    {
        ASTVisitor lcVisitor;
        lcParser.root()->accept(lcVisitor);

        return;
    }
}
