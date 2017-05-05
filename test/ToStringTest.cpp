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

#include "danek/internal/ToString.h"
#include "danek/internal/ConfigItem.h"
#include "danek/internal/ConfigScope.h"
#include <gmock/gmock.h>

using namespace danek;
using namespace testing;

class ToStringTest : public testing::Test
{
};

TEST_F(ToStringTest, toStringEscapesSpecialCharacters)
{
    ConfigItem item{"name", "value\t_\n_%_\"_"};
    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz = \"value%t_%n_%%_%\"_\";\n"));
}

TEST_F(ToStringTest, toStringWithIndention)
{
    ConfigItem item{"name", "value"};
    const auto str = toString(item, "xyz", false, 2);
    EXPECT_THAT(str, StrEq("        xyz = \"value\";\n"));
}

TEST_F(ToStringTest, toStringStringItem)
{
    ConfigItem item{"name", "value"};
    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz = \"value\";\n"));
}

TEST_F(ToStringTest, toStringListItem)
{
    const std::vector<std::string> v{"a", "b\n"};
    ConfigItem item{"name", v};

    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz = [\"a\", \"b%n\"];\n"));
}

TEST_F(ToStringTest, toStringListItemSingleElement)
{
    const std::vector<std::string> v{};
    ConfigItem item{"name", v};

    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz = [];\n"));
}

TEST_F(ToStringTest, toStringListItemIfEmpty)
{
    const std::vector<std::string> v{};
    ConfigItem item{"name", v};

    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz = [];\n"));
}

TEST_F(ToStringTest, toStringScopeItem)
{
    const char c = '\0';
    ConfigItem item{"name", std::make_unique<ConfigScope>(nullptr, &c)};
    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz {\n}\n"));
}

