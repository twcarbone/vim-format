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

TEST_F(FilesystemTest, test_with_stem)
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
