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

              << " --column-limit    Column limit (default:" + std::to_string(Settings::ColumnLimit) + ")\n"
              << "  -h, --help       Display this help.\n"
              << "  -t               Stop after tokenizing.\n"
              << "  -p               Stop after parsing.\n"
              << "  -d               Stop after doc building." << std::endl;
}

/**
 *  Tip 11-1: gdb
 *
 *  RUN & CONTROL
 *
 *      r[un] [<args>]          Start program with command-line arguments.
 *      c[ontinue]              Execute until breakpoint, signal, or crash.
 *      n[ext]                  Execute next line, stepping *over* function calls.
 *      s[tep]                  Execute next line, stepping *into* function calls.
 *      finish                  Execute until current function returns.
 *      u[ntil] <line>          Run until <line>, or existing current loop.
 *
 *  INSPECT STATE
 *
 *      p <expr>                Print <expr>.
 *      ptype <expr>            Print C/C++ type of <expr>.
 *      l[ist]                  Show source lines around current line.
 *      display <expr>          Print <expr> after every stop.
 *      undisplay <expr>        Stopping printing <expr>.
 *      i[nfo] locals           Show local variables
 *      i[nfo] args             Show arguments passed to function.
 *      i[nfo] variables        Show global and static variables across program.
 *
 *  CALL STACK
 *
 *      backtrace, bt           Print call stack.
 *      up [<n>]                Go up <n> stack frames (default: 1).
 *      down [<n>]              Go down <n> stack frames (default: 1).
 *      f[rame] <num>           Go to stack frame <num> of stack trace.
 *
 *  MODIFICATION
 *
 *      return <expr>           Force the current function to immediately return <expr>.
 *      set var <var> = <expr>  Modify <var> live in memory to <expr>.
 *
 *  BREAKPOINTS
 *
 *      b[reak] <func>          Pause when entering <func>.
 *      b[reak] <file>:<line>   Pause at <line> of <file>.
 *      tb[reak] <file>:<line>  Pause at <line> of <file>.
 *      i[nfo] b[break]         List breakpoints.
 *      delete <num>            Remove breakpoint <num>.
 *      disable <num>           Disable breakpoint <num>.
 *      condition <num> <expr>  Pause at breakpoint <num> only when <expr> is true.
 *
 *  WATCHPOINTS
 *
 *      watch <expr>            Pause when <expr> changes value.
 *      i[nfo] watchpoints      List watchpoints.
 *      delete <num>            Remove watchpoints <num>.
 *      disable <num>           Disable watchpoints <num>.
 *
 *  CATCHPOINTS
 *
 *      catch throw <regex>     Pause when an exception matching <regex> is thrown.
 *      catch catch <regex>     Pause when an exception matching <regex> is caught.
 *
 *  LAYOUT
 *
 *      layout src              Split screen to show source code and (gdb) prompt.
 *
 *  TIPS
 *
 *      Press <enter> on empty prompt to repeat previous command.
 */

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
        else if (lsArg == "-p")
        {
            lcContext.settings().StopAfterParsing = true;
        }
        else if (lsArg == "-d")
        {
            lcContext.settings().StopAfterDocBuilding = true;
        }
        else if (lsArg == "--column-limit")
        {
            i++;
            lcContext.settings().ColumnLimit = std::stoi(argv[i]);
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
