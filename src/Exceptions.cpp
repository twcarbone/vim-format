#include "Exceptions.h"

// Intended to match Vim src/errors.h exactly
const std::unordered_map<std::string, std::string> VimError::s_mErrors {
    { "E121", "Undefined variable" },
    { "E125", "Illegal argument" },
    { "E475", "Invalid argument" },
    { "E989", "Non-default argument follows default argument" },
};

VimError::VimError(std::string asCode, std::string asTraceback) :
    std::runtime_error(asCode + ": " + s_mErrors.at(asCode) + "\n\n" + asTraceback)
{
}
