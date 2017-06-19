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

#include <gmock/gmock.h>
#include "danek/internal/UidIdentifierDummyProcessor.h"

using danek::UidIdentifierDummyProcessor;
using danek::StringBuffer;
using namespace testing;

class UidIdentifierDummyProcessorTest : public testing::Test
{
};

TEST(UidIdentifierDummyProcessorTest, expandExpandsNothing)
{
    UidIdentifierDummyProcessor p;
    StringBuffer str{"uid-a.b.uid-123-c"};
    p.expand(str);
    EXPECT_THAT(str.str(), StrEq("uid-a.b.uid-123-c"));
}

TEST(UidIdentifierDummyProcessorTest, unexpandExpandsNothing)
{
    UidIdentifierDummyProcessor p;
    StringBuffer str;
    const auto result = p.unexpand("uid-a.b.uid-123-c", str);
    EXPECT_THAT(result, StrEq("uid-a.b.uid-123-c"));
}

