// Copyright (c) 2017-2021 offa
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

#include "danek/internal/ConfigItem.h"
#include "danek/internal/ConfigScope.h"
#include <gmock/gmock.h>

using namespace danek;
using namespace testing;

class ConfigItemTest : public testing::Test
{
};

TEST_F(ConfigItemTest, stringItem)
{
    const ConfigItem item{"name_s", "value"};
    EXPECT_EQ(ConfType::String, item.type());
    EXPECT_THAT(item.name(), StrEq("name_s"));
    EXPECT_THAT(item.stringVal(), StrEq("value"));
}

TEST_F(ConfigItemTest, stringItemThrowsOnInvalidTypeAccess)
{
    const ConfigItem item{"bad", std::vector<std::string>{}};
    EXPECT_THROW(item.stringVal(), std::domain_error);
}

TEST_F(ConfigItemTest, stringListItem)
{
    const std::vector<std::string> v{"a1", "a2"};
    ConfigItem item{"name_l", v};

    EXPECT_EQ(ConfType::List, item.type());
    EXPECT_THAT(item.name(), StrEq("name_l"));
    const auto& ref = item.listVal();
    EXPECT_THAT(ref[0], StrEq("a1"));
    EXPECT_THAT(ref[1], StrEq("a2"));
}

TEST_F(ConfigItemTest, stringListItemThrowsOnInvalidTypeAccess)
{
    const ConfigItem item{"bad", "value"};
    EXPECT_THROW(item.listVal(), std::domain_error);
}

TEST_F(ConfigItemTest, scopeItem)
{
    const char c = '\0';
    const ConfigItem item{"name_cs", std::make_unique<ConfigScope>(nullptr, &c)};
    EXPECT_EQ(ConfType::Scope, item.type());
    EXPECT_THAT(item.name(), StrEq("name_cs"));
    EXPECT_THAT(item.scopeVal(), Not(nullptr));
}

TEST_F(ConfigItemTest, scopeItemThrowsOnInvalidTypeAccess)
{
    const ConfigItem item{"bad", "value"};
    EXPECT_THROW(item.scopeVal(), std::domain_error);
}
