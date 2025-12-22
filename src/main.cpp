#include <fstream>
#include <iostream>

#include "Context.h"
#include "Controller.h"

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

    Context lcContext;

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
            lcContext.settings().StopAfterTokenizing = true;
        }
        else if (i == argc - 1)
        {
            char c;

            if (lsArg == "-")
            {
                std::string lsText;

                while (std::cin.get(c))
                {
                    lsText += c;
                }

                lcContext.add_text(lsText);
            }
            else
            {
                try
                {
                    lcContext.add_path(lsArg);
                }
                catch (const std::runtime_error& e)
                {
                    std::cerr << e.what() << std::endl;
                    return 1;
                }
            }
        }
        else
        {
            std::cerr << "Error: unrecognized argument: " << lsArg << std::endl;
            return 1;
        }
    }

    Controller lcController(lcContext);

    try
    {
        lcController.compile();
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
