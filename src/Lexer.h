#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <list>
#include <regex>
#include <string_view>
#include <utility>
#include <vector>

#include "Context.h"
#include "Token.h"

class Lexer
{
public:
    // TODO (gh-107): Keywords may not require an abbreviation
    struct Keyword
    {
        std::string sFull = "";
        std::string sAbrv = "";
        Token::Type eTokenType = Token::Type::NONE;
    };

    struct Command : Keyword
    {
    };

    // Punctuators, operators, or other fixed-width symbols
    struct Symbol
    {
        std::string sLexeme = "";
        Token::Type eTokenType = Token::Type::NONE;
    };

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
    void next();

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
    enum class State
    {
        NONE,
        INTERP_STR,
        INTERP_EXP,
        STRING_CONSTANT,
        LITERAL_STRING,
    };

    State m_eState;
    Token* m_pCurrToken;
    size_t m_nBraceLevel;
    Source m_cSource;
    std::vector<Token*> m_lTokens;

    const std::vector<Command> m_lCommands;
    const std::vector<Keyword> m_lKeywords;
    const std::vector<Symbol> m_lSymbols;
    const std::vector<std::pair<std::regex, Token::Type> > m_lReSpec;

    bool match();
    void freeTokens();
    bool disambiguate(Token* token);
    void retype_keyword(Token* token);
    void state_toggle_str(State state);

    bool push_token(Token::Type type, char lexeme);
    bool push_token(Token::Type type, std::string_view lexeme);
    bool push_token(Token::Type type, const std::string& lexeme);

    bool chk_comment() const;
    bool chk_register() const;
};

#endif  // TOKENIZER_H
