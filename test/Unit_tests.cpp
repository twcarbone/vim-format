#include <gtest/gtest.h>

#include "util.h"

TEST(UtilTest, test_is_one_of)
{
    EXPECT_TRUE(vf::is_one_of("a", "a"));
    EXPECT_TRUE(vf::is_one_of("a", "a"));

    EXPECT_FALSE(vf::is_one_of("a", "b"));
    EXPECT_FALSE(vf::is_one_of("ab", "a"));
}
