#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>
#include <unordered_map>

class VimError : public std::runtime_error
{
public:
    explicit VimError(std::string code, std::string context);

    static const std::unordered_map<std::string, std::string> s_mErrors;
};

#endif  // EXCEPTIONS_H
