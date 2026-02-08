#include "ASTParser.h"
#include "Exceptions.h"

ASTParser::ASTParser(const Context& acContext, std::vector<Token*> alTokens) :
    m_pRoot { nullptr },
    m_pCurrToken { nullptr },
    m_cSource { acContext.source() },
    m_nPos { 0 },
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
        { Token::Type::OP_SLICE, { 90, 91 } },
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
    next();
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

ast::StmtList* ASTParser::stmt_list()
{
    ast::StmtList* pStmtList = new ast::StmtList();

    while (true)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::ELSE:
            case Token::Type::ELSEIF:
            case Token::Type::END:
            case Token::Type::ENDFOR:
            case Token::Type::ENDIF:
            case Token::Type::ENDWHILE:
            case Token::Type::ENDFUNCTION:
                return pStmtList;
            case Token::Type::NEWLINE:
                consume(Token::Type::NEWLINE);
                break;
            default:
                pStmtList->push(stmt());
                consume(Token::Type::NEWLINE);
        }
    }
}

ast::Stmt* ASTParser::stmt()
{
    // TODO (gh-5): BREAK and CONTINUE are only allowed during an iteration
    // TODO (gh-31): RETURN only allowed in function

    ast::Stmt* pStmt;

    switch (m_pCurrToken->type())
    {
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
        case Token::Type::CMD_ECHO:
            pStmt = expr_cmd();
            break;
        case Token::Type::CMD_LET:
            pStmt = assign_stmt();
            break;
        default:
            break;
    }

    return pStmt;
}

ast::IfStmt* ASTParser::if_stmt()
{
    switch (m_pCurrToken->type())
    {
        case Token::Type::IF:
        case Token::Type::ELSEIF:
            consume(m_pCurrToken->type());
            break;
        default:
            break;
    }

    ast::Expr* pExpr = expr(0);
    ast::StmtList* pThenStmts = stmt_list();
    ast::StmtList* pElseStmts = nullptr;

    switch (m_pCurrToken->type())
    {
        case Token::Type::ELSEIF:
            pElseStmts = new ast::StmtList();
            pElseStmts->push(if_stmt());
            break;
        case Token::Type::ELSE:
            consume(Token::Type::ELSE);
            pElseStmts = stmt_list();
            consume(Token::Type::ENDIF);
            break;
        default:
            pElseStmts = new ast::StmtList();
            consume(Token::Type::ENDIF);
    }

    return new ast::IfStmt(pExpr, pThenStmts, pElseStmts);
}

ast::WhileStmt* ASTParser::while_stmt()
{
    consume(Token::Type::WHILE);
    ast::Expr* pExpr = expr(0);
    ast::StmtList* pStmts = stmt_list();
    consume(Token::Type::ENDWHILE);
    return new ast::WhileStmt(pExpr, pStmts);
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

        switch (m_pCurrToken->type())
        {
            case Token::Type::IDENTIFIER:
                pVar = new ast::Var(m_pCurrToken);
                consume(Token::Type::IDENTIFIER);

                if (consume_optional(Token::Type::ASSIGN_EQ))
                {
                    bGotDefaultParam = true;
                    pDefaultExpr = expr(0);
                }
                else if (bGotDefaultParam)
                {
                    // A non-default param follows a default param.
                    throw_vim_error("E989");
                }

                if (!consume_optional(Token::Type::COMMA))
                {
                    bDoneParsing = true;
                }

                break;
            case Token::Type::FN_ELLIPSES:
                pVar = new ast::Var(m_pCurrToken);
                consume(Token::Type::FN_ELLIPSES);
                bDoneParsing = true;
                break;
            case Token::Type::COMMA:
                // The first token is a comma, or we got two commas after a param.
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
            switch (m_pCurrToken->type())
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

ast::FnStmt* ASTParser::fn_stmt()
{
    consume(Token::Type::FUNCTION);

    // Bang (!)
    Token* pBang = nullptr;
    if (m_pCurrToken->type() == Token::Type::OP_BANG)
    {
        pBang = m_pCurrToken;
        consume(Token::Type::OP_BANG);
    }

    // Identifier
    Token* pName = m_pCurrToken;
    consume(Token::Type::IDENTIFIER);

    consume(Token::Type::L_PAREN);

    // Params
    ast::FnParamList* pFnParamList = fn_param_list();

    consume(Token::Type::R_PAREN);

    // Modifiers (range, abort, etc.)
    std::vector<Token*> lModifiers;
    while (m_pCurrToken->type() != Token::Type::NEWLINE)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::FN_RANGE:
            case Token::Type::FN_ABORT:
            case Token::Type::FN_DICT:
            case Token::Type::FN_CLOSURE:
                lModifiers.push_back(m_pCurrToken);
                consume(m_pCurrToken->type());
                break;
            default:
                throw_unexpected_token();
        }
    }

    consume(Token::Type::NEWLINE);

    // Body
    ast::StmtList* pBody = stmt_list();

    ast::FnStmt* pFnStmt = new ast::FnStmt(pName, pBang, pFnParamList, lModifiers, pBody);

    consume(Token::Type::ENDFUNCTION);
    return pFnStmt;
}

ast::ForStmt* ASTParser::for_stmt()
{
    consume(Token::Type::FOR);
    ast::Expr* pItem = expr(0);
    consume(Token::Type::IN);
    ast::Expr* pItems = expr(0);
    ast::StmtList* pStmtList = stmt_list();
    consume(Token::Type::ENDFOR);
    return new ast::ForStmt(pItem, pItems, pStmtList);
}

ast::JumpStmt* ASTParser::jump_stmt()
{
    Token* pCmd = m_pCurrToken;
    consume(m_pCurrToken->type());

    ast::Expr* pExpr = nullptr;

    if (pCmd->type() == Token::Type::RETURN && m_pCurrToken->type() != Token::Type::NEWLINE)
    {
        pExpr = expr(0);
    }

    return new ast::JumpStmt(pCmd, pExpr);
}

ast::AssignStmt* ASTParser::assign_stmt()
{
    Token* pOp = nullptr;
    ast::Var* pVar = nullptr;
    ast::Expr* pExpr = nullptr;

    consume(Token::Type::CMD_LET);

    pVar = new ast::Var(m_pCurrToken);
    consume(Token::Type::IDENTIFIER);

    switch (m_pCurrToken->type())
    {
        case Token::Type::ASSIGN_ADD:
        case Token::Type::ASSIGN_MINUS:
        case Token::Type::ASSIGN_MUL:
        case Token::Type::ASSIGN_DIV:
        case Token::Type::ASSIGN_EQ:
        case Token::Type::ASSIGN_MODULO:
        case Token::Type::ASSIGN_CAT_NEW:
        case Token::Type::ASSIGN_CAT_OLD:
            pOp = m_pCurrToken;
            consume(m_pCurrToken->type());
            pExpr = expr(0);
            break;
        default:
            throw_unexpected_token();
    }

    return new ast::AssignStmt(pOp, pVar, pExpr);
}

ast::ExprCmd* ASTParser::expr_cmd()
{
    Token* pCmd = m_pCurrToken;
    consume(m_pCurrToken->type());
    ast::Expr* pExpr = expr(0);
    return new ast::ExprCmd(pCmd, pExpr);
}

ast::ListExpr* ASTParser::list_expr()
{
    ast::ListExpr* pListExpr = new ast::ListExpr();

    consume(Token::Type::L_BRACKET);

    while (true)
    {
        if (m_pCurrToken->type() == Token::Type::R_BRACKET)
        {
            break;
        }

        pListExpr->push(expr(0));

        if (m_pCurrToken->type() != Token::Type::R_BRACKET)
        {
            consume(Token::Type::COMMA);
        }
    }

    consume(Token::Type::R_BRACKET);
    return pListExpr;
}

ast::FnArgList* ASTParser::fn_arg_list()
{
    ast::FnArgList* pFnArgList = new ast::FnArgList();

    bool bLoop = true;

    while (bLoop)
    {
        switch (m_pCurrToken->type())
        {
            case Token::Type::COMMA:
                throw_vim_error("E116");
                break;
            case Token::Type::R_PAREN:
                bLoop = false;
                break;
            default:
                try
                {
                    pFnArgList->push(expr(0));
                }
                catch (const std::runtime_error& err)
                {
                    throw_vim_error("E116");
                }

                consume_optional(Token::Type::COMMA);
        }
    }

    consume(Token::Type::R_PAREN);
    return pFnArgList;
}

ast::Expr* ASTParser::expr(int anMinBindingPower)
{
    ast::Expr* pLhs = nullptr;
    ast::Expr* pRhs = nullptr;
    Token* pOp = nullptr;

    int lnLhsOpBindingPower;
    int lnRhsOpBindingPower;

    // 1. Parse left-hand side
    switch (m_pCurrToken->type())
    {
        case Token::Type::INTEGER:
        case Token::Type::FLOAT:
        case Token::Type::STRING:
            pLhs = new ast::Literal(m_pCurrToken);
            consume(m_pCurrToken->type());
            break;
        case Token::Type::IDENTIFIER:
            pLhs = new ast::Var(m_pCurrToken);
            consume(Token::Type::IDENTIFIER);
            break;
        case Token::Type::L_PAREN:
            consume(Token::Type::L_PAREN);
            pLhs = expr(0);
            consume(Token::Type::R_PAREN);
            break;
        case Token::Type::L_BRACKET:
            pLhs = list_expr();
            break;
        case Token::Type::OP_LOGICAL_NOT:
        case Token::Type::OP_UNARY_MINUS:
        case Token::Type::OP_UNARY_PLUS:
            pOp = m_pCurrToken;
            lnRhsOpBindingPower = m_mOpBindingPower.at(pOp->type()).second;
            consume(m_pCurrToken->type());
            pRhs = expr(lnRhsOpBindingPower);
            pLhs = new ast::UnaryOp(pOp, pRhs);
            break;
        default:
            throw_unexpected_token();
    }

    while (true)
    {
        // 2. Parse operator
        switch (m_pCurrToken->type())
        {
            case Token::Type::COMMA:
            case Token::Type::END:
            case Token::Type::IN:
            case Token::Type::NEWLINE:
            case Token::Type::OP_SLICE:
            case Token::Type::OP_TERNARY_ELSE:
            case Token::Type::R_BRACKET:
            case Token::Type::R_PAREN:
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
                pOp = m_pCurrToken;
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

                if (m_pCurrToken->type() != Token::Type::OP_SLICE)
                {
                    pStart = expr(0);
                }

                consume_optional(Token::Type::OP_SLICE);

                if (m_pCurrToken->type() != Token::Type::R_BRACKET)
                {
                    pStop = expr(0);
                }

                pLhs = new ast::IndexExpr(pLhs, pStart, pStop);
                consume(Token::Type::R_BRACKET);

                break;
            }
            case Token::Type::OP_TERNARY_IF:
            {
                Token* pLeftOp = pOp;
                ast::Expr* pMhs = expr(0);
                Token* pRightOp = m_pCurrToken;
                consume(Token::Type::OP_TERNARY_ELSE);
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
                switch (m_pCurrToken->type())
                {
                    case Token::Type::OP_MATCH_CASE:
                    case Token::Type::OP_IGNORE_CASE:
                        pCaseSensitivity = m_pCurrToken;
                        consume(m_pCurrToken->type());
                        break;
                    default:
                        break;
                }

                // TODO (gh-36): Verify CasedBinaryOp LHS vs. RHS binding power
                pRhs = expr(lnLhsOpBindingPower);
                pLhs = new ast::CasedBinaryOp(pOp, pLhs, pRhs, pCaseSensitivity);
                break;
            }
            default:
                pRhs = expr(lnRhsOpBindingPower);
                pLhs = new ast::BinaryOp(pOp, pLhs, pRhs);
        }
    }
}

void ASTParser::next()
{
    m_pCurrToken = m_lTokens.at(m_nPos++);
}

void ASTParser::consume(const Token::Type aeType)
{
    if (m_pCurrToken->type() != aeType)
    {
        throw_unexpected_token();
    }

    if (m_pCurrToken->type() == Token::Type::END)
    {
        return;
    }

    while (true)
    {
        next();

        switch (m_pCurrToken->type())
        {
            case Token::Type::TAB:
            case Token::Type::SPACE:
                break;
            default:
                return;
        }
    }
}

bool ASTParser::consume_optional(const Token::Type aeType)
{
    if (m_pCurrToken->type() == aeType)
    {
        consume(aeType);
        return true;
    }

    return false;
}

void ASTParser::throw_unexpected_token()
{
    m_cSource.seek(m_pCurrToken->source_pos());
    throw std::runtime_error("Unexpected token.\n\n" + m_cSource.traceback());
}

void ASTParser::throw_vim_error(std::string asCode)
{
    m_cSource.seek(m_pCurrToken->source_pos());
    throw VimError(asCode, m_cSource.traceback());
}
