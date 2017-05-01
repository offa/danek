// Copyright (c) 2017 offa
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

#include "danek/StringVector.h"
#include <memory>
#include <gmock/gmock.h>

using danek::StringVector;
using danek::StringBuffer;

class StringVectorTest : public testing::Test
{
protected:
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

TEST_F(StringVectorTest, emptyPerDefault)
{
    StringVector v;
    EXPECT_EQ(0, v.length());
}

TEST_F(StringVectorTest, copyInitialize)
{
    StringVector toAdd;
    toAdd.add("a");

    StringVector v(toAdd);
    EXPECT_EQ(1, v.length());
    EXPECT_STREQ("a", v[0]);
}

TEST_F(StringVectorTest, copyAssign)
{
    StringVector toAdd;
    toAdd.add("a");

    StringVector v;
    v = toAdd;
    EXPECT_EQ(1, v.length());
    EXPECT_STREQ("a", v[0]);
}

TEST_F(StringVectorTest, copyAssignOverwritesExistingElements)
{
    StringVector toAdd;
    toAdd.add("a");

    StringVector v;
    v.add("should-remove");
    v = toAdd;
    EXPECT_EQ(1, v.length());
    EXPECT_STREQ("a", v[0]);
}

TEST_F(StringVectorTest, addString)
{
    StringVector v;
    v.add("abc");
    EXPECT_EQ(1, v.length());
    EXPECT_STREQ("abc", v[0]);
}

TEST_F(StringVectorTest, addStringKeepsOrder)
{
    StringVector v;
    v.add("a");
    v.add("b");
    v.add("c");
    EXPECT_EQ(3, v.length());
    EXPECT_STREQ("a", v[0]);
    EXPECT_STREQ("b", v[1]);
    EXPECT_STREQ("c", v[2]);
}

TEST_F(StringVectorTest, addStringVector)
{
    StringVector toAdd;
    toAdd.add("a");
    toAdd.add("b");

    StringVector v;
    v.add(toAdd);
    EXPECT_EQ(2, v.length());
    EXPECT_STREQ("a", v[0]);
    EXPECT_STREQ("b", v[1]);
}

TEST_F(StringVectorTest, addStringBuffer)
{
    StringBuffer buffer("123");

    StringVector v;
    v.add(buffer);
    EXPECT_EQ(1, v.length());
    EXPECT_STREQ("123", v[0]);
}

TEST_F(StringVectorTest, elementAccessByIndex)
{
    StringVector v;
    v.add("abc");
    v.add("def");
    EXPECT_STREQ("def", v[1]);
}

TEST_F(StringVectorTest, getCopyOfData)
{
    StringVector v;
    v.add("3");
    const auto data = v.get();
    EXPECT_EQ(v.length(), data.size());
    EXPECT_EQ(std::string{"3"}, data[0]);
}

TEST_F(StringVectorTest, sortSortsElements)
{
    StringVector v;
    v.add("3");
    v.add("1");
    v.add("0");
    v.add("8");
    v.add("3");
    v.add("4");
    v.sort();

    EXPECT_STREQ("0", v[0]);
    EXPECT_STREQ("1", v[1]);
    EXPECT_STREQ("3", v[2]);
    EXPECT_STREQ("3", v[3]);
    EXPECT_STREQ("4", v[4]);
    EXPECT_STREQ("8", v[5]);
}

TEST_F(StringVectorTest, containsIfElementIsFound)
{
    StringVector v;
    v.add("3");
    v.add("1");

    EXPECT_TRUE(v.bSearchContains("1"));
}

TEST_F(StringVectorTest, containsIfElementIsNotFound)
{
    StringVector v;
    v.add("3");
    v.add("1");

    EXPECT_FALSE(v.bSearchContains("2"));
}

TEST_F(StringVectorTest, lengthMatchesElements)
{
    StringVector v;
    EXPECT_EQ(0, v.length());
    v.add("3");
    EXPECT_EQ(1, v.length());
    v.add("1");
    EXPECT_EQ(2, v.length());
}

TEST_F(StringVectorTest, emptyClearsElements)
{
    StringVector v;
    v.add("3");
    v.empty();
    EXPECT_EQ(0, v.length());
}

TEST_F(StringVectorTest, removeLastRemovesLastElement)
{
    StringVector v;
    v.add("x");
    v.add("y");
    v.removeLast();
    EXPECT_EQ(1, v.length());
    EXPECT_STREQ("x", v[v.length() - 1]);
}

TEST_F(StringVectorTest, replaceReplacesElementAtIndex)
{
    StringVector v;
    v.add("3");
    v.add("3");
    v.replace(1, "abc");
    EXPECT_EQ(2, v.length());
    EXPECT_STREQ("abc", v[1]);
}
