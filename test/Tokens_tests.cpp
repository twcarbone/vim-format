#include <gtest/gtest.h>

#include "Context.h"
#include "Lexer.h"
#include "Tokens.h"

TEST(TokensTest, test_01)
{
    Context lcContext;
    lcContext.add_path("../../test/ast/userfunc.vim");

    Lexer lcLexer(lcContext);
    lcLexer.tokenize();

    Tokens lcTokens(lcLexer.take_tokens());

    // pos = 0

    EXPECT_EQ(0, lcTokens.pos());
    EXPECT_EQ(*lcTokens.at(0), *lcTokens.head());
    EXPECT_EQ(*lcTokens.at(0), *lcTokens.advance(0, Flags::skipws));
    EXPECT_EQ(*lcTokens.at(0), *lcTokens.peek(0, Flags::skipws));

    // pos = 3

    EXPECT_EQ(*lcTokens.at(3), *lcTokens.advance(2, Flags::skipws));
    EXPECT_EQ(3, lcTokens.pos());
    EXPECT_EQ(*lcTokens.at(4), *lcTokens.peek(1, Flags::skipws));
    EXPECT_EQ(*lcTokens.at(3), *lcTokens.head());
    EXPECT_EQ(*lcTokens.at(2), *lcTokens.peek(-1));
    EXPECT_EQ(*lcTokens.at(1), *lcTokens.peek(-1, Flags::skipws));
    EXPECT_EQ(3, lcTokens.pos());

    // pos = 7

    EXPECT_EQ(*lcTokens.at(7), *lcTokens.advance(4));
    EXPECT_EQ(7, lcTokens.pos());
}
