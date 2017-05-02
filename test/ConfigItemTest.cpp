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

#include "danek/internal/ConfigItem.h"
#include "danek/internal/ConfigScope.h"
#include <gmock/gmock.h>

using namespace danek;
using namespace testing;

class ConfigItemTest : public testing::Test
{
protected:

    using EntryType = Configuration::Type;
};

TEST_F(ConfigItemTest, stringItem)
{
    ConfigItem item{"name_s", "value"};
    EXPECT_EQ(EntryType::String, item.type());
    EXPECT_THAT(item.name(), StrEq("name_s"));
    EXPECT_THAT(item.stringVal(), StrEq("value"));
}

TEST_F(ConfigItemTest, stringListItem)
{
    StringVector v;
    v.push_back("aa");
    v.push_back("bb");

    ConfigItem item{"name_l", v};
    EXPECT_EQ(EntryType::List, item.type());
    EXPECT_THAT(item.name(), StrEq("name_l"));
    const auto& ref = item.listVal();
    EXPECT_THAT(ref[0], StrEq("aa"));
    EXPECT_THAT(ref[1], StrEq("bb"));
}

TEST_F(ConfigItemTest, stringListItemFromCArray)
{
    std::vector<const char*> v{"a", "b"};

    ConfigItem item{"name_c", v.data(), v.size()};
    EXPECT_EQ(EntryType::List, item.type());
    EXPECT_THAT(item.name(), StrEq("name_c"));
    const auto& ref = item.listVal();
    EXPECT_THAT(ref[0], StrEq("a"));
    EXPECT_THAT(ref[1], StrEq("b"));
}

TEST_F(ConfigItemTest, scopeItem)
{
    const char c = '\0';
    auto nonOwningPtr = new ConfigScope(nullptr, &c);
    ConfigItem item{"name_cs", nonOwningPtr};
    EXPECT_EQ(EntryType::Scope, item.type());
    EXPECT_THAT(item.name(), StrEq("name_cs"));
    EXPECT_THAT(item.scopeVal(), Eq(nonOwningPtr));
}

