#include <iostream>

#include "Analyzer.h"
#include "Node.h"

Analyzer::Analyzer(const Context& acContext) :
    m_cContext { acContext }
{
}

Analyzer::~Analyzer()
{
}

void Analyzer::analyze(const Node* apRoot)
{
    for (const Node* pNode : accumulate(apRoot, NodeVisitor::Order::PRE))
    {
        std::cout << std::string(2 * pNode->level(), ' ');
        std::cout << pNode->toString() << std::endl;
    }
}
