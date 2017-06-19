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

TEST(UidIdentifierProcessorTest, expandString)
{
    UidIdentifierProcessor p;
    StringBuffer str{"abc"};

    p.expand(str);
    EXPECT_THAT(str.str(), StrEq("abc"));
}

TEST(UidIdentifierProcessorTest, expandWithScope)
{
    UidIdentifierProcessor p;
    StringBuffer str{"a.b.c"};

    p.expand(str);
    EXPECT_THAT(str.str(), StrEq("a.b.c"));
}

TEST(UidIdentifierProcessorTest, expandWithScopeAndUid)
{
    UidIdentifierProcessor p;
    StringBuffer str{"a.uid-b.uid-0013-c"};

    p.expand(str);
    EXPECT_THAT(str.str(), StrEq("a.uid-000000000-b.uid-000000001-c"));
}

TEST(UidIdentifierProcessorTest, expandWithUid)
{
    UidIdentifierProcessor p;
    StringBuffer str{"uid-xyz"};

    p.expand(str);
    EXPECT_THAT(str.str(), StrEq("uid-000000000-xyz"));
}

TEST(UidIdentifierProcessorTest, expandWithUidIncreasesUid)
{
    UidIdentifierProcessor p;
    StringBuffer str{"uid-x.uid-y.uid-z"};

    p.expand(str);
    EXPECT_THAT(str.str(), StrEq("uid-000000000-x.uid-000000001-y.uid-000000002-z"));
}

TEST(UidIdentifierProcessorTest, expandMaintaintsUidState)
{
    UidIdentifierProcessor p;
    StringBuffer str{"uid-x"};
    StringBuffer str2{"uid-y"};
    StringBuffer str3{"uid-z"};

    p.expand(str);
    p.expand(str2);
    p.expand(str3);
    EXPECT_THAT(str.str(), StrEq("uid-000000000-x"));
    EXPECT_THAT(str2.str(), StrEq("uid-000000001-y"));
    EXPECT_THAT(str3.str(), StrEq("uid-000000002-z"));
}

TEST(UidIdentifierProcessorTest, expandWithUidAndNumber)
{
    UidIdentifierProcessor p;
    StringBuffer str{"uid-01234-xyz"};

    p.expand(str);
    EXPECT_THAT(str.str(), StrEq("uid-000000000-xyz"));
}

TEST(UidIdentifierProcessorTest, expandWithWithouthUidSuffixThrows)
{
    UidIdentifierProcessor p;
    StringBuffer str{"uid-"};
    StringBuffer str2{"uid--"};
    StringBuffer str3{"uid--abc"};
    StringBuffer str4{"uid-9abc"};
    StringBuffer str5{"uid-123-"};
    StringBuffer str6{"uid-123--a"};
    StringBuffer str7{"uid-123-456a"};

    EXPECT_THROW(p.expand(str), ConfigurationException);
    EXPECT_THROW(p.expand(str2), ConfigurationException);
    EXPECT_THROW(p.expand(str3), ConfigurationException);
    EXPECT_THROW(p.expand(str4), ConfigurationException);
    EXPECT_THROW(p.expand(str5), ConfigurationException);
    EXPECT_THROW(p.expand(str6), ConfigurationException);
    EXPECT_THROW(p.expand(str7), ConfigurationException);
}

TEST(UidIdentifierProcessorTest, unexpandString)
{
    UidIdentifierProcessor p;
    StringBuffer str;

    const auto result = p.unexpand("abc", str);
    EXPECT_THAT(result, StrEq("abc"));
}

TEST(UidIdentifierProcessorTest, unexpandWithScope)
{
    UidIdentifierProcessor p;
    StringBuffer str;

    const auto result = p.unexpand("a.b.c", str);
    EXPECT_THAT(result, StrEq("a.b.c"));
}

TEST(UidIdentifierProcessorTest, unexpandWithScopeAndUid)
{
    UidIdentifierProcessor p;
    StringBuffer str;

    const auto result = p.unexpand("a.uid-000000000-b.uid-000000001-c", str);
    EXPECT_THAT(result, StrEq("a.uid-b.uid-c"));
}

TEST(UidIdentifierProcessorTest, unexpandWithUid)
{
    UidIdentifierProcessor p;
    StringBuffer str;

    const auto result = p.unexpand("uid-000000003-xyz", str);
    EXPECT_THAT(result, StrEq("uid-xyz"));
}

