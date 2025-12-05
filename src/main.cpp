#include <iostream>

#include "Tokenizer.h"

void usage()
{
    std::cout << "OVERVIEW: A tool to format legacy (< Vim9) vimscript code."

              << "\n\nUSAGE: vim-format [options] <file>"

              << "\n\nREQUIRED ARGUMENTS:\n\n"

              << "  <file>           File to format. '-' means read from stdin."

              << "\n\nOPTIONS:\n\n"

              << "  -h, --help       Display this help." << std::endl;
}

int main(int argc, char** argv)
{
    if (argc == 1)
    {
        std::cerr << "Error: missing <file>" << std::endl;
        return 1;
    }

    std::string lsText;

    for (int i = 1; i < argc; i++)
    {
        std::string lsArg = argv[i];

        if ((lsArg == "-h") || (lsArg == "--help"))
        {
            usage();
            return 0;
        }
        else if (lsArg == "-")
        {
            char c;
            while (std::cin.get(c))
            {
                lsText += c;
            }
        }
        else
        {
            std::cerr << "Error: unrecognized argument: " << lsArg << std::endl;
            return 1;
        }
    }

    Tokenizer lcTokenizer(lsText);
    lcTokenizer.tokenize();

    std::cout << lcTokenizer.tokens()->size() << " tokens:" << std::endl;

    for (const Token* pToken : *lcTokenizer.tokens())
    {
        std::cout << "    " << pToken->toString() << std::endl;
    }
}
