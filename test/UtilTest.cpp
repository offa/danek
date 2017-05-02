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

#include "danek/internal/util.h"
#include "danek/StringVector.h"
#include <gmock/gmock.h>

using namespace danek;
using namespace testing;

class UtilTest : public testing::Test
{
};

TEST(UtilTest, splitScopedNameReturnsEmptyStringOnEmptyInput)
{
    StringVector v;
    splitScopedNameIntoVector("", v);
    EXPECT_EQ(1, v.size());
    EXPECT_THAT(v[0], IsEmpty());
}

TEST(UtilTest, splitScopedNameReturnsSplittedResults)
{
    StringVector v;
    splitScopedNameIntoVector("a.b.c", v);
    EXPECT_EQ(3, v.size());
    EXPECT_THAT(v[0], Eq("a"));
    EXPECT_THAT(v[1], Eq("b"));
    EXPECT_THAT(v[2], Eq("c"));
}

