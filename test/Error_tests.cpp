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

        std::string lsEpilogue = "while testing file: " + asPath;

        try
        {
            Lexer lcLexer(lcContext);
            lcLexer.tokenize();

            ASTParser lcParser(lcContext, lcLexer.tokens());
            lcParser.parse();

            FAIL() << "Expected VimError, but did not throw any exception " + lsEpilogue;
        }
        catch (const VimError& err)
        {
            EXPECT_EQ(err.what(), lsErrText) << lsEpilogue;
        }
        catch (...)
        {
            FAIL() << "Expected VimError, but threw a different exception " + lsEpilogue;
        }
    }
};

TEST_F(ErrorTest, test_all)
{
    const std::filesystem::path lcRoot { "../../test/error" };
    const std::filesystem::directory_iterator lcRootIter { lcRoot };

    for (const std::filesystem::directory_entry& lcDirEntry : lcRootIter)
    {
        const std::filesystem::path lcPath { lcDirEntry.path() };

        if (lcPath.extension() == ".vim")
        {
            test_file(lcPath.string());
        }
    }
}
