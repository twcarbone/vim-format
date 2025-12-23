#include <iostream>

#include "Analyzer.h"
#include "Controller.h"
#include "Lexer.h"
#include "Parser.h"

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

    Parser lcParser(m_cContext, lcLexer.tokens());
    lcParser.parse();

    if (m_cContext.settings().StopAfterParsing)
    {
        NodeVisitor lcNodeVisitor;
        lcParser.root()->accept(lcNodeVisitor);

        return;
    }

    //
    // Semantic analysis
    //

    Analyzer lcAnalyzer(m_cContext);
    lcParser.root()->accept(lcAnalyzer);
}
