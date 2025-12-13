#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <list>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "Token.h"
#include "TokenSpec.h"

class Tokenizer
{
public:
    Tokenizer();
    Tokenizer(const std::string& text);
    ~Tokenizer();

    /**
     *  @brief
     *      Return the next Token. Returns 'END' Token at EOF.
     *
     *  @throws
     *      std::runtime_error if Token is unrecognized.
     *      std::runtime_error if Token cannot be disambiguated.
     */
    Token* next();

    /**
     *  @brief
     *      Fill buffer of with all Tokens.
     *
     *  @throws
     *      See Tokenizer::next().
     */
    void tokenize(const std::string& text = "");

    const Token& token(size_t i) const;
    std::vector<Token*>* tokens() const;

private:
    int m_nCursor;
    std::string m_sText;
    std::vector<Token*>* m_pTokens;

    TokenSpec* m_pTokenSpec;

    Token* do_next();
    void freeTokens() const;
    bool disambiguate(Token* token);
    bool hasMoreTokens() const;
};

#endif  // TOKENIZER_H
