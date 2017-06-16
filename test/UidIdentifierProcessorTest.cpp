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
#include "danek/internal/UidIdentifierProcessor.h"
#include "danek/ConfigurationException.h"
#include "danek/StringBuffer.h"

using danek::UidIdentifierProcessor;
using danek::StringBuffer;
using danek::ConfigurationException;
using namespace testing;

class UidIdentifierProcessorTest : public testing::Test
{
};

TEST(UidIdentifierProcessorTest, expandSimpleString)
{
    UidIdentifierProcessor p;
    StringBuffer str{"abc"};

    p.expand(str);
    EXPECT_THAT(str.str(), StrEq("abc"));
}

TEST(UidIdentifierProcessorTest, expandWithUid)
{
    UidIdentifierProcessor p;
    StringBuffer str{"uid-xyz"};

    p.expand(str);
    EXPECT_THAT(str.str(), StrEq("uid-000000000-xyz"));
}

TEST(UidIdentifierProcessorTest, expandWithWithouthUidSuffixThrows)
{
    UidIdentifierProcessor p;
    StringBuffer str{"uid-"};
    StringBuffer str2{"uid--"};

    EXPECT_THROW(p.expand(str), ConfigurationException);
    EXPECT_THROW(p.expand(str2), ConfigurationException);
}

