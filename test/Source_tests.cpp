#include <gtest/gtest.h>

#include "Source.h"

//
// text_test_01
//

TEST(SourceTest, text_test_01)
{
    Source lcSource;
    lcSource.read_text("The quick brown\nfox jumped over the\nlazy dog\n");
    //                  0123456789012345 67890123456789012345 678901234 5
    //                  0         1          2         3          4     ^
    //                                                                  EOF here

    EXPECT_EQ(lcSource.path(), "stdin");

    lcSource.advance(7);

    EXPECT_EQ(lcSource.eof(), false);
    EXPECT_EQ(lcSource.pos(), 7);
    EXPECT_EQ(lcSource.line(), 0);
    EXPECT_EQ(lcSource.column(), 7);
    EXPECT_EQ(lcSource.remaining_text(), "ck brown\nfox jumped over the\nlazy dog\n");
    EXPECT_EQ(lcSource.line_text(), "The quick brown");
    EXPECT_EQ(lcSource.text(), "The quick brown\nfox jumped over the\nlazy dog\n");
    EXPECT_EQ(lcSource.indent(), 0);
    EXPECT_EQ(lcSource.bol(), 0);
    EXPECT_EQ(lcSource.eol(), 15);
    EXPECT_EQ(lcSource.remaining_line(), "ck brown");

    lcSource.advance(10);

    EXPECT_EQ(lcSource.eof(), false);
    EXPECT_EQ(lcSource.pos(), 17);
    EXPECT_EQ(lcSource.line(), 1);
    EXPECT_EQ(lcSource.column(), 1);
    EXPECT_EQ(lcSource.remaining_text(), "ox jumped over the\nlazy dog\n");
    EXPECT_EQ(lcSource.line_text(), "fox jumped over the");
    EXPECT_EQ(lcSource.text(), "The quick brown\nfox jumped over the\nlazy dog\n");
    EXPECT_EQ(lcSource.context(),
              "stdin:2:1\n"
              " 2 | fox jumped over the\n"
              "   |  ^");
    EXPECT_EQ(lcSource.indent(), 0);
    EXPECT_EQ(lcSource.bol(), 16);
    EXPECT_EQ(lcSource.eol(), 35);
    EXPECT_EQ(lcSource.remaining_line(), "ox jumped over the");

    lcSource.advance(19);

    EXPECT_EQ(lcSource.eof(), false);
    EXPECT_EQ(lcSource.pos(), 36);
    EXPECT_EQ(lcSource.line(), 2);
    EXPECT_EQ(lcSource.column(), 0);
    EXPECT_EQ(lcSource.remaining_text(), "lazy dog\n");
    EXPECT_EQ(lcSource.line_text(), "lazy dog");
    EXPECT_EQ(lcSource.text(), "The quick brown\nfox jumped over the\nlazy dog\n");
    EXPECT_EQ(lcSource.indent(), 0);
    EXPECT_EQ(lcSource.bol(), 36);
    EXPECT_EQ(lcSource.eol(), 44);
    EXPECT_EQ(lcSource.remaining_line(), "lazy dog");

    lcSource.advance(8);

    EXPECT_EQ(lcSource.eof(), false);
    EXPECT_EQ(lcSource.pos(), 44);
    EXPECT_EQ(lcSource.line(), 2);
    EXPECT_EQ(lcSource.column(), 8);
    EXPECT_EQ(lcSource.remaining_text(), "\n");
    EXPECT_EQ(lcSource.line_text(), "lazy dog");
    EXPECT_EQ(lcSource.text(), "The quick brown\nfox jumped over the\nlazy dog\n");
    EXPECT_EQ(lcSource.indent(), 0);
    EXPECT_EQ(lcSource.bol(), 36);
    EXPECT_EQ(lcSource.eol(), 44);
    EXPECT_EQ(lcSource.remaining_line(), "");

    lcSource.advance(1);

    EXPECT_EQ(lcSource.eof(), true);
    EXPECT_EQ(lcSource.pos(), 45);
    EXPECT_EQ(lcSource.line(), 2);
    EXPECT_EQ(lcSource.column(), 8);
    EXPECT_EQ(lcSource.remaining_text(), "");
    EXPECT_EQ(lcSource.line_text(), "");
    EXPECT_EQ(lcSource.text(), "The quick brown\nfox jumped over the\nlazy dog\n");
    EXPECT_EQ(lcSource.indent(), 0);
    EXPECT_EQ(lcSource.bol(), 36);
    EXPECT_EQ(lcSource.eol(), 44);
    EXPECT_EQ(lcSource.remaining_line(), "");

    lcSource.seek(23);

    EXPECT_EQ(lcSource.eof(), false);
    EXPECT_EQ(lcSource.pos(), 23);
    EXPECT_EQ(lcSource.line(), 1);
    EXPECT_EQ(lcSource.column(), 7);
    EXPECT_EQ(lcSource.remaining_text(), "ped over the\nlazy dog\n");
    EXPECT_EQ(lcSource.line_text(), "fox jumped over the");
    EXPECT_EQ(lcSource.text(), "The quick brown\nfox jumped over the\nlazy dog\n");
    EXPECT_EQ(lcSource.indent(), 0);
    EXPECT_EQ(lcSource.bol(), 16);
    EXPECT_EQ(lcSource.eol(), 35);
    EXPECT_EQ(lcSource.remaining_line(), "ped over the");
}

//
// text_test_02
//

TEST(SourceTest, text_test_02)
{
    Source lcSource;
    lcSource.read_text("\n");

    EXPECT_EQ(lcSource.path(), "stdin");

    EXPECT_EQ(lcSource.eof(), false);
    EXPECT_EQ(lcSource.pos(), 0);
    EXPECT_EQ(lcSource.line(), 0);
    EXPECT_EQ(lcSource.column(), 0);
    EXPECT_EQ(lcSource.remaining_text(), "\n");
    EXPECT_EQ(lcSource.line_text(), "");
    EXPECT_EQ(lcSource.text(), "\n");
    EXPECT_EQ(lcSource.indent(), 0);
    EXPECT_EQ(lcSource.bol(), 0);
    EXPECT_EQ(lcSource.eol(), 0);
    EXPECT_EQ(lcSource.remaining_line(), "");
}

//
// file_test_01
//

TEST(SourceTest, file_test_01)
{
    Source lcSource;
    lcSource.read_file("../../test/sample/05.vim");

    EXPECT_EQ(lcSource.path(), "../../test/sample/05.vim");

    lcSource.advance(2);

    EXPECT_EQ(lcSource.eof(), false);
    EXPECT_EQ(lcSource.pos(), 2);
    EXPECT_EQ(lcSource.line(), 0);
    EXPECT_EQ(lcSource.column(), 2);
    EXPECT_EQ(lcSource.remaining_text(), "ho 1\n");
    EXPECT_EQ(lcSource.line_text(), "echo 1");
    EXPECT_EQ(lcSource.text(), "echo 1\n");
    EXPECT_EQ(lcSource.context(),
              "../../test/sample/05.vim:1:2\n"
              " 1 | echo 1\n"
              "   |   ^");
    EXPECT_EQ(lcSource.indent(), 0);
    EXPECT_EQ(lcSource.bol(), 0);
    EXPECT_EQ(lcSource.eol(), 6);
    EXPECT_EQ(lcSource.remaining_line(), "ho 1");
}

//
// file_test_02
//

TEST(SourceTest, file_test_02)
{
    Source lcSource;
    lcSource.read_file("../../test/res/while_stmt_01.vim");

    lcSource.advance(30);

    EXPECT_EQ(lcSource.indent(), 4);
}
