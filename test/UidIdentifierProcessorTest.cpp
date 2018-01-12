// Copyright (c) 2017-2018 offa
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
#include <gmock/gmock.h>

using danek::ConfigurationException;
using danek::UidIdentifierProcessor;
using namespace testing;

namespace
{
    struct UidIdentifierProcessorMock : public UidIdentifierProcessor
    {
        void advanceCount(std::size_t n)
        {
            nextCount(n);
        }
    };
}


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
    const auto result = processor->expand("abc");
    EXPECT_THAT(result, StrEq("abc"));
}

TEST_F(UidIdentifierProcessorTest, expandWithScope)
{
    const auto result = processor->expand("a.b.c");
    EXPECT_THAT(result, StrEq("a.b.c"));
}

TEST_F(UidIdentifierProcessorTest, expandWithScopeAndUid)
{
    const auto result = processor->expand("a.uid-b.uid-0013-c");
    EXPECT_THAT(result, StrEq("a.uid-000000000-b.uid-000000001-c"));
}

TEST_F(UidIdentifierProcessorTest, expandWithUid)
{
    const auto result = processor->expand("uid-xyz");
    EXPECT_THAT(result, StrEq("uid-000000000-xyz"));
}

TEST_F(UidIdentifierProcessorTest, expandWithUidIncreasesUid)
{
    const auto result = processor->expand("uid-x.uid-y.uid-z");
    EXPECT_THAT(result, StrEq("uid-000000000-x.uid-000000001-y.uid-000000002-z"));
}

TEST_F(UidIdentifierProcessorTest, expandMaintaintsUidState)
{
    const auto result = processor->expand("uid-x");
    const auto result1 = processor->expand("uid-y");
    const auto result2 = processor->expand("uid-z33");
    EXPECT_THAT(result, StrEq("uid-000000000-x"));
    EXPECT_THAT(result1, StrEq("uid-000000001-y"));
    EXPECT_THAT(result2, StrEq("uid-000000002-z33"));
}

TEST_F(UidIdentifierProcessorTest, expandWithUidAndNumber)
{
    const auto result = processor->expand("uid-01234-xyz12");
    EXPECT_THAT(result, StrEq("uid-000000000-xyz12"));
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

TEST_F(UidIdentifierProcessorTest, expandThrowsIfNineDigitsAreExceeded)
{
    UidIdentifierProcessorMock mock;
    mock.advanceCount(1'000'000'000);
    EXPECT_THROW(mock.expand("uid-00000-abc"), std::domain_error);
}

TEST_F(UidIdentifierProcessorTest, unexpandString)
{
    const auto result = processor->unexpand("abc");
    EXPECT_THAT(result, StrEq("abc"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithScope)
{
    const auto result = processor->unexpand("a.b.c");
    EXPECT_THAT(result, StrEq("a.b.c"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithScopeAndUid)
{
    const auto result = processor->unexpand("a.uid-000000000-b.uid-000000001-c");
    EXPECT_THAT(result, StrEq("a.uid-b.uid-c"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithUid)
{
    const auto result = processor->unexpand("uid-xyz");
    EXPECT_THAT(result, StrEq("uid-xyz"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithUidAndNumber)
{
    const auto result = processor->unexpand("uid-000000003-xyz");
    EXPECT_THAT(result, StrEq("uid-xyz"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithUidAndUidWithNumbers)
{
    const auto result = processor->unexpand("uid-000000000-x.uid-y.uid-000000002-z");
    EXPECT_THAT(result, StrEq("uid-x.uid-y.uid-z"));
}

TEST_F(UidIdentifierProcessorTest, unexpandWithDifferentUid)
{
    const auto result1 = processor->unexpand("uid-000000000-x");
    const auto result2 = processor->unexpand("uid-000000001-y");
    const auto result3 = processor->unexpand("uid-000000002-z");
    EXPECT_THAT(result1, StrEq("uid-x"));
    EXPECT_THAT(result2, StrEq("uid-y"));
    EXPECT_THAT(result3, StrEq("uid-z"));
}
