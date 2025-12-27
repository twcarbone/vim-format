#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <list>
#include <regex>
#include <string_view>
#include <utility>
#include <vector>

#include "Context.h"
#include "Token.h"
#include "TokenSpec.h"

class Lexer
{
public:
    Lexer(const Context& context);
    ~Lexer();

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
     *      See Lexer::next().
     */
    void tokenize();

    const Token& token(size_t i) const;
    std::vector<Token*> tokens() const;

    const Source& source() const;

private:
    Source m_cSource;
    std::vector<Token*> m_lTokens;

    TokenSpec* m_pTokenSpec;

    Token* do_next();
    void freeTokens();
    bool disambiguate(Token* token);
};

#endif  // TOKENIZER_H
