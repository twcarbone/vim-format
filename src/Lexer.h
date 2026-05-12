#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <list>
#include <regex>
#include <string_view>
#include <utility>
#include <vector>

#include "Context.h"
#include "Token.h"
#include "Tokens.h"

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

    Token token(size_t i) const;

    Tokens take_tokens();
    const Tokens& tokens() const;

    const Source& source() const;

private:
    enum class State : int
    {
        NONE = 0,
        HEREDOC_START = 1,
        HEREDOC = 2,
        HEREDOC_EVAL_START = 3,
        HEREDOC_EVAL_STR = 4,
        HEREDOC_EVAL_EXP = 5,
        INTERP_STR_SQUOTE = 6,
        INTERP_STR_DQUOTE = 7,
        INTERP_EXP_SQUOTE = 8,
        INTERP_EXP_DQUOTE = 9,
        STRING_CONSTANT = 10,
        LITERAL_STRING = 11,
        PATTERN = 12,
    };

    State m_eState;
    Token* m_pCurrToken;
    Token* m_pEndMarker;
    Token* m_pCount;
    size_t m_nBraceLevel;
    Source m_cSource;
    Tokens m_lTokens;

    const std::vector<Command> m_lCommands;
    const std::vector<Keyword> m_lKeywords;
    const std::vector<Symbol> m_lSymbols;
    const std::vector<std::pair<std::regex, Token::Type> > m_lReSpec;
    const std::array<const std::array<State, 2>, 20> m_gStateTransitions;

    void next_state();

    bool match();
    bool disambiguate(Token* token);
    void retype_keyword(Token* token);

    bool push_token(Token::Type type, char lexeme);
    bool push_token(Token::Type type, std::string_view lexeme);
    bool push_token(Token::Type type, const std::string& lexeme);
    bool push_regex();
    bool push_symbol();
    bool push_string();
    bool push_command();
    bool push_keyword();
    bool push_comment();
    bool push_register();
    bool push_number();
    bool push_blob();
    bool push_continuation();
};

#endif  // TOKENIZER_H
