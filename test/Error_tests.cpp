#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "ASTParser.h"
#include "Exceptions.h"
#include "Lexer.h"
#include "util.h"

class ErrorTest : public testing::Test
{
protected:
    void test_file(const std::string& asPath)
    {
        std::filesystem::path lcSrcPath { asPath };
        std::string lsErrText = vf::read_file(lcSrcPath.replace_extension("out"));

        // Trim trailing newline, if there is one.
        if (lsErrText.back() == '\n')
        {
            lsErrText.erase(lsErrText.size() - 1);
        }

        Context lcContext;
        lcContext.add_path(asPath);

        try
        {
            Lexer lcLexer(lcContext);
            lcLexer.tokenize();

            ASTParser lcParser(lcContext, lcLexer.tokens());
            lcParser.parse();
        }
        catch (const VimError& err)
        {
            EXPECT_EQ(err.what(), lsErrText);
        }
        catch (...)
        {
            FAIL() << "Expected VimError, but a different exception was thrown";
        }
    }
};

TEST_F(ErrorTest, E1278_1)
{
    test_file("../../test/error/E1278_1.vim");
}
