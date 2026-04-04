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
    Source m_cSource;
    size_t m_nPos;

    const std::vector<Token*> m_lTokens;
    const std::unordered_map<Token::Type, std::pair<int, int> > m_mOpBindingPower;

    ast::Var* var();
    ast::Stmt* stmt();
    ast::Expr* expr(int min_binding_power);
    ast::IfStmt* if_stmt();
    ast::FnStmt* fn_stmt();
    ast::Program* program();
    ast::ForStmt* for_stmt();
    ast::ExprCmd* expr_cmd();
    ast::StrExpr* str_expr();
    ast::StmtList* stmt_list();
    ast::JumpStmt* jump_stmt();
    ast::ListExpr* list_expr();
    ast::DictExpr* dict_expr();
    ast::IfBranch* if_branch(Token::Type type);
    ast::FnArgList* fn_arg_list();
    ast::InterpStr* interp_str();
    ast::WhileStmt* while_stmt();
    ast::UnletStmt* unlet_stmt();
    ast::AssignStmt* assign_stmt();
    ast::FnParamList* fn_param_list();
    ast::CommentStmt* comment_stmt();

    void consume(const Token::Type type);
    bool consume_optional(const Token::Type type);

    Token* curr() const;
    Token* prev() const;
    Token* next() const;

    [[noreturn]] void throw_unexpected_token();
    [[noreturn]] void throw_vim_error(std::string code);
};
