#pragma once

#include <vector>

#include "Context.h"
#include "Token.h"
#include "ast.h"

class HybridParser
{
public:
    HybridParser(const Context& context, std::vector<Token*> tokens);
    ~HybridParser();

    void parse();
    ast::Program* root() const;

private:
    ast::Program* m_pRoot;
    Token* m_pCurrToken;
    Source m_cSource;
    size_t m_nPos;

    const std::vector<Token*> m_lTokens;
    const std::unordered_map<Token::Type, std::pair<int, int> > m_mOpBindingPower;

    ast::Program* program();
    ast::StmtList* stmt_list();
    ast::Stmt* stmt();
    ast::IfStmt* if_stmt();
    ast::WhileStmt* while_stmt();
    ast::ExprCmd* expr_cmd();
    ast::Expr* expr(int min_binding_power);

    void next();
    void consume(const Token::Type type);

    [[noreturn]] void throw_unexpected_token();
};
