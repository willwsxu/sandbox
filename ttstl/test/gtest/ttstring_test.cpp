#include <ttstl/string.h>
#include <gtest/gtest.h>

#include <cstdlib>
#include <string>

namespace {

TEST(StringTest, VerifyNumericConversions)
{
    {
        tt::String test("123");
        EXPECT_EQ(tt::stol(test), 123L);
        EXPECT_EQ(tt::stoul(test), 123UL);
        EXPECT_EQ(tt::stoll(test), 123LL);
        EXPECT_EQ(tt::stoull(test), 123ULL);
    }

    {
        tt::String test("123.45");
        EXPECT_FLOAT_EQ(tt::stof(test), 123.45f);
        EXPECT_DOUBLE_EQ(tt::stod(test), 123.45);
        EXPECT_DOUBLE_EQ(tt::stold(test), 123.45);
    }
}

TEST(StringTest, VerifyToString)
{
    {
        EXPECT_EQ(tt::to_string((int)0), "0");
        EXPECT_EQ(tt::to_string((int)123), "123");
        EXPECT_EQ(tt::to_string((int)-123), "-123");
        EXPECT_EQ(tt::to_string((int)-2147483648), "-2147483648");
    }

    {
        EXPECT_EQ(tt::to_string((unsigned)0), "0");
        EXPECT_EQ(tt::to_string((unsigned)123), "123");
    }

    //{
        //EXPECT_EQ(tt::to_string(123.45f), "123.45");
        //EXPECT_EQ(tt::to_string(123.45), "123.45");
    //}
}

} // anonymous namespace
