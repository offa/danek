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

#include "danek/internal/SchemaIdRuleInfo.h"
#include "danek/internal/SchemaIgnoreRuleInfo.h"
#include <gmock/gmock.h>

using danek::SchemaIdRuleInfo;
using danek::SchemaIgnoreRuleInfo;
using namespace testing;

class SchemaIdRuleInfoTest : public testing::Test
{
};

TEST(SchemaIdRuleInfoTest, idRuleInfoGetter)
{
    const SchemaIdRuleInfo info{};
    EXPECT_THAT(info.locallyScopedName(), StrEq(""));
    EXPECT_THAT(info.typeName(), StrEq(""));
    EXPECT_THAT(info.args(), IsEmpty());
    EXPECT_FALSE(info.isOptional());
}

TEST(SchemaIdRuleInfoTest, idRuleInfoSetter)
{
    SchemaIdRuleInfo info{};
    info.setLocallyScopedName("abc");
    info.setTypeName("123");
    info.addArg("xyz");
    info.setIsOptional(true);

    EXPECT_THAT(info.locallyScopedName(), StrEq("abc"));
    EXPECT_THAT(info.typeName(), StrEq("123"));
    EXPECT_THAT(info.args(), ElementsAre("xyz"));
    EXPECT_TRUE(info.isOptional());
}

TEST(SchemaIdRuleInfoTest, ignoreRuleInfoGetter)
{
    const SchemaIgnoreRuleInfo info{};
    EXPECT_THAT(info.symbol(), Eq(0));
    EXPECT_THAT(info.locallyScopedName(), StrEq(""));
}

TEST(SchemaIdRuleInfoTest, ignoreRuleInfoSetter)
{
    SchemaIgnoreRuleInfo info{};
    info.setLocallyScopedName("abc");
    info.setSymbol(9);

    EXPECT_THAT(info.symbol(), Eq(9));
    EXPECT_THAT(info.locallyScopedName(), StrEq("abc"));
}

