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

#include "danek/internal/UidIdentifierProcessor.h"
#include "danek/ConfigurationException.h"
#include "danek/StringBuffer.h"
#include <gmock/gmock.h>

using danek::UidIdentifierProcessor;
using danek::StringBuffer;
using danek::ConfigurationException;
using namespace testing;

class UidIdentifierProcessorTest : public testing::Test
{
public:

    void SetUp() override
    {
        processor = std::make_unique<UidIdentifierProcessor>();
    }

    std::unique_ptr<UidIdentifierProcessor> processor;
};

TEST_F(UidIdentifierProcessorTest, expandString)
{
    const auto s = processor->expand("abc");
    EXPECT_THAT(s, StrEq("abc"));
}

TEST_F(UidIdentifierProcessorTest, expandWithScope)
{
    const auto s = processor->expand("a.b.c");
    EXPECT_THAT(s, StrEq("a.b.c"));
}

TEST_F(UidIdentifierProcessorTest, expandWithScopeAndUid)
{
    const auto s = processor->expand("a.uid-b.uid-0013-c");
    EXPECT_THAT(s, StrEq("a.uid-000000000-b.uid-000000001-c"));
}

TEST_F(UidIdentifierProcessorTest, expandWithUid)
{
    const auto s = processor->expand("uid-xyz");
    EXPECT_THAT(s, StrEq("uid-000000000-xyz"));
}

TEST_F(UidIdentifierProcessorTest, expandWithUidIncreasesUid)
{
    const auto s = processor->expand("uid-x.uid-y.uid-z");
    EXPECT_THAT(s, StrEq("uid-000000000-x.uid-000000001-y.uid-000000002-z"));
}

TEST_F(UidIdentifierProcessorTest, expandMaintaintsUidState)
{
    const auto s = processor->expand("uid-x");
    const auto s1 = processor->expand("uid-y");
    const auto s2 = processor->expand("uid-z33");
    EXPECT_THAT(s , StrEq("uid-000000000-x"));
    EXPECT_THAT(s1, StrEq("uid-000000001-y"));
    EXPECT_THAT(s2, StrEq("uid-000000002-z33"));
}

TEST_F(UidIdentifierProcessorTest, expandWithUidAndNumber)
{
    const auto s = processor->expand("uid-01234-xyz12");
    EXPECT_THAT(s, StrEq("uid-000000000-xyz12"));
}

TEST_F(UidIdentifierProcessorTest, expandWithWithouthUidSuffixThrows)
{
    EXPECT_THROW(processor->expand("uid-"), ConfigurationException);
    EXPECT_THROW(processor->expand("uid--"), ConfigurationException);
    EXPECT_THROW(processor->expand("uid--abc"), ConfigurationException);
    EXPECT_THROW(processor->expand("uid-9abc"), ConfigurationException);
    EXPECT_THROW(processor->expand("uid-123-"), ConfigurationException);
    EXPECT_THROW(processor->expand("uid-123--a"), ConfigurationException);
    EXPECT_THROW(processor->expand("uid-123-456a"), ConfigurationException);
    EXPECT_THROW(processor->expand("uid-9"), ConfigurationException);
}

TEST_F(UidIdentifierProcessorTest, unexpandString)
{
    StringBuffer str;

    const auto result = processor->unexpand("abc", str);
    EXPECT_THAT(result, StrEq("abc"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithScope)
{
    StringBuffer str;

    const auto result = processor->unexpand("a.b.c", str);
    EXPECT_THAT(result, StrEq("a.b.c"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithScopeAndUid)
{
    StringBuffer str;

    const auto result = processor->unexpand("a.uid-000000000-b.uid-000000001-c", str);
    EXPECT_THAT(result, StrEq("a.uid-b.uid-c"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithUid)
{
    StringBuffer str;

    const auto result = processor->unexpand("uid-xyz", str);
    EXPECT_THAT(result, StrEq("uid-xyz"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithUidAndNumber)
{
    StringBuffer str;

    const auto result = processor->unexpand("uid-000000003-xyz", str);
    EXPECT_THAT(result, StrEq("uid-xyz"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithUidAndUidWithNumbers)
{
    StringBuffer str;

    const auto result = processor->unexpand("uid-000000000-x.uid-y.uid-000000002-z", str);
    EXPECT_THAT(result, StrEq("uid-x.uid-y.uid-z"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithDifferentUid)
{
    StringBuffer str;
    StringBuffer str2;
    StringBuffer str3;

    const auto result1 =  processor->unexpand("uid-000000000-x", str);
    const auto result2 = processor->unexpand("uid-000000001-y", str2);
    const auto result3 = processor->unexpand("uid-000000002-z", str3);
    EXPECT_THAT(result1, StrEq("uid-x"));
    EXPECT_THAT(result2, StrEq("uid-y"));
    EXPECT_THAT(result3, StrEq("uid-z"));
}

