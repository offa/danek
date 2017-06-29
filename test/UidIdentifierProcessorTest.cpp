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
    StringBuffer str{"abc"};

    processor->expand(str);
    EXPECT_THAT(str.str(), StrEq("abc"));
}

TEST_F(UidIdentifierProcessorTest, expandWithScope)
{
    StringBuffer str{"a.b.c"};

    processor->expand(str);
    EXPECT_THAT(str.str(), StrEq("a.b.c"));
}

TEST_F(UidIdentifierProcessorTest, expandWithScopeAndUid)
{
    StringBuffer str{"a.uid-b.uid-0013-c"};

    processor->expand(str);
    EXPECT_THAT(str.str(), StrEq("a.uid-000000000-b.uid-000000001-c"));
}

TEST_F(UidIdentifierProcessorTest, expandWithUid)
{
    StringBuffer str{"uid-xyz"};

    processor->expand(str);
    EXPECT_THAT(str.str(), StrEq("uid-000000000-xyz"));
}

TEST_F(UidIdentifierProcessorTest, expandWithUidIncreasesUid)
{
    StringBuffer str{"uid-x.uid-y.uid-z"};

    processor->expand(str);
    EXPECT_THAT(str.str(), StrEq("uid-000000000-x.uid-000000001-y.uid-000000002-z"));
}

TEST_F(UidIdentifierProcessorTest, expandMaintaintsUidState)
{
    StringBuffer str{"uid-x"};
    StringBuffer str2{"uid-y"};
    StringBuffer str3{"uid-z33"};

    processor->expand(str);
    processor->expand(str2);
    processor->expand(str3);
    EXPECT_THAT(str.str(), StrEq("uid-000000000-x"));
    EXPECT_THAT(str2.str(), StrEq("uid-000000001-y"));
    EXPECT_THAT(str3.str(), StrEq("uid-000000002-z33"));
}

TEST_F(UidIdentifierProcessorTest, expandWithUidAndNumber)
{
    StringBuffer str{"uid-01234-xyz12"};

    processor->expand(str);
    EXPECT_THAT(str.str(), StrEq("uid-000000000-xyz12"));
}

TEST_F(UidIdentifierProcessorTest, expandWithWithouthUidSuffixThrows)
{
    StringBuffer str{"uid-"};
    StringBuffer str2{"uid--"};
    StringBuffer str3{"uid--abc"};
    StringBuffer str4{"uid-9abc"};
    StringBuffer str5{"uid-123-"};
    StringBuffer str6{"uid-123--a"};
    StringBuffer str7{"uid-123-456a"};
    StringBuffer str8{"uid-9"};

    EXPECT_THROW(processor->expand(str), ConfigurationException);
    EXPECT_THROW(processor->expand(str2), ConfigurationException);
    EXPECT_THROW(processor->expand(str3), ConfigurationException);
    EXPECT_THROW(processor->expand(str4), ConfigurationException);
    EXPECT_THROW(processor->expand(str5), ConfigurationException);
    EXPECT_THROW(processor->expand(str6), ConfigurationException);
    EXPECT_THROW(processor->expand(str7), ConfigurationException);
    EXPECT_THROW(processor->expand(str8), ConfigurationException);
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

