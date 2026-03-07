#ifndef SETTINGS_H
#define SETTINGS_H

#include <cstddef>

class Settings
{
public:
    static bool StopAfterTokenizing;
    static bool StopAfterParsing;

    static size_t IndentWidth;
};

#endif  // SETTINGS_H
