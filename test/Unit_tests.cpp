#include <gtest/gtest.h>

#include "util.h"

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
