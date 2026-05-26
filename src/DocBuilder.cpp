#include "DocBuilder.h"
#include "Settings.h"
#include "ast.h"

/**
 * -- IN WORK --
 *
 * Recipes:
 *
 *  Delimiters on separate lines:
 *
 *      text
 *      nest
 *          group
 *              line
 *              <contents>
 *      line
 *      text
 *
 *  Delimiters on same line:
 *
 *      text
 *      nest
 *          group
 *              line
 *              <contents>
 *              line
 *      text
 *
 */
doc::Doc* DocBuilder::root() const
{
    return m_lDocStack.at(0);
}

void DocBuilder::visit(const ast::AssignStmt* apAssignStmt)
{
    push_group();
    {
        push_text(apAssignStmt->ex_cmd()->str());
        push_deferred_line(Settings::SpaceAfterExprCmd);
        apAssignStmt->lexpr()->accept(*this);
        push_line(Settings::OperatorPadding);
        push_text(apAssignStmt->op()->str());
        push_deferred_line(Settings::OperatorPadding);
        apAssignStmt->rexpr()->accept(*this);
    }
    pop();
}

void DocBuilder::visit(const ast::BinaryOp* apNode)
{
    size_t lnPadding = apNode->op()->type() == Token::Type::OP_DOT ? 0 : Settings::OperatorPadding;

    // FIXME: The current system does not allow for indenting all '<op> + <rhs>' lines.

    apNode->lexpr()->accept(*this);
    push_line(lnPadding);
    push_group();
    {
        push_text(apNode->op()->str());
        push_deferred_line(lnPadding);
        apNode->rexpr()->accept(*this);
    }
    pop();
}

void DocBuilder::visit(const ast::CallExpr* apNode)
{
    push_line(m_nDeferredLine);
    apNode->callable()->accept(*this);
    push_group();
    {
        push_text("(");
        push_nest();
        {
            push_deferred_line(Settings::ParenPadding);
            push_group();
            {
                apNode->args()->accept(*this);
            }
            pop();
            push_deferred_line(Settings::ParenPadding);
        }
        pop();
        push_text(")");
    }
    pop();
}

void DocBuilder::visit(const ast::CasedBinaryOp* apNode)
{
    push_group();
    {
        apNode->lexpr()->accept(*this);
        push_line(Settings::OperatorPadding);
        push_text(apNode->op()->str());
        push_text(apNode->case_sensitivity()->str());
        push_deferred_line(Settings::OperatorPadding);
        apNode->rexpr()->accept(*this);
    }
    pop();
}

void DocBuilder::visit(const ast::CommentStmt* apCommentStmt)
{
    if (apCommentStmt->trailing())
    {
        // FIXME: Trailing comments are not supported.

        throw std::runtime_error("Trailing comments are not supported");
    }

    push_text(apCommentStmt->comment()->str());
}

void DocBuilder::visit(const ast::DictEntry* apNode)
{
    push_group();
    {
        apNode->key()->accept(*this);
        push_line(Settings::SpaceAfterDictKey);
        push_text(":");
        push_deferred_line(Settings::SpaceBeforeDictValue);
        apNode->value()->accept(*this);
    }
    pop();
}

void DocBuilder::visit(const ast::DictExpr* apNode)
{
    push_line(m_nDeferredLine);
    push_text("{");
    push_nest();
    {
        push_deferred_line(Settings::CurlyBracePadding);
        push_group();
        {
            for (size_t i = 0; i < apNode->children().size(); i++)
            {
                apNode->children().at(i)->accept(*this);

                if (i < apNode->children().size() - 1)
                {
                    push_text(",");
                    push_deferred_line(Settings::SpaceAfterDictSeparator);
                }
            }
        }
        pop();
        push_line(Settings::CurlyBracePadding);
    }
    pop();
    push_line();
    push_text("}");
}

void DocBuilder::visit(const ast::EmptyStmt* apNode)
{
}

void DocBuilder::visit(const ast::ExprCmd* apNode)
{
    push_group();
    {
        push_text(apNode->ex_cmd()->str());

        for (ast::Node* pChildNode : apNode->children())
        {
            push_deferred_line(Settings::SpaceAfterExprCmd);
            pChildNode->accept(*this);
        }
    }
    pop();
}

void DocBuilder::visit(const ast::FnArgList* apNode)
{
    for (size_t i = 0; i < apNode->children().size(); i++)
    {
        apNode->children().at(i)->accept(*this);

        if (i < apNode->children().size() - 1)
        {
            push_text(",");
            push_line(Settings::SpaceAfterFnArgSeparator);
        }
    }
}

void DocBuilder::visit(const ast::FnParam* apNode)
{
    push_group();
    {
        apNode->name()->accept(*this);

        if (apNode->default_value() != nullptr)
        {
            push_line(Settings::DefaultFnParamPadding);
            push_text("=");
            push_deferred_line(Settings::DefaultFnParamPadding);
            apNode->default_value()->accept(*this);
        }
    }
    pop();
}

void DocBuilder::visit(const ast::FnParamList* apNode)
{
    for (size_t i = 0; i < apNode->children().size(); i++)
    {
        apNode->children().at(i)->accept(*this);

        if (i < apNode->children().size() - 1)
        {
            push_text(",");
            push_line(Settings::SpaceAfterFnParamSeparator);
        }
    }
}

void DocBuilder::visit(const ast::FnStmt* apFnStmt)
{
    push_group();
    {
        push_text(apFnStmt->ex_fu()->str());

        if (apFnStmt->bang() != nullptr)
        {
            push_text(apFnStmt->bang()->str());
        }

        push_line(Settings::SpaceBeforeFunctionName);
        push_text(apFnStmt->name()->str());

        push_text("(");
        push_deferred_line(Settings::ParenPadding);
        push_nest();
        {
            push_group();
            {
                apFnStmt->params()->accept(*this);
            }
            pop();
        }
        pop();
        push_line(Settings::ParenPadding);
        push_text(")");
    }
    pop();

    for (const Token* pModifier : apFnStmt->modifiers())
    {
        if (pModifier != nullptr)
        {
            push_line(Settings::ExCmdModifierPadding);
            push_text(pModifier->str());
        }
    }

    if (!apFnStmt->children().empty())
    {
        push_nest();
        {
            push_break();
            apFnStmt->body()->accept(*this);
        }
    }
    pop();
    push_break();
    push_text(apFnStmt->ex_endfu()->str());
}

void DocBuilder::visit(const ast::ForStmt* apForStmt)
{
    push_group();
    {
        push_text("for");
        push_group();
        {
            push_deferred_line(Settings::ControlStmtPadding);
            apForStmt->item()->accept(*this);
            push_line(Settings::ControlStmtPadding);
            push_text("in");
            push_deferred_line(Settings::ControlStmtPadding);
            apForStmt->items()->accept(*this);
        }
        pop();
        if (apForStmt->stmts()->size() > 0)
        {
            push_nest();
            {
                push_break();
                apForStmt->stmts()->accept(*this);
            }
            pop();
        }
        push_break();
        push_text(apForStmt->ex_endfo()->str());
    }
    pop();
}

void DocBuilder::visit(const ast::GroupExpr* apGroupExpr)
{
    push_text(' ', m_nDeferredLine);
    push_text("(");
    push_deferred_line(Settings::ParenPadding);
    push_group();
    {
        apGroupExpr->expr()->accept(*this);
        push_line(Settings::ParenPadding);
    }
    pop();
    push_text(")");
}

void DocBuilder::visit(const ast::HereDocExpr* apHereDocExpr)
{
    // FIXME: HereDocExpr makes the enclosing AssignStmt enter BREAK mode

    push_line(m_nDeferredLine);
    for (const Token* pModifier : apHereDocExpr->modifiers())
    {
        push_text(pModifier->str());
        push_line(Settings::ExCmdModifierPadding);
    }

    push_text(apHereDocExpr->endmarker()->str());
    push_break();

    push_group();
    {
        for (const ast::Node* pChildNode : apHereDocExpr->children())
        {
            push_group();
            {
                pChildNode->accept(*this);
            }
            pop();
        }
    }
    pop();
    push_text(apHereDocExpr->endmarker()->str());
}

void DocBuilder::visit(const ast::IfBranch* apIfBranch)
{
    push_group();
    {
        push_text(apIfBranch->token()->str());

        if (apIfBranch->condition() != nullptr)
        {
            push_deferred_line(Settings::ControlStmtPadding);
            apIfBranch->condition()->accept(*this);
        }
    }
    pop();
    if (apIfBranch->body()->size() > 0)
    {
        push_nest();
        {
            push_break();
            apIfBranch->body()->accept(*this);
        }
        pop();
    }
    push_break();
}

void DocBuilder::visit(const ast::IfStmt* apIfStmt)
{
    push_group();
    {
        for (const ast::Node* pChildNode : apIfStmt->children())
        {
            pChildNode->accept(*this);
        }

        push_text(apIfStmt->ex_endif()->str());
    }
    pop();
}

void DocBuilder::visit(const ast::IndexExpr* apNode)
{
    apNode->indexable()->accept(*this);
    push_text("[");
    push_deferred_line(Settings::SquareBracketPadding);
    push_nest();
    {
        push_group();
        {
            apNode->index()->accept(*this);
            push_line(Settings::SquareBracketPadding);
        }
        pop();
    }
    pop();
    push_text("]");
}

void DocBuilder::visit(const ast::InterpStr* apInterpStr)
{
    for (ast::Node* pChildNode : apInterpStr->children())
    {
        pChildNode->accept(*this);
    }
}

void DocBuilder::visit(const ast::JumpStmt* apJumpStmt)
{
    push_group();
    {
        push_text(apJumpStmt->ex_cmd()->str());

        if (apJumpStmt->expr() != nullptr)
        {
            push_deferred_line(Settings::ReturnStmtPadding);
            apJumpStmt->expr()->accept(*this);
        }
    }
    pop();
}

void DocBuilder::visit(const ast::ListAssignExpr* apListAssignExpr)
{
    push_text(' ', m_nDeferredLine);
    push_text("[");
    push_deferred_line(Settings::SquareBracketPadding);
    push_nest();
    {
        push_group();
        {
            for (size_t i = 0; i < apListAssignExpr->children().size(); i++)
            {
                apListAssignExpr->children().at(i)->accept(*this);

                if (apListAssignExpr->semi() == nullptr)
                {
                    if (i < apListAssignExpr->children().size() - 1)
                    {
                        push_text(",");
                        push_deferred_line(Settings::SpaceAfterListSeparator);
                    }
                }
                else
                {
                    if (i < apListAssignExpr->children().size() - 2)
                    {
                        push_text(",");
                        push_deferred_line(Settings::SpaceAfterListSeparator);
                    }
                    else if (i == apListAssignExpr->children().size() - 2)
                    {
                        push_text(";");
                        push_deferred_line(1);
                    }
                }
            }
        }
        pop();
    }
    pop();
    push_line(Settings::SquareBracketPadding);
    push_text("]");
}

void DocBuilder::visit(const ast::ListExpr* apNode)
{
    push_text(' ', m_nDeferredLine);
    push_text("[");
    push_deferred_line(Settings::SquareBracketPadding);
    push_nest();
    {
        push_group();
        {
            for (size_t i = 0; i < apNode->children().size(); i++)
            {
                apNode->children().at(i)->accept(*this);

                if (i < apNode->children().size() - 1)
                {
                    push_text(",");
                    push_deferred_line(Settings::SpaceAfterListSeparator);
                }
            }
        }
        pop();
    }
    pop();
    push_line(Settings::SquareBracketPadding);
    push_text("]");
}

void DocBuilder::visit(const ast::Literal* apNode)
{
    push_line(m_nDeferredLine);
    push_text(apNode->token()->str());
}

void DocBuilder::visit(const ast::LiteralStr* apNode)
{
    push_line(m_nDeferredLine);
    push_str_expr(apNode);
}

void DocBuilder::visit(const ast::LockVarStmt* apLockVarStmt)
{
    push_group();
    {
        push_text(apLockVarStmt->ex_cmd()->str());

        if (apLockVarStmt->bang() != nullptr)
        {
            push_text(apLockVarStmt->bang()->str());
        }

        if (apLockVarStmt->depth() != nullptr)
        {
            push_line(Settings::SpaceAfterExprCmd);
            push_text(apLockVarStmt->depth()->str());
        }

        push_deferred_line(Settings::SpaceAfterExprCmd);

        for (const ast::Node* pChildNode : apLockVarStmt->children())
        {
            pChildNode->accept(*this);
            push_deferred_line(Settings::SpaceAfterListSeparator);
        }
    }
    pop();
}

void DocBuilder::visit(const ast::MethodCallExpr* apMethodCallExpr)
{
    push_group();
    {
        apMethodCallExpr->receiver()->accept(*this);
        push_text(apMethodCallExpr->op()->str());
        apMethodCallExpr->call()->accept(*this);
    }
    pop();
}

void DocBuilder::visit(const ast::Pattern* apNode)
{
    push_line(m_nDeferredLine);
    push_str_expr(apNode);
}

void DocBuilder::visit(const ast::Program* apNode)
{
    push_group();
    {
        for (ast::Node* pChildNode : apNode->children())
        {
            pChildNode->accept(*this);
        }
    }

    push_break();
}

void DocBuilder::visit(const ast::SliceExpr* apNode)
{
    apNode->sliceable()->accept(*this);

    push_text("[");
    push_deferred_line(Settings::SquareBracketPadding);

    if (apNode->start() != nullptr)
    {
        push_nest();
        {
            push_group();
            {
                apNode->start()->accept(*this);
                push_line(Settings::SquareBracketPadding);
            }
            pop();
        }
        pop();
    }

    push_text(":");

    if (apNode->stop() != nullptr)
    {
        push_nest();
        {
            push_group();
            {
                apNode->stop()->accept(*this);
                push_line(Settings::SquareBracketPadding);
            }
            pop();
        }
        pop();
    }

    push_text("]");
}

void DocBuilder::visit(const ast::StmtList* apNode)
{
    for (size_t i = 0; i < apNode->children().size(); i++)
    {
        apNode->children().at(i)->accept(*this);

        if (i < apNode->children().size() - 1)
        {
            push_break();
        }
    }
}

void DocBuilder::visit(const ast::StrConst* apNode)
{
    push_line(m_nDeferredLine);
    push_str_expr(apNode);
}

void DocBuilder::visit(const ast::TernaryOp* apTernaryOp)
{
    apTernaryOp->lexpr()->accept(*this);
    push_line(Settings::OperatorPadding);

    push_group();
    {
        push_text(apTernaryOp->lop()->str());
        push_deferred_line(Settings::OperatorPadding);
        apTernaryOp->mexpr()->accept(*this);
        push_line(Settings::OperatorPadding);
    }
    pop();

    push_group();
    {
        push_text(apTernaryOp->rop()->str());
        push_deferred_line(Settings::OperatorPadding);
        apTernaryOp->rexpr()->accept(*this);
    }
    pop();
}

void DocBuilder::visit(const ast::TryBranch* apTryBranch)
{
    push_group();
    {
        push_text(apTryBranch->ex_cmd()->str());

        if (apTryBranch->pattern() != nullptr)
        {
            push_deferred_line(Settings::ControlStmtPadding);
            apTryBranch->pattern()->accept(*this);
        }
    }
    pop();
    if (apTryBranch->body()->size() > 0)
    {
        push_nest();
        {
            push_break();
            apTryBranch->body()->accept(*this);
        }
        pop();
    }
    push_break();
}

void DocBuilder::visit(const ast::TryStmt* apTryStmt)
{
    push_group();
    {
        for (const ast::Node* pChildNode : apTryStmt->children())
        {
            pChildNode->accept(*this);
        }

        push_text(apTryStmt->ex_endtry()->str());
    }
    pop();
}

void DocBuilder::visit(const ast::UnaryOp* apUnaryOp)
{
    push_group();
    {
        push_line(m_nDeferredLine);
        push_text(apUnaryOp->op()->str());
        push_deferred_line();
        apUnaryOp->rexpr()->accept(*this);
    }
    pop();
}

void DocBuilder::visit(const ast::UnletStmt* apUnletStmt)
{
    push_group();
    {
        push_text(apUnletStmt->ex_unlet()->str());

        if (apUnletStmt->bang() != nullptr)
        {
            push_text(apUnletStmt->bang()->str());
        }

        push_deferred_line(Settings::SpaceAfterExprCmd);
        apUnletStmt->expr()->accept(*this);
    }
    pop();
}

void DocBuilder::visit(const ast::Var* apNode)
{
    push_line(m_nDeferredLine);
    push_group();
    {
        if (apNode->sigil() != nullptr)
        {
            push_text(apNode->sigil()->str());
        }

        if (apNode->scope() != nullptr)
        {
            push_text(apNode->scope()->str());
        }

        if (apNode->name() != nullptr)
        {
            push_text(apNode->name()->str());
        }
    }
    pop();
}

void DocBuilder::visit(const ast::VarQueryStmt* apNode)
{
    push_group();
    {
        push_text(apNode->ex_cmd()->str());

        for (ast::Node* pChildNode : apNode->children())
        {
            push_deferred_line(Settings::SpaceAfterExprCmd);
            pChildNode->accept(*this);
        }
        pop();
    }
}

void DocBuilder::visit(const ast::WhileStmt* apWhileStmt)
{
    push_group();
    {
        push_text(apWhileStmt->ex_cmd_while()->str());
        push_deferred_line(Settings::ControlStmtPadding);
        apWhileStmt->condition()->accept(*this);
    }
    pop();
    if (apWhileStmt->stmts()->size() > 0)
    {
        push_nest();
        {
            push_break();
            apWhileStmt->stmts()->accept(*this);
        }
        pop();
    }
    push_break();
    push_text(apWhileStmt->ex_cmd_endwile()->str());
}

void DocBuilder::push_text(const std::string& asText)
{
    doc::Doc* pText = new doc::Text(asText);
    m_lDocStack.back()->push(pText);
}

void DocBuilder::push_text(char aText, size_t anCount)
{
    push_text(std::string(anCount, aText));
}

void DocBuilder::push_line(size_t anWidth)
{
    // FIXME: Can m_nDeferredLine be here instead of an argument?

    doc::Line* pLine = new doc::Line(anWidth);
    m_lDocStack.back()->push(pLine);
}

void DocBuilder::push_deferred_line(size_t anWidth)
{
    m_nDeferredLine = anWidth;
}

void DocBuilder::push_break()
{
    doc::Break* pBreak = new doc::Break();
    m_lDocStack.back()->push(pBreak);
}

void DocBuilder::push_group()
{
    m_lDocStack.push_back(new doc::Group());
}

void DocBuilder::push_nest()
{
    m_lDocStack.push_back(new doc::Nest());
}

void DocBuilder::pop()
{
    doc::Doc* pOld = m_lDocStack.back();
    m_lDocStack.pop_back();
    m_lDocStack.back()->push(pOld);
}

void DocBuilder::push_str_expr(const ast::StrExpr* apStrExpr)
{
    std::string lstr = apStrExpr->ldelim() == nullptr ? "" : apStrExpr->ldelim()->str();
    std::string rstr = apStrExpr->rdelim() == nullptr ? "" : apStrExpr->rdelim()->str();
    std::string str = apStrExpr->str() == nullptr ? "" : apStrExpr->str()->str();

    push_text(lstr + str + rstr);
}

void DocBuilder::visit_children(const ast::Node* apNode, const std::string& asDelimiter)
{
    for (size_t i = 0; i < apNode->children().size(); i++)
    {
        push_group();
        {
            if (!asDelimiter.empty())
            {
                push_text(asDelimiter);
            }

            apNode->children().at(i)->accept(*this);
        }
        pop();
    }
}
