#ifndef ANALYZER_H
#define ANALYZER_H

#include "Context.h"
#include "Node.h"
#include "NodeVisitor.h"

class Analyzer : public NodeVisitor
{
public:
    Analyzer(const Context& context);
    ~Analyzer();

private:
    const Context& m_cContext;
};

#endif  // ANALYZER_H
