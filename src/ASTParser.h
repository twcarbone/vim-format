#pragma once

#include <vector>

#include "Context.h"
#include "Token.h"
#include "Tokens.h"
#include "ast.h"

class ASTParser
{
public:
    ASTParser(const Context& context, Tokens&& tokens);
    ~ASTParser();

    void parse();
    ast::Program* root() const;

private:
    ast::Program* m_pRoot;
    Source m_cSource;

    // true if we are parsing an expression on the left-hand side of an assignment
    // operator, false otherwise.
    bool m_bLhs;

    Tokens m_lTokens;
    const std::unordered_map<Token::Type, std::pair<int, int> > m_mOpBindingPower;

    ast::Var* var();
    ast::Stmt* stmt();
    ast::Stmt* let_stmt();
    ast::Expr* expr(int min_binding_power = 0);
    ast::IfStmt* if_stmt();
    ast::FnStmt* fn_stmt();
    ast::Program* program();
    ast::ForStmt* for_stmt();
    ast::ExprCmd* expr_cmd();
    ast::StrExpr* str_expr();
    ast::TryStmt* try_stmt();
    ast::Pattern* pattern();
    ast::StmtList* stmt_list();
    ast::JumpStmt* jump_stmt();
    ast::ListExpr* list_expr();
    ast::DictExpr* dict_expr();
    ast::IfBranch* if_branch();
    ast::FnArgList* fn_arg_list();
    ast::TryBranch* try_branch();
    ast::InterpStr* interp_str();
    ast::WhileStmt* while_stmt();
    ast::UnletStmt* unlet_stmt();
    ast::AssignStmt* assign_stmt();
    ast::FnParamList* fn_param_list();
    ast::HereDocExpr* heredoc_expr();
    ast::CommentStmt* comment_stmt();
    ast::LockVarStmt* lockvar_stmt();
    ast::VarQueryStmt* var_query_stmt();
    ast::ListAssignExpr* list_assign_expr();

    std::vector<ast::Expr*> names();

    void try_consume(const Token::Type type, const std::string& vim_error_code);
    void consume(const Token::Type type);
    bool consume_optional(const Token::Type type);

    bool chk_let_query();

    Token* curr() const;

    ast::Expr* try_expr(const std::string& vim_error_code);
    [[noreturn]] void throw_unexpected_token();
    [[noreturn]] void throw_vim_error(std::string code);
};
