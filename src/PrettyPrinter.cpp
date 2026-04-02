#include <iostream>

#include "PrettyPrinter.h"
#include "Settings.h"
#include "ast.h"

Indent::Indent(size_t anTabStop) :
    m_nTabStop { anTabStop }
{
}

Indent Indent::operator++(int)
{
    Indent temp = *this;
    this->m_nLevel++;
    return temp;
}

Indent Indent::operator--(int)
{
    Indent temp = *this;
    this->m_nLevel--;
    return temp;
}

std::ostream& operator<<(std::ostream& os, const Indent& acIndent)
{
    return os << std::string(acIndent.m_nLevel * acIndent.m_nTabStop, ' ');
}

PrettyPrinter::PrettyPrinter(std::ostream& acOutStream) :
    ASTVisitor(acOutStream),
    m_bNewlinePending { false },
    m_cIndent { Settings::IndentWidth }
{
}

void PrettyPrinter::visit(const ast::AssignStmt* apAssignStmt)
{
    write_bol();

    write("let");
    write(' ', Settings::SpaceAfterExprCmd);
    apAssignStmt->lexpr()->accept(*this);
    write(' ', Settings::OperatorPadding);
    write(apAssignStmt->op()->str());
    write(' ', Settings::OperatorPadding);
    apAssignStmt->rexpr()->accept(*this);

    write_eol();
}

void PrettyPrinter::visit(const ast::CommentStmt* apCommentStmt)
{
    if (!apCommentStmt->trailing())
    {
        write_bol();
    }
    else
    {
        write(' ', Settings::SpaceBeforeTrailingComment);
    }

    write(apCommentStmt->comment()->str());

    write_eol();
}

void PrettyPrinter::visit(const ast::BinaryOp* apBinaryOp)
{
    apBinaryOp->lexpr()->accept(*this);
    write(' ', Settings::OperatorPadding);
    write(apBinaryOp->op()->str());
    write(' ', Settings::OperatorPadding);
    apBinaryOp->rexpr()->accept(*this);
}

void PrettyPrinter::visit(const ast::CallExpr* apCallExpr)
{
    apCallExpr->callable()->accept(*this);

    write('(');
    write(' ', Settings::ParenPadding);
    apCallExpr->args()->accept(*this);
    write(' ', Settings::ParenPadding);
    write(')');
}

void PrettyPrinter::visit(const ast::CasedBinaryOp* apCasedBinaryOp)
{
    apCasedBinaryOp->lexpr()->accept(*this);
    write(' ', Settings::OperatorPadding);
    write(apCasedBinaryOp->op()->str());
    write(apCasedBinaryOp->case_sensitivity()->str());
    write(' ', Settings::OperatorPadding);
    apCasedBinaryOp->rexpr()->accept(*this);
}

void PrettyPrinter::visit(const ast::DictEntry* apDictEntry)
{
    apDictEntry->key()->accept(*this);
    write(' ', Settings::SpaceAfterDictKey);
    write(':');
    write(' ', Settings::SpaceBeforeDictValue);
    apDictEntry->value()->accept(*this);
}

void PrettyPrinter::visit(const ast::DictExpr* apDictExpr)
{
    const std::vector<const ast::DictEntry*>& lDictEntries = apDictExpr->entries();

    write('{');
    write(' ', Settings::CurlyBracePadding);

    for (size_t i = 0; i < lDictEntries.size(); i++)
    {
        lDictEntries.at(i)->accept(*this);

        if (i != lDictEntries.size() - 1)
        {
            write(',');
            write(' ', Settings::SpaceAfterDictSeparator);
        }
    }

    write(' ', Settings::CurlyBracePadding);
    write('}');
}

void PrettyPrinter::visit(const ast::EmptyStmt* apEmptyStmt)
{
    write('\n');
}

void PrettyPrinter::visit(const ast::ExprCmd* apExprCmd)
{
    write_bol();

    write(apExprCmd->ex_cmd()->str());
    write(' ', Settings::SpaceAfterExprCmd);
    apExprCmd->expr()->accept(*this);

    write_eol();
}

void PrettyPrinter::visit(const ast::FnArgList* apFnArgList)
{
    for (auto it = apFnArgList->children().cbegin(); it != apFnArgList->children().cend(); it++)
    {
        (*it)->accept(*this);

        if (std::next(it) != apFnArgList->children().cend())
        {
            write(',');
            write(' ', Settings::SpaceAfterFnArgSeparator);
        }
    }
}

void PrettyPrinter::visit(const ast::FnParam* apFnParam)
{
    apFnParam->name()->accept(*this);

    if (apFnParam->default_value() != nullptr)
    {
        write(' ', Settings::DefaultFnParamPadding);
        write('=');
        write(' ', Settings::DefaultFnParamPadding);
        apFnParam->default_value()->accept(*this);
    }
}

void PrettyPrinter::visit(const ast::FnParamList* apFnParamList)
{
    for (auto it = apFnParamList->children().cbegin(); it != apFnParamList->children().cend(); it++)
    {
        (*it)->accept(*this);

        if (std::next(it) != apFnParamList->children().cend())
        {
            write(',');
            write(' ', Settings::SpaceAfterFnParamSeparator);
        }
    }
}

void PrettyPrinter::visit(const ast::FnStmt* apFnStmt)
{
    write_bol();

    write(apFnStmt->ex_fu()->str());

    if (apFnStmt->bang() != nullptr)
    {
        write(apFnStmt->bang()->str());
    }

    write(' ', Settings::SpaceBeforeFunctionName);
    write(apFnStmt->name()->str());

    write('(');
    write(' ', Settings::ParenPadding);
    apFnStmt->params()->accept(*this);
    write(' ', Settings::ParenPadding);
    write(')');

    for (const Token* pModifier : apFnStmt->modifiers())
    {
        if (pModifier != nullptr)
        {
            write(' ', Settings::FnModifierPadding);
            write(pModifier->str());
        }
    }

    write_eol();

    m_cIndent++;
    apFnStmt->body()->accept(*this);
    m_cIndent--;

    write_bol();
    write(apFnStmt->ex_endfu()->str());

    write_eol();
}

void PrettyPrinter::visit(const ast::ForStmt* apForStmt)
{
    write_bol();

    write("for");
    write(' ', Settings::ControlStmtPadding);
    apForStmt->item()->accept(*this);

    write(' ', Settings::ControlStmtPadding);
    write("in");
    write(' ', Settings::ControlStmtPadding);
    apForStmt->items()->accept(*this);
    write_eol();

    m_cIndent++;
    apForStmt->stmts()->accept(*this);
    m_cIndent--;

    write_bol();
    write("endfor");

    write_eol();
}

void PrettyPrinter::visit(const ast::GroupExpr* apGroupExpr)
{
    write('(');
    write(' ', Settings::ParenPadding);
    apGroupExpr->expr()->accept(*this);
    write(' ', Settings::ParenPadding);
    write(')');
}

void PrettyPrinter::visit(const ast::IfBranch* apIfBranch)
{
    write_bol();

    write(apIfBranch->token()->str());

    if (apIfBranch->condition() != nullptr)
    {
        write(' ', Settings::ControlStmtPadding);
        apIfBranch->condition()->accept(*this);
    }

    write_eol();

    m_cIndent++;
    apIfBranch->body()->accept(*this);
    m_cIndent--;
}

void PrettyPrinter::visit(const ast::IfStmt* apIfStmt)
{
    for (const ast::IfBranch* pIfBranch : apIfStmt->branches())
    {
        pIfBranch->accept(*this);
    }

    write_bol();
    write(apIfStmt->ex_endif()->str());

    write_eol();
}

void PrettyPrinter::visit(const ast::IndexExpr* apIndexExpr)
{
    apIndexExpr->indexable()->accept(*this);

    write('[');
    write(' ', Settings::SquareBracketPadding);

    apIndexExpr->index()->accept(*this);

    write(' ', Settings::SquareBracketPadding);
    write(']');
}

void PrettyPrinter::visit(const ast::SliceExpr* apSliceExpr)
{
    apSliceExpr->sliceable()->accept(*this);

    write('[');
    write(' ', Settings::SquareBracketPadding);

    if (apSliceExpr->start() != nullptr)
    {
        apSliceExpr->start()->accept(*this);
    }

    write(':');

    if (apSliceExpr->stop() != nullptr)
    {
        apSliceExpr->stop()->accept(*this);
    }

    write(' ', Settings::SquareBracketPadding);
    write(']');
}

void PrettyPrinter::visit(const ast::JumpStmt* apJumpStmt)
{
    write_bol();

    write(apJumpStmt->ex_cmd()->str());

    if (apJumpStmt->expr() != nullptr)
    {
        write(' ', Settings::ReturnStmtPadding);
        apJumpStmt->expr()->accept(*this);
    }

    write_eol();
}

void PrettyPrinter::visit(const ast::ListExpr* apListExpr)
{
    const std::vector<const ast::Expr*>& lExpr = apListExpr->exprs();

    write('[');
    write(' ', Settings::SquareBracketPadding);

    for (size_t i = 0; i < lExpr.size(); i++)
    {
        lExpr.at(i)->accept(*this);

        if (i != lExpr.size() - 1)
        {
            write(',');
            write(' ', Settings::SpaceAfterListSeparator);
        }
    }

    write(' ', Settings::SquareBracketPadding);
    write(']');
}

void PrettyPrinter::visit(const ast::Literal* apLiteral)
{
    write(apLiteral->token()->str());
}

void PrettyPrinter::visit(const ast::LiteralStr* apLiteralStr)
{
    write(apLiteralStr->ldelim()->str());

    if (apLiteralStr->str() != nullptr)
    {
        write(apLiteralStr->str()->str());
    }

    write(apLiteralStr->rdelim()->str());
}

void PrettyPrinter::visit(const ast::StrConst* apStrConst)
{
    write(apStrConst->ldelim()->str());

    if (apStrConst->str() != nullptr)
    {
        write(apStrConst->str()->str());
    }

    write(apStrConst->rdelim()->str());
}

void PrettyPrinter::visit(const ast::InterpStr* apInterpStr)
{
    write('$');

    for (ast::Node* pNode : apInterpStr->children())
    {
        pNode->accept(*this);
    }
}

void PrettyPrinter::visit(const ast::MethodCallExpr* apMethodCallExpr)
{
    apMethodCallExpr->receiver()->accept(*this);
    write(apMethodCallExpr->op()->str());
    apMethodCallExpr->call()->accept(*this);
}

void PrettyPrinter::visit(const ast::Program* apProgram)
{
    for (ast::Node* pNode : apProgram->children())
    {
        pNode->accept(*this);
    }

    write_bol();
}

void PrettyPrinter::visit(const ast::StmtList* apStmtList)
{
    for (ast::Node* pNode : apStmtList->children())
    {
        pNode->accept(*this);
    }
}

void PrettyPrinter::visit(const ast::TernaryOp* apTernaryOp)
{
    apTernaryOp->lexpr()->accept(*this);
    write(' ', Settings::OperatorPadding);
    write(apTernaryOp->lop()->str());
    write(' ', Settings::OperatorPadding);
    apTernaryOp->mexpr()->accept(*this);
    write(' ', Settings::OperatorPadding);
    write(apTernaryOp->rop()->str());
    write(' ', Settings::OperatorPadding);
    apTernaryOp->rexpr()->accept(*this);
}

void PrettyPrinter::visit(const ast::UnaryOp* apUnaryOp)
{
    write(apUnaryOp->op()->str());
    apUnaryOp->rexpr()->accept(*this);
}

void PrettyPrinter::visit(const ast::ScopeExpr* apScopeExpr)
{
    write(apScopeExpr->scope()->str());
}

void PrettyPrinter::visit(const ast::Var* apVar)
{
    if (apVar->sigil() != nullptr)
    {
        write(apVar->sigil()->str());
    }

    if (apVar->scope() != nullptr)
    {
        apVar->scope()->accept(*this);
    }

    write(apVar->name()->str());
}

void PrettyPrinter::visit(const ast::WhileStmt* apWhileStmt)
{
    write_bol();

    write(apWhileStmt->ex_cmd_while()->str());
    write(' ', Settings::ControlStmtPadding);
    apWhileStmt->condition()->accept(*this);
    write_eol();

    m_cIndent++;
    apWhileStmt->stmts()->accept(*this);
    m_cIndent--;

    write_bol();
    write(apWhileStmt->ex_cmd_endwile()->str());

    write_eol();
}

void PrettyPrinter::write_bol()
{
    if (m_bNewlinePending)
    {
        m_bNewlinePending = false;
        m_cOutStream << std::endl;
        m_cOutStream << m_cIndent;
    }
}

void PrettyPrinter::write_eol()
{
    m_bNewlinePending = true;
}

void PrettyPrinter::write(const std::string& asText)
{
    m_cOutStream << asText;
}

void PrettyPrinter::write(char anChar, size_t anCount)
{
    if (anCount < 1)
    {
        return;
    }

    m_cOutStream << std::string(anCount, anChar);
}
