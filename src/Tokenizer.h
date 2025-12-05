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
    Tokenizer(const std::string& text);
    ~Tokenizer();

    void tokenize();

    std::vector<Token*>* tokens() const;

private:
    int m_nCursor;
    std::string m_sText;
    std::vector<Token*>* m_pTokens;

    TokenSpec* m_pTokenSpec;

    bool hasMoreTokens() const;

    Token* next();
};

#endif  // TOKENIZER_H
