#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "ASTParser.h"
#include "ASTVisitor.h"
#include "Lexer.h"
#include "util.h"

class FunctionalTest : public testing::Test
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

        std::stringstream lcAstStrStream;
        ASTVisitor lcVisitor(lcAstStrStream);
        lcParser.root()->accept(lcVisitor);

        std::string lsAstStr = vf::read_file(lcSrcPath.replace_extension("ast"));

        EXPECT_EQ(lcAstStrStream.str(), lsAstStr);
    }
};

TEST_F(FunctionalTest, if_stmt)
{
    test_file("../../test/ast/if_stmt.vim");
}

TEST_F(FunctionalTest, func_stmt)
{
    test_file("../../test/ast/func_stmt.vim");
}

TEST_F(FunctionalTest, while_stmt)
{
    test_file("../../test/ast/while_stmt.vim");
}

TEST_F(FunctionalTest, expr)
{
    test_file("../../test/ast/expr.vim");
}

TEST_F(FunctionalTest, comment)
{
    test_file("../../test/ast/comment.vim");
}
