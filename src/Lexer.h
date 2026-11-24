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
    struct Keyword
    {
        std::string sFull = "";
        Token::Type eTokenType = Token::Type::NONE;
    };

    struct Command : Keyword
    {
        std::string sAbrv = "";
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
     *      Fill buffer of with all Tokens.
     *
     *  @throws
     *      std::runtime_error if Token is unrecognized.
     *      std::runtime_error if Token cannot be disambiguated.
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
        // Cursor is one past the leading ' of a literal string, but has not yet consumed.
        LITERAL_STRING_BEGIN = 11,
        // Cursor is at the trailing ' or a literal string, but has not yet consumed.
        LITERAL_STRING_END = 12,
        PATTERN = 13,
    };

    State m_eState;
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

    void extract_tokens();
    bool disambiguate(Token* token);

    void push_token(Token* token);
    void push_token(Token::Type type, char lexeme);
    void push_token(Token::Type type, std::string_view lexeme);
    void push_token(Token::Type type, const std::string& lexeme);

    void expect_token(Token::Type type, const std::string& lexeme);

    void push_literal_string();
    void push_string_constant();
    void push_interp_literal_string();
    void push_interp_string_constant();
    void push_heredoc_string();
    void push_heredoc_eval_string();
    void push_pattern();

    bool try_push_regex();
    bool try_push_symbol();
    bool try_push_command();
    bool try_push_keyword();
    bool try_push_comment();
    bool try_push_register();
    bool try_push_number();
    bool try_push_blob();
    bool try_push_continuation();
    bool try_push_heredoc_endmarker();
};

#endif  // TOKENIZER_H
