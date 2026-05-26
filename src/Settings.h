#ifndef SETTINGS_H
#define SETTINGS_H

#include <cstddef>
#include <string>

class Settings
{
public:
    enum class Style
    {
        KR,
        Allman,
    };

    static bool StopAfterTokenizing;
    static bool StopAfterParsing;
    static bool StopAfterDocBuilding;

    static Style BreakStyle;
    static size_t ControlStmtPadding;
    static size_t CurlyBracePadding;
    static size_t DefaultFnParamPadding;
    static size_t ExCmdModifierPadding;
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
    static size_t SpaceBeforeTrailingComment;
    static size_t SquareBracketPadding;
    static size_t SpaceAfterRange;
    static size_t ColumnLimit;
};

#endif  // SETTINGS_H
