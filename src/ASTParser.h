#pragma once

#include <vector>

#include "Context.h"
#include "Token.h"
#include "ast.h"

class ASTParser
{
public:
    ASTParser(const Context& context, std::vector<Token*> tokens);
    ~ASTParser();

    void parse();
    ast::Program* root() const;

private:
    ast::Program* m_pRoot;
    Token* m_pCurrToken;
    Source m_cSource;
    size_t m_nPos;

    const std::vector<Token*> m_lTokens;
    const std::unordered_map<Token::Type, std::pair<int, int> > m_mOpBindingPower;

    ast::Stmt* stmt();
    ast::Expr* slice_expr();
    ast::IfStmt* if_stmt();
    ast::Program* program();
    ast::ForStmt* for_stmt();
    ast::ExprCmd* expr_cmd();
    ast::StmtList* stmt_list();
    ast::FnStmt* fn_stmt();
    ast::JumpStmt* jump_stmt();
    ast::ListExpr* list_expr();
    ast::FnArgList* fn_arg_list();
    ast::FnParamList* fn_param_list();
    ast::WhileStmt* while_stmt();
    ast::AssignStmt* assign_stmt();

    ast::Expr* expr(int min_binding_power);

    void next();
    void consume(const Token::Type type);
    bool consume_optional(const Token::Type type);

    [[noreturn]] void throw_unexpected_token();
    [[noreturn]] void throw_vim_error(std::string code);
};
