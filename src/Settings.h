#ifndef SETTINGS_H
#define SETTINGS_H

#include <cstddef>

class Settings
{
public:
    static bool StopAfterTokenizing;
    static bool StopAfterParsing;

    static size_t ControlStmtPadding;
    static size_t CurlyBracePadding;
    static size_t DefaultFnParamPadding;
    static size_t FnModifierPadding;
    static size_t IndentWidth;
    static size_t OperatorPadding;
    static size_t ParenPadding;
    static size_t ReturnStmtPadding;
    static size_t SpaceAfterDictKey;
    static size_t SpaceAfterDictSeparator;
    static size_t SpaceAfterExprCmd;
    static size_t SpaceAfterFnArgSeparator;
    static size_t SpaceAfterFnParamSeparator;
    static size_t SpaceAfterListSeparator;
    static size_t SpaceBeforeDictValue;
    static size_t SpaceBeforeFunctionName;
    static size_t SquareBracketPadding;
};

#endif  // SETTINGS_H
