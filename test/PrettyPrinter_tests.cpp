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
    void test_file(const std::string& asInPath, const std::string& asOutPath)
    {
        std::filesystem::path lcSrcPath { asInPath };

        Context lcContext;
        lcContext.add_path(asInPath);

        Lexer lcLexer(lcContext);
        lcLexer.tokenize();

        ASTParser lcParser(lcContext, lcLexer.tokens());
        lcParser.parse();

        std::stringstream lcPrettyStrStream;
        PrettyPrinter lcPrettyPrinter(lcPrettyStrStream);
        lcParser.root()->accept(lcPrettyPrinter);

        std::string lsPrettyStr = vf::read_file(asOutPath);

        EXPECT_EQ(lsPrettyStr, lcPrettyStrStream.str());
    }
};

TEST_F(PrettyPrinterTest, expr)
{
    test_file("../../test/pretty/expr.vim", "../../test/pretty/expr.pretty.vim");
}

TEST_F(PrettyPrinterTest, expr_pretty)
{
    test_file("../../test/pretty/expr.pretty.vim", "../../test/pretty/expr.pretty.vim");
}
