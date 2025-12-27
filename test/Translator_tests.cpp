#include <gtest/gtest.h>

#include "AST.h"
#include "Context.h"
#include "Lexer.h"
#include "Parser.h"
#include "Translator.h"

class TranslatorTest : public testing::Test
{
protected:
    Lexer* m_pLexer = nullptr;
    Parser* m_pParser = nullptr;
    Translator* m_pTranslator = nullptr;
    NodeVisitor* m_pVisitor = nullptr;

    void TearDown() override
    {
        delete m_pLexer;
        m_pLexer = nullptr;

        delete m_pParser;
        m_pParser = nullptr;

        delete m_pTranslator;
        m_pTranslator = nullptr;

        delete m_pVisitor;
        m_pVisitor = nullptr;
    }

    void translate_str(const std::string& asText)
    {
        Context lcContext;
        lcContext.add_text(asText);

        m_pLexer = new Lexer(lcContext);
        m_pLexer->tokenize();

        m_pParser = new Parser(lcContext, m_pLexer->tokens());
        m_pParser->parse();

        m_pTranslator = new Translator(lcContext);
        m_pTranslator->translate(m_pParser->root());
    }

    void expect_nodes(size_t anCount)
    {
        const size_t expected = anCount;
        const size_t actual = m_pVisitor->accumulate(m_pTranslator->root()).size();
        EXPECT_EQ(actual, expected) << "Text: " << m_pLexer->source().text();
    }

    void expect_node(const std::vector<int>& alPath, const std::string& asStr)
    {
        const std::string expected = m_pVisitor->get_node(m_pTranslator->root(), alPath)->toString();
        const std::string actual = asStr;
        EXPECT_EQ(actual, expected) << "Text: " << m_pLexer->source().text();
    }
};

//
// test_01
//

TEST_F(TranslatorTest, test_01)
{
    translate_str("echo 1 + 2 * 3\n");

    expect_nodes(8);
    expect_node({}, "Program(1)");
    expect_node({ 0 }, "StmtList(1)");
    expect_node({ 0, 0 }, "CmdExpr(1):echo");
    expect_node({ 0, 0, 0 }, "BinOp(2):+");
    expect_node({ 0, 0, 0, 0 }, "Literal(0):1");
    expect_node({ 0, 0, 0, 1 }, "BinOp(2):*");
    expect_node({ 0, 0, 0, 1, 0 }, "Literal(0):2");
    expect_node({ 0, 0, 0, 1, 1 }, "Literal(0):3");
    expect_node({ 0, 0, 0, 1, 1 }, "Literal(0):3");
}

//
// test_02
//

TEST_F(TranslatorTest, test_02)
{
    translate_str("echo 1\nlet i += 2\n");

    expect_nodes(7);
    expect_node({}, "Program(1)");
    expect_node({ 0 }, "StmtList(2)");
    expect_node({ 0, 0 }, "CmdExpr(1):echo");
    expect_node({ 0, 0, 0 }, "Literal(0):1");
    expect_node({ 0, 1 }, "CmdExpr(2):+=");
    expect_node({ 0, 1, 0 }, "Var(0):i");
    expect_node({ 0, 1, 1 }, "Literal(0):2");
}

//
// test_03
//

TEST_F(TranslatorTest, test_03)
{
    translate_str("if 1\necho 2\nelse\necho 3\nendif\n");

    expect_nodes(10);
    expect_node({}, "Program(1)");
    expect_node({ 0 }, "StmtList(1)");
    expect_node({ 0, 0 }, "CmdExpr(3):if");
    expect_node({ 0, 0, 0 }, "Literal(0):1");
    expect_node({ 0, 0, 1 }, "StmtList(1)");
    expect_node({ 0, 0, 1, 0 }, "CmdExpr(1):echo");
    expect_node({ 0, 0, 1, 0, 0 }, "Literal(0):2");
    expect_node({ 0, 0, 2 }, "StmtList(1)");
    expect_node({ 0, 0, 2, 0 }, "CmdExpr(1):echo");
    expect_node({ 0, 0, 2, 0, 0 }, "Literal(0):3");
}
