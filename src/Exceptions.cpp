#include "Exceptions.h"

// Intended to match Vim src/errors.h exactly
const std::unordered_map<std::string, std::string> VimError::s_mErrors {
    { "E125", "Illegal argument" },
    { "E989", "Non-default argument follows default argument" },
};

VimError::VimError(std::string asCode) :
    std::runtime_error(asCode + ": " + s_mErrors.at(asCode))
{
}
