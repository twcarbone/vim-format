#pragma once

#include <unordered_map>
#include <utility>
#include <vector>

#include "Context.h"
#include "Token.h"
#include "ast.h"

class PrattParser
{
public:
    PrattParser(const Context& context, std::vector<Token*> tokens);

    /**
     *  @brief
     *      Parse an expression.
     *
     *  @credit
     *      (Core Dumped) This Simple Algorithm Powers Real Interpreters: Pratt Parsing
     */
    void parse();
    ast::Expr* root() const;

    size_t pos() const;
    void seek(size_t pos);

private:
    ast::Expr* m_pRoot;
    Token* m_pCurrToken;
    Source m_cSource;
    size_t m_nPos;

    const std::vector<Token*> m_lTokens;
    const std::unordered_map<Token::Type, std::pair<int, int> > m_mOpBindingPower;

    void next();
    void consume(Token::Type type);
    ast::Expr* parse_expr(int min_binding_power);

    [[noreturn]] void throw_unexpected_token();
};
