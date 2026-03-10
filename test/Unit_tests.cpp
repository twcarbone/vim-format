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

TEST(UtilTest, test_is_one_of)
{
    EXPECT_TRUE(vf::is_one_of("a", "a"));
    EXPECT_TRUE(vf::is_one_of("a", "a"));

    EXPECT_FALSE(vf::is_one_of("a", "b"));
    EXPECT_FALSE(vf::is_one_of("ab", "a"));
}

TEST(UtilTest, test_base_name)
{
    EXPECT_EQ(vf::base_name("bar"), "bar");
    EXPECT_EQ(vf::base_name("bar.txt"), "bar");
    EXPECT_EQ(vf::base_name("bar.tar.gz"), "bar");
    EXPECT_EQ(vf::base_name("/bar"), "bar");
    EXPECT_EQ(vf::base_name("/foo/bar.txt"), "bar");
    EXPECT_EQ(vf::base_name("/foo/bar.tar.gz"), "bar");
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
