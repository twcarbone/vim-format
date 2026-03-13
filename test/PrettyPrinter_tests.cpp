#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "ASTParser.h"
#include "Lexer.h"
#include "PrettyPrinter.h"
#include "util.h"

class PrettyPrinterTest : public testing::Test
{
protected:
    void test_file(const std::string& asPath)
    {
        std::filesystem::path lcSrcPath { asPath };

        Context lcContext;
        lcContext.add_path(asPath);

        Lexer lcLexer(lcContext);
        lcLexer.tokenize();

        ASTParser lcParser(lcContext, lcLexer.tokens());
        lcParser.parse();

        std::stringstream lcPrettyStrStream;
        PrettyPrinter lcPrettyPrinter(lcPrettyStrStream);
        lcParser.root()->accept(lcPrettyPrinter);

        std::string lsPrettyStr = vf::read_file(vf::with_tail(lcSrcPath, "pretty.vim"));

        EXPECT_EQ(lcPrettyStrStream.str(), lsPrettyStr);
    }
};

TEST_F(PrettyPrinterTest, expr)
{
    test_file("../../test/pretty/expr.vim");
}

TEST_F(PrettyPrinterTest, expr_pretty)
{
    test_file("../../test/pretty/expr.pretty.vim");
}
