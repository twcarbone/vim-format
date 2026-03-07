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
    m_cIndent { Settings::IndentWidth }
{
}

void PrettyPrinter::visit(const ast::AssignStmt* apAssignStmt)
{
    write_bol();

    write_txt("let ");
    apAssignStmt->var()->accept(*this);
    write_txt(" " + apAssignStmt->op()->str() + " ");
    apAssignStmt->expr()->accept(*this);

    write_eol();
}

void PrettyPrinter::visit(const ast::CommentStmt* apCommentStmt)
{
    // TODO (gh-59): PrettyPrinting trailing statement comments

    if (!apCommentStmt->trailing())
    {
        write_bol();
    }

    write_txt(apCommentStmt->comment()->str());

    write_eol();
}

void PrettyPrinter::visit(const ast::BinaryOp* apBinaryOp)
{
    apBinaryOp->lexpr()->accept(*this);
    write_txt(" " + apBinaryOp->op()->str() + " ");
    apBinaryOp->rexpr()->accept(*this);
}

void PrettyPrinter::visit(const ast::CallExpr* apCallExpr)
{
    apCallExpr->callable()->accept(*this);

    write_txt("(");
    apCallExpr->args()->accept(*this);
    write_txt(")");
}

void PrettyPrinter::visit(const ast::CasedBinaryOp* apCasedBinaryOp)
{
    apCasedBinaryOp->lexpr()->accept(*this);
    write_txt(" " + apCasedBinaryOp->op()->str() + apCasedBinaryOp->case_sensitivity()->str() + " ");
    apCasedBinaryOp->rexpr()->accept(*this);
}

void PrettyPrinter::visit(const ast::DictEntry* apDictEntry)
{
    apDictEntry->key()->accept(*this);
    write_txt(": ");
    apDictEntry->value()->accept(*this);
}

void PrettyPrinter::visit(const ast::DictExpr* apDictExpr)
{
    const std::vector<const ast::DictEntry*>& lDictEntries = apDictExpr->entries();

    write_txt("{");

    for (size_t i = 0; i < lDictEntries.size(); i++)
    {
        lDictEntries.at(i)->accept(*this);

        if (i != lDictEntries.size() - 1)
        {
            write_txt(", ");
        }
    }

    write_txt("}");
}

void PrettyPrinter::visit(const ast::ExprCmd* apExprCmd)
{
    write_bol();

    write_txt(apExprCmd->cmd()->str() + " ");
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
            write_txt(", ");
        }
    }
}

void PrettyPrinter::visit(const ast::FnParam* apFnParam)
{
    apFnParam->name()->accept(*this);

    if (apFnParam->default_value() != nullptr)
    {
        write_txt(" = ");
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
            write_txt(", ");
        }
    }
}

void PrettyPrinter::visit(const ast::FnStmt* apFnStmt)
{
    write_bol();

    write_txt("function");

    if (apFnStmt->bang() != nullptr)
    {
        write_txt(apFnStmt->bang()->str());
    }

    write_txt(" " + apFnStmt->name()->str());

    write_txt("(");
    apFnStmt->params()->accept(*this);
    write_txt(")");

    for (const Token* pModifier : apFnStmt->modifiers())
    {
        if (pModifier != nullptr)
        {
            write_txt(" " + pModifier->str());
        }
    }

    write_eol();

    m_cIndent++;
    apFnStmt->body()->accept(*this);
    m_cIndent--;

    write_txt("endfunction");

    write_eol();
}

void PrettyPrinter::visit(const ast::ForStmt* apForStmt)
{
    write_bol();

    write_txt("for ");
    apForStmt->item()->accept(*this);

    write_txt(" in ");
    apForStmt->items()->accept(*this);
    write_eol();

    m_cIndent++;
    apForStmt->stmts()->accept(*this);
    m_cIndent--;

    write_txt("endfor");

    write_eol();
}

void PrettyPrinter::visit(const ast::IfStmt* apIfStmt)
{
    // TODO (gh-60): Handle elseif in PrettyPrinter

    write_bol();

    write_txt("if ");
    apIfStmt->condition()->accept(*this);
    write_eol();

    m_cIndent++;
    apIfStmt->then_stmts()->accept(*this);
    m_cIndent--;

    if (apIfStmt->else_stmts() != nullptr)
    {
        write_ln("else");
        m_cIndent++;
        apIfStmt->else_stmts()->accept(*this);
        m_cIndent--;
    }

    write_txt("endif");

    write_eol();
}

void PrettyPrinter::visit(const ast::IndexExpr* apIndexExpr)
{
    apIndexExpr->indexable()->accept(*this);

    write_txt("[");

    if (apIndexExpr->start() != nullptr)
    {
        apIndexExpr->start()->accept(*this);
    }

    if (apIndexExpr->stop() != nullptr)
    {
        write_txt(":");
        apIndexExpr->stop()->accept(*this);
    }

    write_txt("]");
}

void PrettyPrinter::visit(const ast::JumpStmt* apJumpStmt)
{
    write_bol();

    write_txt(apJumpStmt->token()->str());

    if (apJumpStmt->expr() != nullptr)
    {
        write_txt(" ");
        apJumpStmt->expr()->accept(*this);
    }

    write_eol();
}

void PrettyPrinter::visit(const ast::ListExpr* apListExpr)
{
    const std::vector<const ast::Expr*>& lExpr = apListExpr->exprs();

    write_txt("[");

    for (size_t i = 0; i < lExpr.size(); i++)
    {
        lExpr.at(i)->accept(*this);

        if (i != lExpr.size() - 1)
        {
            write_txt(", ");
        }
    }

    write_txt("]");
}

void PrettyPrinter::visit(const ast::Literal* apLiteral)
{
    write_txt(apLiteral->token()->str());
}

void PrettyPrinter::visit(const ast::MethodCallExpr* apMethodCallExpr)
{
    apMethodCallExpr->receiver()->accept(*this);
    write_txt(apMethodCallExpr->op()->str());
    apMethodCallExpr->call()->accept(*this);
}

void PrettyPrinter::visit(const ast::Program* apProgram)
{
    for (ast::Node* pNode : apProgram->children())
    {
        pNode->accept(*this);
    }
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
    write_txt(" " + apTernaryOp->lop()->str() + " ");
    apTernaryOp->mexpr()->accept(*this);
    write_txt(" " + apTernaryOp->rop()->str() + " ");
    apTernaryOp->rexpr()->accept(*this);
}

void PrettyPrinter::visit(const ast::UnaryOp* apUnaryOp)
{
    write_txt(apUnaryOp->op()->str());
    apUnaryOp->rexpr()->accept(*this);
}

void PrettyPrinter::visit(const ast::Var* apVar)
{
    if (apVar->scope() != nullptr)
    {
        write_txt(apVar->scope()->str() + ":" + apVar->name()->str());
    }
    else
    {
        write_txt(apVar->name()->str());
    }
}

void PrettyPrinter::visit(const ast::WhileStmt* apWhileStmt)
{
    write_bol();

    write_txt("while ");
    apWhileStmt->condition()->accept(*this);
    write_eol();

    m_cIndent++;
    apWhileStmt->stmts()->accept(*this);
    m_cIndent--;

    write_txt("endwhile");

    write_eol();
}

void PrettyPrinter::write_bol()
{
    m_cOutStream << m_cIndent;
}

void PrettyPrinter::write_txt(const std::string& asText)
{
    m_cOutStream << asText;
}

void PrettyPrinter::write_eol()
{
    m_cOutStream << std::endl;
}

void PrettyPrinter::write_ln(const std::string& asLine)
{
    m_cOutStream << m_cIndent << asLine << std::endl;
}
