#include <gtest/gtest.h>

#include <string>

#include "HybridParser.h"
#include "Lexer.h"

class HybridParserTest : public testing::Test
{
protected:
    Lexer* m_pLexer = nullptr;
    HybridParser* m_pParser = nullptr;

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

        m_pParser = new HybridParser(lcContext, m_pLexer->tokens());
        m_pParser->parse();

        TearDown();
    }
};

//
// if_stmt
//

TEST_F(HybridParserTest, if_stmt_01)
{
    parse_str("if 1\n"
              "  echo 1\n"
              "endif\n");
}

TEST_F(HybridParserTest, if_stmt_02)
{
    parse_str("if 1\n"
              "  echo 1\n"
              "else\n"
              "  echo 2\n"
              "endif\n");
}

TEST_F(HybridParserTest, if_stmt_03)
{
    parse_str("if 1\n"
              "  echo 1\n"
              "elseif 1\n"
              "  echo 2\n"
              "endif\n");
}

TEST_F(HybridParserTest, if_stmt_04)
{
    parse_str("if 1\n"
              "  echo 1\n"
              "elseif 1\n"
              "  echo 2\n"
              "else\n"
              "  echo 3\n"
              "endif\n");
}
