#ifndef TOKENSPEC_H
#define TOKENSPEC_H

#include <map>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "Token.h"

class TokenSpec
{
public:
    TokenSpec();
    ~TokenSpec();

    Token* match(const std::string& text);

private:
    const std::vector<std::pair<std::regex, Token::Type> > m_lReSpec;

    const std::vector<std::string> m_lDelimitedSpecKeys;
    const std::map<std::string, Token::Type> m_mDelimitedSpec;

    const std::vector<std::string> m_lFixedWidthSpecKeys;
    const std::map<std::string, Token::Type> m_mFixedWidthSpec;

    static constexpr std::string_view s_lDigits = "0123456789";

    /**
     *  @brief
     *      Return true if `str` begins with `prefix`, delimited by one of `delim`.
     *
     *  @example
     *      startswith("integer", "int")            => true
     *      startswith("integer", "int", " ")       => false
     *      startswith("int i", "int", " ")         => true
     */
    static bool startswith(std::string_view str, std::string_view prefix, std::string_view delim = "");

    static bool startswith_str(std::string_view str, std::string_view& substr);
    static bool startswith_digits(std::string_view str, std::string_view& digits);

    /**
     *  @brief
     *      Return true if `str` starts with a valid float and set `out` to the float.
     */
    static bool startswith_float(std::string_view str, std::string_view& out);
};

#endif  // TOKENSPEC_H
