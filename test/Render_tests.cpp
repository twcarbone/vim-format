#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "ASTParser.h"
#include "DocBuilder.h"
#include "Lexer.h"
#include "Renderer.h"
#include "util.h"

class RenderTest : public testing::Test
{
protected:
    Context m_cContext;

    void test_file(const std::string& asInPath, const std::string& asOutPath)
    {
        std::filesystem::path lcSrcPath { asInPath };

        m_cContext.add_path(asInPath);

        Lexer lcLexer(m_cContext);
        lcLexer.tokenize();

        ASTParser lcParser(m_cContext, lcLexer.take_tokens());
        lcParser.parse();

        DocBuilder lcDocBuilder;
        lcParser.root()->accept(lcDocBuilder);

        std::stringstream lcPrettyStrStream;
        Renderer lcRenderer(lcPrettyStrStream);
        lcDocBuilder.root()->accept(lcRenderer);

        std::string lsPrettyStr = vf::read_file(asOutPath);

        EXPECT_EQ(lsPrettyStr, lcPrettyStrStream.str());
    }
};

TEST_F(RenderTest, expr)
{
    test_file("test/pretty/expr.vim", "test/pretty/expr.pretty");
}

TEST_F(RenderTest, expr_pretty)
{
    test_file("test/pretty/expr.pretty", "test/pretty/expr.pretty");
}

TEST_F(RenderTest, ast_eval)
{
    test_file("test/ast/eval.vim", "test/ast/eval.vim");
}

TEST_F(RenderTest, ast_userfunc)
{
    test_file("test/ast/userfunc.vim", "test/ast/userfunc.vim");
}

TEST_F(RenderTest, ast_lockvar)
{
    test_file("test/ast/lockvar.vim", "test/ast/lockvar.vim");
}

#if 0
TEST_F(RenderTest, column_simple_list)
{
    m_cContext.settings().ColumnLimit = 20;
    test_file("test/column/simple_list.in.vim", "test/column/simple_list.out.vim");
}
#endif
