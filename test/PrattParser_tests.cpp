#include <gtest/gtest.h>

#include <string>

#include "ExprDisplayVisitor.h"
#include "Lexer.h"
#include "PrattParser.h"
#include "ast.h"

class PrattParserTest : public testing::Test
{
protected:
    Lexer* m_pLexer = nullptr;
    PrattParser* m_pParser = nullptr;
    ExprDisplayVisitor* m_pVisitor = nullptr;

    void TearDown() override
    {
        delete m_pLexer;
        m_pLexer = nullptr;

        delete m_pParser;
        m_pParser = nullptr;

        delete m_pVisitor;
        m_pVisitor = nullptr;
    }

    void parse_str(const std::string& asText, const std::string& asDisplay)
    {
        Context lcContext;
        lcContext.add_text(asText);

        m_pLexer = new Lexer(lcContext);
        m_pLexer->tokenize();

        m_pParser = new PrattParser(lcContext, m_pLexer->tokens());
        m_pParser->parse();

        m_pVisitor = new ExprDisplayVisitor();

        const std::string expected = asDisplay;
        const std::string actual = m_pVisitor->display(m_pParser->root());
        EXPECT_EQ(actual, expected) << "Text: " << m_pLexer->source().text();

        TearDown();
    }
};

//
// expr_01
//  - binary operator & parentheses
//

TEST_F(PrattParserTest, expr_01)
{
    parse_str("1", "1");
    parse_str("1 + 2", "(+ 1 2)");
    parse_str("1 + 2 - 3", "(- (+ 1 2) 3)");
    parse_str("1 + 2 * 3", "(+ 1 (* 2 3))");
    parse_str("(1 + 2) * (a - b)", "(* (+ 1 2) (- a b))");
    parse_str("1 + 2 / (a - b)", "(+ 1 (/ 2 (- a b)))");
    parse_str("a + b / c - d", "(- (+ a (/ b c)) d)");
}

//
// expr_02
//  - unary operator
//

TEST_F(PrattParserTest, expr_02)
{
    parse_str("-1", "(- 1)");
    parse_str("--a", "(- (- a))");
    parse_str("2 + -1", "(+ 2 (- 1))");
    parse_str("-2 + 1", "(+ (- 2) 1)");
    parse_str("-a / +-2 - 5", "(- (/ (- a) (+ (- 2))) 5)");
}

//
// expr_03
//  - index operator
//

TEST_F(PrattParserTest, expr_03)
{
    parse_str("a[1]", "([ a 1)");
    parse_str("a[2 + 3]", "([ a (+ 2 3))");
    parse_str("a[b[1 << 2]]", "([ a ([ b (<< 1 2)))");
}

//
// expr_04
//  - slice operator
//

TEST_F(PrattParserTest, expr_04)
{
    parse_str("a[2:4]", "([ a (: 2 4))");
    parse_str("a[:4]", "([ a (: <begin> 4))");
    parse_str("a[4:]", "([ a (: 4 <end>))");
    parse_str("a[:]", "([ a (: <begin> <end>))");
    parse_str("a[:b[1 + 2:c]]", "([ a (: <begin> ([ b (: (+ 1 2) c))))");
}

//
// expr_05
//  - dot and cat operator
//

TEST_F(PrattParserTest, expr_05)
{
    parse_str("dict.bar", "(. dict bar)");
    parse_str("dict.2", "(. dict 2)");
    parse_str("2 . 2", "(. 2 2)");
}
