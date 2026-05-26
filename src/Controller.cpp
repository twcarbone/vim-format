#include <iostream>

#include "ASTParser.h"
#include "ASTVisitor.h"
#include "Controller.h"
#include "DocBuilder.h"
#include "DocVisitor.h"
#include "Lexer.h"
#include "PrettyPrinter.h"
#include "Renderer.h"

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

    ASTParser lcParser(m_cContext, lcLexer.take_tokens());
    lcParser.parse();

    if (m_cContext.settings().StopAfterParsing)
    {
        ASTVisitor lcVisitor;
        lcParser.root()->accept(lcVisitor);

        return;
    }

    //
    // Pretty-printing
    //

#if 0
    PrettyPrinter lcPrettyPrinter;
    lcParser.root()->accept(lcPrettyPrinter);
#endif

    DocBuilder lcDocBuilder;
    lcParser.root()->accept(lcDocBuilder);

    if (m_cContext.settings().StopAfterDocBuilding)
    {
        DocVisitor lcDocVisitor;
        lcDocBuilder.root()->accept(lcDocVisitor);

        return;
    }

    Renderer lcRenderer;
    lcDocBuilder.root()->accept(lcRenderer);
}
