#include <gtest/gtest.h>

#include "util.h"

class FilesystemTest : public testing::Test
{
protected:
    const std::string m_s1 = "/home/user/pkg.tar.gz";
    const std::string m_s2 = "/home/user/";
    const std::string m_s3 = "/home/user";
    const std::string m_s4 = "/home/user/.ssh/known_hosts";
    const std::string m_s5 = "/home/user/.config/.tmux.conf";
    const std::string m_s6 = "/home/user/.vimrc";
    const std::string m_s7 = "/etc/yum.repos.d/epel.repo";
    const std::string m_s8 = "/";
};

class UtilTest : public testing::Test
{
protected:
    void test_split(const std::string& asStr, char asDelim, std::vector<std::string> alParts)
    {
        EXPECT_EQ(vf::split(asStr, asDelim), alParts);
    }

    void test_startswithint(std::string_view asStr, std::string_view expected_out, int expected_base)
    {
        std::string_view actual_out;
        int actual_base;

        EXPECT_TRUE(vf::startswith_int(asStr, actual_out, &actual_base));
        EXPECT_EQ(expected_out, actual_out);
        EXPECT_EQ(expected_base, actual_base);
    }

    void test_startswithfloat(std::string_view asStr, std::string_view expected_out)
    {
        std::string_view actual_out;

        EXPECT_TRUE(vf::startswith_float(asStr, actual_out));
        EXPECT_EQ(expected_out, actual_out);
    }
};

TEST_F(UtilTest, test_is_one_of)
{
    EXPECT_TRUE(vf::is_one_of("a", "a"));
    EXPECT_TRUE(vf::is_one_of("a", "a"));

    EXPECT_FALSE(vf::is_one_of("a", "b"));
    EXPECT_FALSE(vf::is_one_of("ab", "a"));
}

TEST_F(UtilTest, test_split)
{
    test_split("aa,bb,cc", ',', { "aa", "bb", "cc" });
    test_split("the\n\n\nyellow\nhouse", '\n', { "the", "", "", "yellow", "house" });
    test_split("1//2//", '/', { "1", "", "2", "", "" });
    test_split("hello", '.', { "hello" });
    test_split("a + b ", '+', { "a ", " b " });
}

TEST_F(UtilTest, test_startswithint)
{
    test_startswithint("42_", "42", 10);
    test_startswithint("423_", "423", 10);
    test_startswithint("004_", "004", 10);
    test_startswithint("0x42_", "0x42", 16);
    test_startswithint("0X42_", "0X42", 16);
    test_startswithint("0x42ff_", "0x42ff", 16);
    test_startswithint("0Xa42f_", "0Xa42f", 16);
    test_startswithint("0o42_", "0o42", 8);
    test_startswithint("0O42_", "0O42", 8);
    test_startswithint("0b1101_", "0b1101", 2);
    test_startswithint("0B1101_", "0B1101", 2);
}

TEST_F(UtilTest, test_startswithflost)
{
    test_startswithfloat("1.0_", "1.0");
    test_startswithfloat("55.0_", "55.0");
    test_startswithfloat("123.456_", "123.456");
    test_startswithfloat("1.234e03_", "1.234e03");
    test_startswithfloat("1.234e-03_", "1.234e-03");
    test_startswithfloat("1.234e+03_", "1.234e+03");
    test_startswithfloat("1.234E03_", "1.234E03");
    test_startswithfloat("1.234E-03_", "1.234E-03");
    test_startswithfloat("1.234E+03_", "1.234E+03");
}

TEST_F(UtilTest, test_sanitize)
{
    std::string lsIn = { 0, 2, 3, 9, 10, 32, 57, 80, 100, 123 };
    std::string lsOut = R"(\0\x02\x03\t\n 9Pd{)";
    EXPECT_EQ(lsOut, vf::sanitize(lsIn));
}

TEST_F(FilesystemTest, test_name)
{
    EXPECT_EQ(vf::name(m_s1), "pkg.tar.gz");
    EXPECT_EQ(vf::name(m_s2), "user");
    EXPECT_EQ(vf::name(m_s3), "user");
    EXPECT_EQ(vf::name(m_s4), "known_hosts");
    EXPECT_EQ(vf::name(m_s5), ".tmux.conf");
    EXPECT_EQ(vf::name(m_s6), ".vimrc");
    EXPECT_EQ(vf::name(m_s7), "epel.repo");
    EXPECT_EQ(vf::name(m_s8), "");
}

TEST_F(FilesystemTest, test_stem)
{
    EXPECT_EQ(vf::stem(m_s1), "pkg");
    EXPECT_EQ(vf::stem(m_s2), "user");
    EXPECT_EQ(vf::stem(m_s3), "user");
    EXPECT_EQ(vf::stem(m_s4), "known_hosts");
    EXPECT_EQ(vf::stem(m_s5), ".tmux");
    EXPECT_EQ(vf::stem(m_s6), ".vimrc");
    EXPECT_EQ(vf::stem(m_s7), "epel");
    EXPECT_EQ(vf::stem(m_s8), "");
}

TEST_F(FilesystemTest, test_tail)
{
    EXPECT_EQ(vf::tail(m_s1), "tar.gz");
    EXPECT_EQ(vf::tail(m_s2), "");
    EXPECT_EQ(vf::tail(m_s3), "");
    EXPECT_EQ(vf::tail(m_s4), "");
    EXPECT_EQ(vf::tail(m_s5), "conf");
    EXPECT_EQ(vf::tail(m_s6), "");
    EXPECT_EQ(vf::tail(m_s7), "repo");
    EXPECT_EQ(vf::tail(m_s8), "");
}

TEST_F(FilesystemTest, test_with_tail)
{
    EXPECT_EQ(vf::with_tail(m_s1, "cpp").string(), "/home/user/pkg.cpp");
    EXPECT_EQ(vf::with_tail(m_s2, "d").string(), "/home/user.d");
    EXPECT_EQ(vf::with_tail(m_s3, "d").string(), "/home/user.d");
    EXPECT_EQ(vf::with_tail(m_s4, "").string(), "/home/user/.ssh/known_hosts");
    EXPECT_EQ(vf::with_tail(m_s5, "conf.bak").string(), "/home/user/.config/.tmux.conf.bak");
    EXPECT_EQ(vf::with_tail(m_s6, "2").string(), "/home/user/.vimrc.2");
    EXPECT_EQ(vf::with_tail(m_s7, "tar.gz").string(), "/etc/yum.repos.d/epel.tar.gz");

    EXPECT_THROW(vf::with_tail(m_s8, "cpp"), std::runtime_error);
}
