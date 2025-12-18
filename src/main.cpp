#include <fstream>
#include <iostream>

#include "Lexer.h"
#include "Node.h"
#include "NodeVisitor.h"
#include "Parser.h"

void usage()
{
    std::cout << "OVERVIEW: A tool to format legacy (< Vim9) vimscript code."

              << "\n\nUSAGE: vim-format [options] <file>"

              << "\n\nREQUIRED ARGUMENTS:\n\n"

              << "  <file>           File to format. '-' means read from stdin."

              << "\n\nOPTIONS:\n\n"

              << "  -h, --help       Display this help.\n"
              << "  -t               Stop after tokenizing." << std::endl;
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        std::cerr << "Error: missing <file>" << std::endl;
        return 1;
    }

    bool lbStopAfterTokenizing = false;
    std::string lsText;

    for (int i = 1; i < argc; i++)
    {
        std::string lsArg = argv[i];

        if ((lsArg == "-h") || (lsArg == "--help"))
        {
            usage();
            return 0;
        }
        else if (lsArg == "-t")
        {
            lbStopAfterTokenizing = true;
        }
        else if (i == argc - 1)
        {
            char c;

            if (lsArg == "-")
            {
                while (std::cin.get(c))
                {
                    lsText += c;
                }
            }
            else
            {
                std::ifstream lcIfStream { lsArg };

                if (!lcIfStream.is_open())
                {
                    std::cerr << "Error: cannot open for reading: " << lsArg << std::endl;
                    return 1;
                }

                while (lcIfStream.get(c))
                {
                    lsText += c;
                }
            }
        }
        else
        {
            std::cerr << "Error: unrecognized argument: " << lsArg << std::endl;
            return 1;
        }
    }

    if (lbStopAfterTokenizing)
    {
        Lexer lcLexer;

        try
        {
            lcLexer.tokenize(lsText);
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << e.what() << std::endl;
            return 1;
        }

        for (const Token* pToken : lcLexer.tokens())
        {
            std::cout << *pToken << std::endl;
        }

        return 0;
    }

    Parser lcParser;

    try
    {
        lcParser.parse(lsText);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    NodeVisitor lcNodeVisitor;
    for (const Node* pNode : lcNodeVisitor.accumulate(lcParser.root(), NodeVisitor::Order::PRE))
    {
        std::cout << std::string(2 * pNode->level(), ' ');
        std::cout << pNode->toString() << std::endl;
    }
}
