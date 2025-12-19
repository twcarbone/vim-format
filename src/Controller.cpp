#include <iostream>

#include "Controller.h"
#include "Lexer.h"
#include "Node.h"
#include "NodeVisitor.h"
#include "Parser.h"

Controller::Controller(Context acContext) :
    m_cContext(acContext)
{
}

Controller::~Controller()
{
}

void Controller::compile(std::string_view asText)
{
    Lexer lcLexer;
    lcLexer.tokenize(asText);

    if (m_cContext.settings().StopAfterTokenizing)
    {
        for (const Token* pToken : lcLexer.tokens())
        {
            std::cout << *pToken << std::endl;
        }

        return;
    }

    Parser lcParser(lcLexer.tokens());
    lcParser.parse();

    NodeVisitor lcNodeVisitor;
    for (const Node* pNode : lcNodeVisitor.accumulate(lcParser.root(), NodeVisitor::Order::PRE))
    {
        std::cout << std::string(2 * pNode->level(), ' ');
        std::cout << pNode->toString() << std::endl;
    }
}
