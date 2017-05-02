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
    EXPECT_EQ(0, v.size());
}

TEST_F(StringVectorTest, copyInitialize)
{
    StringVector toAdd;
    toAdd.push_back("a");

    StringVector v(toAdd);
    EXPECT_EQ(1, v.size());
    EXPECT_EQ(std::string{"a"}, v[0]);
}

TEST_F(StringVectorTest, copyAssign)
{
    StringVector toAdd;
    toAdd.push_back("a");

    StringVector v;
    v = toAdd;
    EXPECT_EQ(1, v.size());
    EXPECT_EQ(std::string{"a"}, v[0]);
}

TEST_F(StringVectorTest, copyAssignOverwritesExistingElements)
{
    StringVector toAdd;
    toAdd.push_back("a");

    StringVector v;
    v.push_back("should-remove");
    v = toAdd;
    EXPECT_EQ(1, v.size());
    EXPECT_EQ(std::string{"a"}, v[0]);
}

TEST_F(StringVectorTest, addString)
{
    StringVector v;
    v.push_back("abc");
    EXPECT_EQ(1, v.size());
    EXPECT_EQ(std::string{"abc"}, v[0]);
}

TEST_F(StringVectorTest, addStringKeepsOrder)
{
    StringVector v;
    v.push_back("a");
    v.push_back("b");
    v.push_back("c");
    EXPECT_EQ(3, v.size());
    EXPECT_EQ(std::string{"a"}, v[0]);
    EXPECT_EQ(std::string{"b"}, v[1]);
    EXPECT_EQ(std::string{"c"}, v[2]);
}

TEST_F(StringVectorTest, addStringVector)
{
    StringVector toAdd;
    toAdd.push_back("a");
    toAdd.push_back("b");

    StringVector v;

    for(const auto& str : toAdd )
    {
        v.push_back(str);
    }
    EXPECT_EQ(2, v.size());
    EXPECT_EQ(std::string{"a"}, v[0]);
    EXPECT_EQ(std::string{"b"}, v[1]);
}

TEST_F(StringVectorTest, addStringBuffer)
{
    StringBuffer buffer("123");

    StringVector v;
    v.push_back(buffer.c_str());
    EXPECT_EQ(1, v.size());
    EXPECT_EQ(std::string{"123"}, v[0]);
}

TEST_F(StringVectorTest, elementAccessByIndex)
{
    StringVector v;
    v.push_back("abc");
    v.push_back("def");
    EXPECT_EQ(std::string{"abc"}, v[0]);
}

TEST_F(StringVectorTest, getCopyOfData)
{
    StringVector v;
    v.push_back("3");
    const auto data = v.get();
    EXPECT_EQ(v.size(), data.size());
    EXPECT_EQ(std::string{"3"}, data[0]);
}

TEST_F(StringVectorTest, lengthMatchesElements)
{
    StringVector v;
    EXPECT_EQ(0, v.size());
    v.push_back("3");
    EXPECT_EQ(1, v.size());
    v.push_back("1");
    EXPECT_EQ(2, v.size());
}

TEST_F(StringVectorTest, emptyClearsElements)
{
    StringVector v;
    v.push_back("3");
    v.clear();
    EXPECT_EQ(0, v.size());
}

TEST_F(StringVectorTest, removeLastRemovesLastElement)
{
    StringVector v;
    v.push_back("x");
    v.push_back("y");
    v.erase(v.end() - 1);
    EXPECT_EQ(1, v.size());
    EXPECT_EQ(std::string{"x"}, v[v.size() - 1]);
}

