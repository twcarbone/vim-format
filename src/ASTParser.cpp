#include "ASTParser.h"
#include "Exceptions.h"

ASTParser::ASTParser(const Context& acContext, Tokens&& alTokens) :
    m_pRoot { nullptr },
    m_cSource { acContext.source() },
    m_bLhs { false },
    m_lTokens { std::move(alTokens) },
    m_mOpBindingPower {
        // expr1
        { Token::Type::OP_FALSEY, { 10, 11 } },
        { Token::Type::OP_TERNARY_IF, { 10, 0 } },
        // expr2
        { Token::Type::OP_OR, { 20, 21 } },
        // expr3
        { Token::Type::OP_AND, { 30, 31 } },
        // expr4
        { Token::Type::OP_EQUAL, { 40, 41 } },
        { Token::Type::OP_NEQUAL, { 40, 41 } },
        { Token::Type::OP_GT, { 40, 41 } },
        { Token::Type::OP_GTE, { 40, 41 } },
        { Token::Type::OP_LT, { 40, 41 } },
        { Token::Type::OP_LTE, { 40, 41 } },
        { Token::Type::OP_MATCH, { 40, 41 } },
        { Token::Type::OP_NMATCH, { 40, 41 } },
        { Token::Type::OP_IS, { 40, 41 } },
        { Token::Type::OP_ISNOT, { 40, 41 } },
        // expr5
        { Token::Type::OP_LSHIFT, { 50, 51 } },
        { Token::Type::OP_RSHIFT, { 50, 51 } },
        // expr6
        { Token::Type::OP_ADD, { 60, 61 } },
        { Token::Type::OP_SUB, { 60, 61 } },
        { Token::Type::OP_CAT_OLD, { 60, 61 } },
        { Token::Type::OP_CAT_NEW, { 60, 61 } },
        // expr7
        { Token::Type::OP_MUL, { 70, 71 } },
        { Token::Type::OP_DIV, { 70, 71 } },
        { Token::Type::OP_MODULO, { 70, 71 } },
        // expr9
        { Token::Type::OP_LOGICAL_NOT, { 80, 81 } },
        { Token::Type::OP_UNARY_MINUS, { 80, 81 } },
        { Token::Type::OP_UNARY_PLUS, { 80, 81 } },
        // expr10
        { Token::Type::OP_METHOD, { 83, 84 } },
        { Token::Type::L_PAREN, { 85, 0 } },
        { Token::Type::L_BRACKET, { 90, 0 } },
        { Token::Type::COLON, { 90, 91 } },
        { Token::Type::OP_DOT, { 90, 91 } },
    }
{
}

ASTParser::~ASTParser()
{
    delete m_pRoot;
}

void ASTParser::parse()
{
    m_pRoot = program();
}

ast::Program* ASTParser::root() const
{
    return m_pRoot;
}

ast::Program* ASTParser::program()
{
    ast::Program* pProgram = new ast::Program(stmt_list());
    consume(Token::Type::END);
    return pProgram;
}

// 0379281600
// 3260976525
// 1394056515
// 3064648682
// 0956216668
// 1003042615
// 3722509427
// 3506342964
// 0757707588
// 0682653799
// 3466179363
// 3280454500
// 0886019744
// 2412689443
// 1776776039
// 1694040864
ast::Var* ASTParser::var()
{
    Token* pSigil = nullptr;
    ast::ScopeExpr* pScope = nullptr;
    Token* pName = nullptr;

    switch (curr()->type())
    {
        case Token::Type::SIG_ENV:
            pSigil = curr();
            consume(Token::Type::SIG_ENV);
            pName = curr();
            consume(Token::Type::IDENTIFIER);
            break;
        case Token::Type::SIG_REG:
            pSigil = curr();
            consume(Token::Type::SIG_REG);
            pName = curr();
            consume(Token::Type::REGISTER);
            break;
        case Token::Type::SIG_OPT:
            pSigil = curr();
            consume(Token::Type::SIG_OPT);

            switch (curr()->type())
            {
                case Token::Type::SCOPE_G:
                case Token::Type::SCOPE_L:
                    pScope = new ast::ScopeExpr(curr());
                    consume(curr()->type());
                    [[fallthrough]];
                case Token::Type::IDENTIFIER:
                    pName = curr();
                    consume(Token::Type::IDENTIFIER);
                    break;
                default:
                    throw_unexpected_token();
            }

            break;
        case Token::Type::SCOPE_B:
        case Token::Type::SCOPE_W:
        case Token::Type::SCOPE_T:
        case Token::Type::SCOPE_S:
        case Token::Type::SCOPE_A:
        case Token::Type::SCOPE_V:
        case Token::Type::SCOPE_G:
        case Token::Type::SCOPE_L:
            pScope = new ast::ScopeExpr(curr());
            consume(curr()->type());
            [[fallthrough]];
        case Token::Type::IDENTIFIER:
            pName = curr();
            consume(Token::Type::IDENTIFIER);
            break;
        default:
            throw_unexpected_token();
    }

    return new ast::Var(pSigil, pScope, pName);
}

ast::StmtList* ASTParser::stmt_list()
{
    ast::StmtList* pStmtList = new ast::StmtList();

    while (true)
    {
        switch (curr()->type())
        {
            case Token::Type::ELSE:
            case Token::Type::ELSEIF:
            case Token::Type::END:
            case Token::Type::ENDFOR:
            case Token::Type::ENDIF:
            case Token::Type::ENDWHILE:
            case Token::Type::ENDFUNCTION:
                return pStmtList;
            default:
                pStmtList->push(stmt());

                if (curr()->type() == Token::Type::COMMENT)
                {
                    pStmtList->push(comment_stmt());
                }

                consume(Token::Type::NEWLINE);
        }
    }
}

ast::Stmt* ASTParser::stmt()
{
    // TODO (gh-5): BREAK and CONTINUE are only allowed during an iteration
    // TODO (gh-31): RETURN only allowed in function

    ast::Stmt* pStmt;

    switch (curr()->type())
    {
        case Token::Type::COMMENT:
            pStmt = comment_stmt();
            break;
        case Token::Type::IF:
            pStmt = if_stmt();
            break;
        case Token::Type::WHILE:
            pStmt = while_stmt();
            break;
        case Token::Type::BREAK:
        case Token::Type::CONTINUE:
        case Token::Type::RETURN:
            pStmt = jump_stmt();
            break;
        case Token::Type::FUNCTION:
            pStmt = fn_stmt();
            break;
        case Token::Type::FOR:
            pStmt = for_stmt();
            break;
        case Token::Type::EX_ECHO:
            pStmt = expr_cmd();
            break;
        case Token::Type::EX_LET:
            pStmt = assign_stmt();
            break;
        case Token::Type::EX_UNLET:
            pStmt = unlet_stmt();
            break;
        case Token::Type::NEWLINE:
            pStmt = new ast::EmptyStmt();
            break;
        default:
            break;
    }

    return pStmt;
}

ast::IfBranch* ASTParser::if_branch(Token::Type aeType)
{
    Token* pToken = nullptr;
    ast::Expr* pCondition = nullptr;
    ast::StmtList* pBody = new ast::StmtList();

    pToken = curr();
    consume(aeType);

    switch (aeType)
    {
        case Token::Type::IF:
        case Token::Type::ELSEIF:
            pCondition = expr();
            break;
        case Token::Type::ELSE:
        default:
            break;
    }

    if (curr()->type() == Token::Type::COMMENT)
    {
        pBody->push(comment_stmt());
    }

    consume(Token::Type::NEWLINE);

    ast::StmtList* pBody2 = stmt_list();
    pBody->take(pBody2);
    delete pBody2;

    return new ast::IfBranch(pToken, pCondition, pBody);
}

// 4058558481
// 0010687229
// 3530542534
// 3767655736
ast::IfStmt* ASTParser::if_stmt()
{
    std::vector<ast::IfBranch*> lIfBranches;
    Token* pExEndIf = nullptr;

    lIfBranches.push_back(if_branch(Token::Type::IF));

    while (true)
    {
        switch (curr()->type())
        {
            case Token::Type::ELSEIF:
            case Token::Type::ELSE:
                lIfBranches.push_back(if_branch(curr()->type()));
                break;
            case Token::Type::ENDIF:
                pExEndIf = curr();
                consume(Token::Type::ENDIF);
                [[fallthrough]];
            default:
                goto ifbranches_end;
        }
    }

ifbranches_end:
    return new ast::IfStmt(lIfBranches, pExEndIf);
}

// 0743278179
// 3137047372
ast::WhileStmt* ASTParser::while_stmt()
{
    ast::StmtList* pBody = new ast::StmtList();

    Token* pExWhile = curr();
    consume(Token::Type::WHILE);

    ast::Expr* pExpr = expr();

    if (curr()->type() == Token::Type::COMMENT)
    {
        pBody->push(comment_stmt());
    }

    consume(Token::Type::NEWLINE);

    ast::StmtList* pBody2 = stmt_list();
    pBody->take(pBody2);
    delete pBody2;

    Token* pExEndWhile = curr();
    consume(Token::Type::ENDWHILE);

    return new ast::WhileStmt(pExWhile, pExEndWhile, pExpr, pBody);
}

// 3280446171
// 4056585016
ast::UnletStmt* ASTParser::unlet_stmt()
{
    Token* pExUnlet = curr();
    consume(Token::Type::EX_UNLET);

    Token* pBang = nullptr;
    if (consume_optional(Token::Type::OP_BANG))
    {
        pBang = m_lTokens.peek(-1, Flags::skipws);
    }

    ast::Expr* pExpr = expr();

    return new ast::UnletStmt(pExUnlet, pBang, pExpr);
}

ast::FnParamList* ASTParser::fn_param_list()
{
    ast::FnParamList* pFnParamList = new ast::FnParamList();

    bool bGotDefaultParam = false;
    bool bDoneParsing = false;

    while (true)
    {
        ast::Var* pVar = nullptr;
        ast::Expr* pDefaultExpr = nullptr;
        ast::FnParam* pFnParam = nullptr;

        switch (curr()->type())
        {
            case Token::Type::SIG_ENV:
            case Token::Type::SIG_REG:
            case Token::Type::SIG_OPT:
            case Token::Type::SCOPE_B:
            case Token::Type::SCOPE_W:
            case Token::Type::SCOPE_T:
            case Token::Type::SCOPE_G:
            case Token::Type::SCOPE_L:
            case Token::Type::SCOPE_S:
            case Token::Type::SCOPE_A:
            case Token::Type::SCOPE_V:
            case Token::Type::IDENTIFIER:
                // TODO (gh-112): FnParamList allows scoped variable parameters
                pVar = var();

                if (consume_optional(Token::Type::ASSIGN_EQ))
                {
                    bGotDefaultParam = true;
                    pDefaultExpr = expr();
                }
                else if (bGotDefaultParam)
                {
                    // test/error/E989_1.out
                    throw_vim_error("E989");
                }

                if (!consume_optional(Token::Type::COMMA))
                {
                    bDoneParsing = true;
                }

                break;
            case Token::Type::FN_ELLIPSES:
                pVar = new ast::Var(nullptr, nullptr, curr());
                consume(Token::Type::FN_ELLIPSES);
                bDoneParsing = true;
                break;
            case Token::Type::COMMA:
                // test/error/E125_1.out
                throw_vim_error("E125");
                break;
            default:
                bDoneParsing = true;
        }

        if (pVar != nullptr)
        {
            pFnParam = new ast::FnParam(pVar, pDefaultExpr);
            pFnParamList->push(pFnParam);
        }

        if (bDoneParsing)
        {
            switch (curr()->type())
            {
                case Token::Type::R_PAREN:
                case Token::Type::OP_METHOD:
                    return pFnParamList;
                    break;
                default:
                    // We should be at the end, but aren't.
                    throw_vim_error("E475");
            }
        }
    }
}

// 2041554108
ast::FnStmt* ASTParser::fn_stmt()
{
    Token* pExFu = curr();
    consume(Token::Type::FUNCTION);

    // Bang (!)
    Token* pBang = nullptr;
    if (curr()->type() == Token::Type::OP_BANG)
    {
        pBang = curr();
        consume(Token::Type::OP_BANG);
    }

    // Identifier
    Token* pName = curr();
    consume(Token::Type::IDENTIFIER);

    consume(Token::Type::L_PAREN);

    // Params
    ast::FnParamList* pFnParamList = fn_param_list();

    consume(Token::Type::R_PAREN);

    // Modifiers (range, abort, etc.)
    std::vector<Token*> lModifiers;
    while (true)
    {
        switch (curr()->type())
        {
            case Token::Type::FN_RANGE:
            case Token::Type::FN_ABORT:
            case Token::Type::FN_DICT:
            case Token::Type::FN_CLOSURE:
                lModifiers.push_back(curr());
                consume(curr()->type());
                break;
            default:
                goto modifiers_end;
        }
    }

modifiers_end:
    ast::StmtList* pBody = new ast::StmtList();

    if (curr()->type() == Token::Type::COMMENT)
    {
        pBody->push(comment_stmt());
    }

    consume(Token::Type::NEWLINE);

    // Body
    ast::StmtList* pBody2 = stmt_list();
    pBody->take(pBody2);
    delete pBody2;

    Token* pExEndFu = curr();
    consume(Token::Type::ENDFUNCTION);

    // TODO (gh-105): endfunction does not support [argument]

    return new ast::FnStmt(pExFu, pExEndFu, pName, pBang, pFnParamList, lModifiers, pBody);
}

// 2662856482
// 2432440349
// 3291519474
ast::ForStmt* ASTParser::for_stmt()
{
    ast::Expr* pItem = nullptr;
    ast::Expr* pItems = nullptr;
    ast::StmtList* pBody = new ast::StmtList();

    consume(Token::Type::FOR);
    pItem = expr();

    consume(Token::Type::IN);
    pItems = expr();

    if (curr()->type() == Token::Type::COMMENT)
    {
        pBody->push(comment_stmt());
    }

    consume(Token::Type::NEWLINE);

    ast::StmtList* pBody2 = stmt_list();
    pBody->take(pBody2);
    delete pBody2;

    Token* pExEndFo = curr();
    consume(Token::Type::ENDFOR);

    return new ast::ForStmt(pItem, pItems, pBody, pExEndFo);
}

// 1883144826
// 2096733821
// 0178149365
ast::JumpStmt* ASTParser::jump_stmt()
{
    Token* pCmd = curr();
    consume(curr()->type());

    ast::Expr* pExpr = nullptr;

    if (pCmd->type() == Token::Type::RETURN && curr()->type() != Token::Type::NEWLINE)
    {
        pExpr = expr();
    }

    return new ast::JumpStmt(pCmd, pExpr);
}

// 1813411950
// 1974695300
// 2024605123
// 2591104698
// 2086196734
// 1803175223
// 2476392128
// 3269168211
// 0127630265
// 4211754421
// 1350391715
// 0379804504
// 4035612700
// 4254392411
// 2818147631
ast::AssignStmt* ASTParser::assign_stmt()
{
    Token* pOp = nullptr;
    ast::Expr* pLhs = nullptr;
    ast::Expr* pRhs = nullptr;

    consume(Token::Type::EX_LET);

    m_bLhs = true;
    pLhs = expr();
    m_bLhs = false;

    switch (curr()->type())
    {
        case Token::Type::ASSIGN_ADD:
        case Token::Type::ASSIGN_MINUS:
        case Token::Type::ASSIGN_MUL:
        case Token::Type::ASSIGN_DIV:
        case Token::Type::ASSIGN_EQ:
        case Token::Type::ASSIGN_MODULO:
        case Token::Type::ASSIGN_CAT_NEW:
        case Token::Type::ASSIGN_CAT_OLD:
            pOp = curr();
            consume(curr()->type());
            pRhs = expr();
            break;
        case Token::Type::ASSIGN_HEREDOC:
            pOp = curr();
            consume(curr()->type());
            pRhs = heredoc_expr();
            break;
        default:
            throw_unexpected_token();
    }

    return new ast::AssignStmt(pOp, pLhs, pRhs);
}

ast::HereDocExpr* ASTParser::heredoc_expr()
{
    std::vector<Token*> lModifiers;
    Token* pEndMarker = nullptr;

    while (true)
    {
        switch (curr()->type())
        {
            case Token::Type::HD_TRIM:
            case Token::Type::HD_EVAL:
                lModifiers.push_back(curr());
                consume(curr()->type());
                break;
            default:
                pEndMarker = curr();
                consume(Token::Type::ENDMARKER);
                consume(Token::Type::NEWLINE);
                goto args_end;
        }
    }

args_end:

    std::vector<ast::Expr*> llLines;
    ast::LiteralStr* pLiteralStr = nullptr;
    ast::InterpStr* pInterpStr = new ast::InterpStr();
    Token* pLDelim = nullptr;
    Token* pRDelim = nullptr;
    Token* pStr = nullptr;

    while (true)
    {
        switch (curr()->type())
        {
            case Token::Type::L_BRACE:
                pRDelim = curr();
                consume(Token::Type::L_BRACE);

                pInterpStr->push(new ast::LiteralStr(pStr, pLDelim, pRDelim));

                pLDelim = pRDelim = pStr = nullptr;

                pInterpStr->push(expr());

                break;
            case Token::Type::NEWLINE:
                pRDelim = curr();
                consume(Token::Type::NEWLINE);

                pInterpStr->push(new ast::LiteralStr(pStr, pLDelim, pRDelim));

                pLDelim = pRDelim = pStr = nullptr;

                llLines.push_back(pInterpStr);
                pInterpStr = new ast::InterpStr();

                break;
            case Token::Type::R_BRACE:
                pLDelim = curr();
                consume(Token::Type::R_BRACE);
                break;
            case Token::Type::STRING:
                pStr = curr();
                consume(Token::Type::STRING);
                break;
            case Token::Type::ENDMARKER:
                consume(Token::Type::ENDMARKER);
                goto lines_end;
            default:
                break;
        }
    }

lines_end:

    return new ast::HereDocExpr(std::move(llLines), std::move(lModifiers), pEndMarker);
}

ast::CommentStmt* ASTParser::comment_stmt()
{
    ast::CommentStmt* pCommentStmt = nullptr;

    if (m_lTokens.pos() == 0)
    {
        pCommentStmt = new ast::CommentStmt(curr());
    }
    else
    {
        Token* pPrevToken = m_lTokens.peek(-1, Flags::skipws);
        switch (pPrevToken->type())
        {
            case Token::Type::NEWLINE:
                pCommentStmt = new ast::CommentStmt(curr());
                break;
            default:
                pCommentStmt = new ast::CommentStmt(curr(), true);
        }
    }

    consume(Token::Type::COMMENT);

    return pCommentStmt;
}

// 2742485518
// 3191311960
// 1478378780
// 1426077019
ast::ListAssignExpr* ASTParser::list_assign_expr()
{
    std::vector<ast::Expr*> llExprs;
    Token* pSemicolon = nullptr;

    consume(Token::Type::L_BRACKET);

    if (curr()->type() == Token::Type::R_BRACKET)
    {
        // test/error/E475_1.out
        throw_vim_error("E475");
    }

    while (true)
    {
        if (curr()->type() == Token::Type::R_BRACKET)
        {
            break;
        }

        // test/error/E475_2.out
        // test/error/E475_3.out
        llExprs.push_back(try_expr("E475"));

        if (consume_optional(Token::Type::SEMICOLON))
        {
            pSemicolon = m_lTokens.peek(-1, Flags::skipws);
            llExprs.push_back(try_expr("E475"));
            break;
        }
        else if (curr()->type() != Token::Type::R_BRACKET)
        {
            try_consume(Token::Type::COMMA, "E475");
        }
    }

    try_consume(Token::Type::R_BRACKET, "E475");

    return new ast::ListAssignExpr(std::move(llExprs), pSemicolon);
}

// 1417249700
ast::ExprCmd* ASTParser::expr_cmd()
{
    Token* pCmd = curr();
    consume(curr()->type());
    ast::Expr* pExpr = expr();
    return new ast::ExprCmd(pCmd, pExpr);
}

ast::ListExpr* ASTParser::list_expr()
{
    std::vector<ast::Expr*> llExprs;

    consume(Token::Type::L_BRACKET);

    while (true)
    {
        if (curr()->type() == Token::Type::R_BRACKET)
        {
            break;
        }

        llExprs.push_back(try_expr("E15"));

        if (curr()->type() != Token::Type::R_BRACKET)
        {
            consume(Token::Type::COMMA);
        }
    }

    consume(Token::Type::R_BRACKET);

    // Tip 1-1: Use std::move to cast to an r-value, forcing a move-only transfer and
    // preventing accidental expensive copies of the container.
    return new ast::ListExpr(std::move(llExprs));
}

ast::DictExpr* ASTParser::dict_expr()
{
    std::vector<ast::DictEntry*> llEntries;

    consume(Token::Type::L_BRACE);

    while (true)
    {
        if (curr()->type() == Token::Type::R_BRACE)
        {
            break;
        }

        ast::Expr* pKey = expr();
        consume(Token::Type::COLON);
        ast::Expr* pValue = expr();

        llEntries.push_back(new ast::DictEntry(pKey, pValue));

        if (curr()->type() != Token::Type::R_BRACE)
        {
            consume(Token::Type::COMMA);
        }
    }

    consume(Token::Type::R_BRACE);
    return new ast::DictExpr(std::move(llEntries));
}

ast::FnArgList* ASTParser::fn_arg_list()
{
    ast::FnArgList* pFnArgList = new ast::FnArgList();

    while (true)
    {
        switch (curr()->type())
        {
            case Token::Type::COMMA:
                // test/error/E116_1.out
                // test/error/E116_2.out
                throw_vim_error("E116");
                break;
            case Token::Type::R_PAREN:
                goto args_end;
            default:
                pFnArgList->push(try_expr("E116"));
                consume_optional(Token::Type::COMMA);
        }
    }

args_end:
    consume(Token::Type::R_PAREN);
    return pFnArgList;
}

ast::StrExpr* ASTParser::str_expr()
{
    Token* pLDelim = nullptr;
    Token* pRDelim = nullptr;
    Token* pStr = nullptr;

    switch (curr()->type())
    {
        case Token::Type::SQUOTE:
        case Token::Type::DQUOTE:
            pLDelim = curr();
            consume(curr()->type());
            break;
        default:
            break;
    }

    if (consume_optional(Token::Type::STRING))
    {
        pStr = m_lTokens.peek(-1, Flags::skipws);
    }

    pRDelim = curr();
    consume(pLDelim->type());

    if (pLDelim->type() == Token::Type::SQUOTE)
    {
        return new ast::LiteralStr(pStr, pLDelim, pRDelim);
    }

    return new ast::StrConst(pStr, pLDelim, pRDelim);
}

ast::InterpStr* ASTParser::interp_str()
{
    ast::InterpStr* pInterpStr = new ast::InterpStr();
    ast::StrExpr* pStrExpr = nullptr;
    Token* pLDelim = nullptr;
    Token* pRDelim = nullptr;
    Token* pStr = nullptr;

    Token::Type eStrEndQuoteType;
    switch (curr()->type())
    {
        case Token::Type::STR_INTERP_SQUOTE:
            eStrEndQuoteType = Token::Type::SQUOTE;
            break;
        case Token::Type::STR_INTERP_DQUOTE:
            eStrEndQuoteType = Token::Type::DQUOTE;
            break;
        default:
            break;
    }

    pLDelim = curr();
    consume(curr()->type());

    while (true)
    {
        switch (curr()->type())
        {
            case Token::Type::SQUOTE:
            case Token::Type::DQUOTE:
                pRDelim = curr();
                consume(eStrEndQuoteType);

                if (eStrEndQuoteType == Token::Type::SQUOTE)
                {
                    pStrExpr = new ast::LiteralStr(pStr, pLDelim, pRDelim);
                }
                else
                {
                    pStrExpr = new ast::StrConst(pStr, pLDelim, pRDelim);
                }

                pInterpStr->push(pStrExpr);
                pStr = nullptr;

                goto string_end;
            case Token::Type::L_BRACE:
                pRDelim = curr();
                consume(Token::Type::L_BRACE);

                if (eStrEndQuoteType == Token::Type::SQUOTE)
                {
                    pStrExpr = new ast::LiteralStr(pStr, pLDelim, pRDelim);
                }
                else
                {
                    pStrExpr = new ast::StrConst(pStr, pLDelim, pRDelim);
                }

                pInterpStr->push(pStrExpr);
                pStr = nullptr;
                pInterpStr->push(expr());
                break;
            case Token::Type::R_BRACE:
                pLDelim = curr();
                consume(Token::Type::R_BRACE);
                break;
            default:
                pStr = curr();
                consume(Token::Type::STRING);
                break;
        }
    }

string_end:
    return pInterpStr;
}

ast::Expr* ASTParser::expr(int anMinBindingPower)
{
    ast::Expr* pLhs = nullptr;
    ast::Expr* pRhs = nullptr;
    Token* pOp = nullptr;

    int lnLhsOpBindingPower;
    int lnRhsOpBindingPower;

    // 1. Parse left-hand side
    switch (curr()->type())
    {
        case Token::Type::INTEGER:
        case Token::Type::FLOAT:
        case Token::Type::BLOB:
            pLhs = new ast::Literal(curr());
            consume(curr()->type());
            break;
        case Token::Type::SQUOTE:
        case Token::Type::DQUOTE:
            pLhs = str_expr();
            break;
        case Token::Type::STR_INTERP_SQUOTE:
        case Token::Type::STR_INTERP_DQUOTE:
            pLhs = interp_str();
            break;
        case Token::Type::SIG_ENV:
        case Token::Type::SIG_REG:
        case Token::Type::SIG_OPT:
        case Token::Type::SCOPE_B:
        case Token::Type::SCOPE_W:
        case Token::Type::SCOPE_T:
        case Token::Type::SCOPE_G:
        case Token::Type::SCOPE_L:
        case Token::Type::SCOPE_S:
        case Token::Type::SCOPE_A:
        case Token::Type::SCOPE_V:
        case Token::Type::IDENTIFIER:
            pLhs = var();
            break;
        case Token::Type::L_PAREN:
            consume(Token::Type::L_PAREN);
            pLhs = new ast::GroupExpr(expr());
            consume(Token::Type::R_PAREN);
            break;
        case Token::Type::L_BRACKET:
            if (m_bLhs)
            {
                pLhs = list_assign_expr();
            }
            else
            {
                pLhs = list_expr();
            }

            break;
        case Token::Type::L_BRACE:
            pLhs = dict_expr();
            break;
        case Token::Type::OP_LOGICAL_NOT:
        case Token::Type::OP_UNARY_MINUS:
        case Token::Type::OP_UNARY_PLUS:
            pOp = curr();
            lnRhsOpBindingPower = m_mOpBindingPower.at(pOp->type()).second;
            consume(curr()->type());
            pRhs = expr(lnRhsOpBindingPower);
            pLhs = new ast::UnaryOp(pOp, pRhs);
            break;
        default:
            throw_unexpected_token();
    }

    while (true)
    {
        // 2. Parse operator
        switch (curr()->type())
        {
            case Token::Type::COMMENT:
            case Token::Type::COMMA:
            case Token::Type::COLON:
            case Token::Type::END:
            case Token::Type::IN:
            case Token::Type::NEWLINE:
            case Token::Type::R_BRACKET:
            case Token::Type::R_BRACE:
            case Token::Type::R_PAREN:
            case Token::Type::ASSIGN_ADD:
            case Token::Type::ASSIGN_MINUS:
            case Token::Type::ASSIGN_MUL:
            case Token::Type::ASSIGN_DIV:
            case Token::Type::ASSIGN_EQ:
            case Token::Type::ASSIGN_MODULO:
            case Token::Type::ASSIGN_CAT_NEW:
            case Token::Type::ASSIGN_CAT_OLD:
            case Token::Type::ASSIGN_HEREDOC:
            case Token::Type::SEMICOLON:
                return pLhs;
            // expr1
            case Token::Type::OP_FALSEY:
            case Token::Type::OP_TERNARY_IF:
            // expr2
            case Token::Type::OP_OR:
            // expr3
            case Token::Type::OP_AND:
            // expr4
            case Token::Type::OP_EQUAL:
            case Token::Type::OP_NEQUAL:
            case Token::Type::OP_GT:
            case Token::Type::OP_GTE:
            case Token::Type::OP_LT:
            case Token::Type::OP_LTE:
            case Token::Type::OP_MATCH:
            case Token::Type::OP_NMATCH:
            case Token::Type::OP_IS:
            case Token::Type::OP_ISNOT:
            // expr5
            case Token::Type::OP_LSHIFT:
            case Token::Type::OP_RSHIFT:
            // expr6
            case Token::Type::OP_ADD:
            case Token::Type::OP_SUB:
            case Token::Type::OP_DOT:
            case Token::Type::OP_CAT_OLD:
            case Token::Type::OP_CAT_NEW:
            // expr7
            case Token::Type::OP_MUL:
            case Token::Type::OP_DIV:
            case Token::Type::OP_MODULO:
            // expr8
            // expr9
            case Token::Type::OP_LOGICAL_NOT:
            case Token::Type::OP_UNARY_MINUS:
            case Token::Type::OP_UNARY_PLUS:
            // expr10
            case Token::Type::OP_METHOD:
            case Token::Type::L_BRACKET:
            // expr11
            case Token::Type::L_PAREN:
                pOp = curr();
                break;
            default:
                throw_unexpected_token();
        }

        // 3. Parse right-hand side
        lnLhsOpBindingPower = m_mOpBindingPower.at(pOp->type()).first;
        lnRhsOpBindingPower = m_mOpBindingPower.at(pOp->type()).second;

        if (lnLhsOpBindingPower < anMinBindingPower)
        {
            return pLhs;
        }

        consume(pOp->type());

        switch (pOp->type())
        {
            case Token::Type::OP_METHOD:
                pRhs = expr(lnRhsOpBindingPower);
                pLhs = new ast::MethodCallExpr(pOp, pLhs, pRhs);
                break;
            case Token::Type::L_PAREN:
                pLhs = new ast::CallExpr(pLhs, fn_arg_list());
                break;
            case Token::Type::L_BRACKET:
            {
                ast::Expr* pStart = nullptr;
                ast::Expr* pStop = nullptr;

                if (curr()->type() != Token::Type::COLON)
                {
                    pStart = expr();

                    if (consume_optional(Token::Type::R_BRACKET))
                    {
                        pLhs = new ast::IndexExpr(pLhs, pStart);
                        break;
                    }
                }

                consume_optional(Token::Type::COLON);

                if (curr()->type() != Token::Type::R_BRACKET)
                {
                    pStop = expr();
                }

                pLhs = new ast::SliceExpr(pLhs, pStart, pStop);
                consume(Token::Type::R_BRACKET);

                break;
            }
            case Token::Type::OP_TERNARY_IF:
            {
                Token* pLeftOp = pOp;
                ast::Expr* pMhs = expr();
                Token* pRightOp = curr();
                consume(Token::Type::COLON);
                pRhs = expr(lnRhsOpBindingPower);
                pLhs = new ast::TernaryOp(pLeftOp, pRightOp, pLhs, pMhs, pRhs);

                break;
            }
            case Token::Type::OP_EQUAL:
            case Token::Type::OP_NEQUAL:
            case Token::Type::OP_GT:
            case Token::Type::OP_GTE:
            case Token::Type::OP_LT:
            case Token::Type::OP_LTE:
            case Token::Type::OP_MATCH:
            case Token::Type::OP_NMATCH:
            case Token::Type::OP_IS:
            case Token::Type::OP_ISNOT:
            {
                Token* pCaseSensitivity = nullptr;
                switch (curr()->type())
                {
                    case Token::Type::OP_MATCH_CASE:
                    case Token::Type::OP_IGNORE_CASE:
                        pCaseSensitivity = curr();
                        consume(curr()->type());
                        break;
                    default:
                        break;
                }

                pRhs = expr(lnRhsOpBindingPower);

                if (pCaseSensitivity == nullptr)
                {
                    pLhs = new ast::BinaryOp(pOp, pLhs, pRhs);
                }
                else
                {
                    // TODO (gh-36): Verify CasedBinaryOp LHS vs. RHS binding power
                    pLhs = new ast::CasedBinaryOp(pOp, pLhs, pRhs, pCaseSensitivity);
                }

                break;
            }
            default:
                pRhs = expr(lnRhsOpBindingPower);
                pLhs = new ast::BinaryOp(pOp, pLhs, pRhs);
        }
    }
}

void ASTParser::try_consume(const Token::Type aeType, const std::string& asVimErrorcode)
{
    try
    {
        consume(aeType);
    }
    catch (const std::runtime_error& err)
    {
        throw_vim_error(asVimErrorcode);
    }
}

void ASTParser::consume(const Token::Type aeType)
{
    if (curr()->type() != aeType)
    {
        throw_unexpected_token();
    }

    if (curr()->type() == Token::Type::END)
    {
        return;
    }

    m_lTokens.advance(1, Flags::skipws);
}

bool ASTParser::consume_optional(const Token::Type aeType)
{
    if (curr()->type() == aeType)
    {
        consume(aeType);
        return true;
    }

    return false;
}

Token* ASTParser::curr() const
{
    return m_lTokens.head();
}

void ASTParser::throw_unexpected_token()
{
    m_cSource.seek(curr()->source_pos());
    throw std::runtime_error("Unexpected token.\n\n" + m_cSource.context());
}

void ASTParser::throw_vim_error(std::string asCode)
{
    m_cSource.seek(curr()->source_pos());
    throw VimError(asCode, m_cSource.context());
}

ast::Expr* ASTParser::try_expr(const std::string& asVimErrorCode)
{
    try
    {
        return expr();
    }
    catch (const std::runtime_error& err)
    {
        throw_vim_error(asVimErrorCode);
    }
}
