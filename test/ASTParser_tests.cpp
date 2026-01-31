#include <gtest/gtest.h>

#include <string>

#include "ASTParser.h"
#include "Exceptions.h"
#include "Lexer.h"

class ASTParserTest : public testing::Test
{
protected:
    Lexer* m_pLexer = nullptr;
    ASTParser* m_pParser = nullptr;

    void TearDown() override
    {
        delete m_pLexer;
        m_pLexer = nullptr;

        delete m_pParser;
        m_pParser = nullptr;
    }

    void parse_str(const std::string& asText)
    {
        Context lcContext;
        lcContext.add_text(asText);

        m_pLexer = new Lexer(lcContext);
        m_pLexer->tokenize();

        m_pParser = new ASTParser(lcContext, m_pLexer->tokens());
        m_pParser->parse();

        TearDown();
    }
};

//
// if_stmt
//

TEST_F(ASTParserTest, if_stmt_01)
{
    parse_str("if 1\n"
              "  echo 1\n"
              "endif\n");
}

TEST_F(ASTParserTest, if_stmt_02)
{
    parse_str("if 1\n"
              "  echo 1\n"
              "else\n"
              "  echo 2\n"
              "endif\n");
}

TEST_F(ASTParserTest, if_stmt_03)
{
    parse_str("if 1\n"
              "  echo 1\n"
              "elseif 1\n"
              "  echo 2\n"
              "endif\n");
}

TEST_F(ASTParserTest, if_stmt_04)
{
    parse_str("if 1\n"
              "  echo 1\n"
              "elseif 1\n"
              "  echo 2\n"
              "else\n"
              "  echo 3\n"
              "endif\n");
}

//
// while_stmt
//

TEST_F(ASTParserTest, while_stmt_01)
{
    parse_str("while 1\n"
              "  echo 1\n"
              "endwhile\n");
}

//
// func_stmt
//

TEST_F(ASTParserTest, func_stmt_01)
{
    parse_str("function! Foo() range abort dict closure\n"
              "    echo 1\n"
              "endfunction\n");
}

TEST_F(ASTParserTest, func_stmt_02)
{
    parse_str("function! Foo(...)\n"
              "    echo 1\n"
              "endfunction\n");
}

TEST_F(ASTParserTest, func_stmt_03)
{
    parse_str("function! Foo(apple)\n"
              "    echo 1\n"
              "endfunction\n");
}

TEST_F(ASTParserTest, func_stmt_04)
{
    parse_str("function! Foo(apple,)\n"
              "    echo 1\n"
              "endfunction\n");
}

TEST_F(ASTParserTest, func_stmt_05)
{
    parse_str("function! Foo(apple, ...)\n"
              "    return 1\n"
              "endfunction\n");
}

TEST_F(ASTParserTest, func_stmt_06)
{
    parse_str("function! Foo(apple, banana = 1)\n"
              "    return\n"
              "endfunction\n");
}

//
// bad_func_stmt
//

TEST_F(ASTParserTest, bad_func_stmt_01)
{
    EXPECT_THROW(parse_str("function Bar(a,,)\nendfunction\n"), VimError);
    EXPECT_THROW(parse_str("function Bar(,a)\nendfunction\n"), VimError);
    EXPECT_THROW(parse_str("function Bar(,)\nendfunction\n"), VimError);
    EXPECT_THROW(parse_str("function Bar(...,)\nendfunction\n"), VimError);
    EXPECT_THROW(parse_str("function Bar(..., b)\nendfunction\n"), VimError);
    EXPECT_THROW(parse_str("function Bar(a b)\nendfunction\n"), VimError);
    EXPECT_THROW(parse_str("function Bar(a ...)\nendfunction\n"), VimError);
    EXPECT_THROW(parse_str("function Bar(a = 1, b)\nendfunction\n"), VimError);
}

//
// expr
//

TEST_F(ASTParserTest, expr_01)
{
    parse_str("echo foo ?? bar\n");
    parse_str("echo 1 ?? 2\n");

    parse_str("echo foo ? bar : baz\n");
    parse_str("echo 1 ? 2 : 3\n");
}

TEST_F(ASTParserTest, expr_02)
{
    parse_str("echo 1 || 2\n");
    parse_str("echo foo || bar\n");
}

TEST_F(ASTParserTest, expr_03)
{
    parse_str("echo 1 && 2\n");
    parse_str("echo foo && bar\n");
}

TEST_F(ASTParserTest, expr_04)
{
    parse_str("echo 1 == 2\n");
    parse_str("echo 1 ==? 2\n");
    parse_str("echo 1 ==# 2\n");
    parse_str("echo 1 != 2\n");
    parse_str("echo 6 >  4\n");
    parse_str("echo 1 >= 5\n");
    parse_str("echo 5 <  3\n");
    parse_str("echo 3 <= 2\n");
    parse_str("echo foo is bar\n");
    parse_str("echo foo isnot bar\n");
}

TEST_F(ASTParserTest, expr_05)
{
    parse_str("echo 1 << 1\n");
    parse_str("echo 2 >> 1\n");
}

TEST_F(ASTParserTest, expr_06)
{
    parse_str("echo 1 + 2\n");
    parse_str("echo 2 - 1\n");
    parse_str("echo sp .. am\n");
    parse_str("echo sp . am\n");
}

TEST_F(ASTParserTest, expr_07)
{
    parse_str("echo 2 * 1\n");
    parse_str("echo 6 / 2\n");
    parse_str("echo 6 % 4\n");
}

TEST_F(ASTParserTest, expr_08)
{
    // TODO (gh-8)
}

TEST_F(ASTParserTest, expr_09)
{
    parse_str("echo !1\n");
    parse_str("echo -1\n");
    parse_str("echo +1\n");
}

TEST_F(ASTParserTest, expr_10)
{
    parse_str("echo foo[3]\n");
    parse_str("echo foo[1:3]\n");
    parse_str("echo foo[1:]\n");
    parse_str("echo foo[:3]\n");
    parse_str("echo foo[:]\n");
    parse_str("echo foo.bar\n");
}

TEST_F(ASTParserTest, expr_11)
{
    parse_str("echo [1, 2]\n");
    parse_str("echo [foo, bar,]\n");
    parse_str("echo [1,2,3,4,5]\n");
}
