// Copyright (c) 2017 offa
// Copyright 2011 Ciaran McHale.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions.
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "danek/StringBuffer.h"
#include <gmock/gmock.h>

using danek::StringBuffer;
using namespace testing;

class StringBufferTest : public testing::Test
{
protected:
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

TEST_F(StringBufferTest, emptyPerDefault)
{
    StringBuffer sb;
    EXPECT_EQ(0, sb.size());
    EXPECT_THAT(sb.str(), StrEq(""));
}

TEST_F(StringBufferTest, initializeWithString)
{
    StringBuffer sb{"abc"};
    EXPECT_EQ(3, sb.size());
    EXPECT_THAT(sb.str(), StrEq("abc"));
}

TEST_F(StringBufferTest, sizeMatchesElements)
{
    StringBuffer sb{"xy"};
    EXPECT_EQ(2, sb.size());
}

TEST_F(StringBufferTest, accessByIndex)
{
    StringBuffer sb{"xy"};
    const StringBuffer csb{"abc"};
    EXPECT_EQ('y', sb[1]);
    EXPECT_EQ('c', csb[2]);
}

TEST_F(StringBufferTest, stringAccess)
{
    StringBuffer sb{"xy"};
    EXPECT_THAT(sb.str(), StrEq("xy"));
    EXPECT_THAT(sb.c_str(), StrEq("xy"));
}

TEST_F(StringBufferTest, clearElements)
{
    StringBuffer sb{"1234"};
    sb.clear();
    EXPECT_EQ(0, sb.size());
}

TEST_F(StringBufferTest, deleteLastChar)
{
    StringBuffer sb{"1234"};
    sb.deleteLastChar();
    EXPECT_THAT(sb.str(), StrEq("123"));
}

TEST_F(StringBufferTest, appendBuffer)
{
    const StringBuffer toAdd{"ab"};
    StringBuffer sb{"01"};
    sb.append(toAdd);
    sb << StringBuffer{"c"};
    EXPECT_THAT(sb.str(), StrEq("01abc"));
}

TEST_F(StringBufferTest, appendString)
{
    StringBuffer sb{"aa"};
    sb.append("bb");
    sb << "cc";
    EXPECT_THAT(sb.str(), StrEq("aabbcc"));
}

TEST_F(StringBufferTest, appendInt)
{
    StringBuffer sb{"i"};
    sb.append(3);
    sb << 6;
    EXPECT_THAT(sb.str(), StrEq("i36"));
}

TEST_F(StringBufferTest, appendFloat)
{
    StringBuffer sb{"f"};
    sb.append(5.4f);
    sb << 1.5f;
    EXPECT_THAT(sb.str(), MatchesRegex("f5\\.40*1\\.50*"));
}

TEST_F(StringBufferTest, appendChar)
{
    StringBuffer sb{"c"};
    sb.append('u');
    sb << '_';
    EXPECT_THAT(sb.str(), StrEq("cu_"));
}

TEST_F(StringBufferTest, assignNewValue)
{
    StringBuffer sb{"123"};
    sb = "new_string";
    EXPECT_THAT(sb.str(), StrEq("new_string"));
}

