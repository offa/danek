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

#include "danek/internal/ToString.h"
#include "danek/internal/ConfigItem.h"
#include "danek/internal/ConfigScope.h"
#include <gmock/gmock.h>

using namespace danek;
using namespace testing;

class ToStringTest : public testing::Test
{
};

TEST_F(ToStringTest, configItemEscapesSpecialCharacters)
{
    const ConfigItem item{"name", "value\t_\n_%_\"_"};
    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz = \"value%t_%n_%%_%\"_\";\n"));
}

TEST_F(ToStringTest, configItemWithIndention)
{
    const ConfigItem item{"name", "value"};
    const auto str = toString(item, "xyz", false, 2);
    EXPECT_THAT(str, StrEq("        xyz = \"value\";\n"));
}

TEST_F(ToStringTest, configItemStringItem)
{
    const ConfigItem item{"name", "value"};
    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz = \"value\";\n"));
}

TEST_F(ToStringTest, configItemListItem)
{
    const std::vector<std::string> v{"a", "b\n"};
    const ConfigItem item{"name", v};

    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz = [\"a\", \"b%n\"];\n"));
}

TEST_F(ToStringTest, configItemListItemSingleElement)
{
    const std::vector<std::string> v{};
    const ConfigItem item{"name", v};

    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz = [];\n"));
}

TEST_F(ToStringTest, configItemListItemIfEmpty)
{
    const std::vector<std::string> v{};
    const ConfigItem item{"name", v};

    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz = [];\n"));
}

TEST_F(ToStringTest, configItemScopeItem)
{
    const char c = '\0';
    const ConfigItem item{"name", std::make_unique<ConfigScope>(nullptr, &c)};
    const auto str = toString(item, "xyz", false);
    EXPECT_THAT(str, StrEq("xyz {\n}\n"));
}

TEST_F(ToStringTest, configScopeRoot)
{
    ConfigScope root{nullptr, "\0"};
    StringBuffer buffer;
    const auto str = toString(root, true);
    EXPECT_THAT(str, StrEq(""));
}

TEST_F(ToStringTest, configScopeStringNode)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope node{&root, "abc"};
    node.addOrReplaceString("x", "y");
    node.addOrReplaceString("1", "2");

    const auto str = toString(node, true);
    EXPECT_THAT(str, StrEq("1 = \"2\";\nx = \"y\";\n"));
}

TEST_F(ToStringTest, configScopeStringNodeWithIndention)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope node{&root, "abc"};
    node.addOrReplaceString("x", "y");
    node.addOrReplaceString("1", "2");

    const auto str = toString(node, true, 1);
    EXPECT_THAT(str, StrEq("    1 = \"2\";\n    x = \"y\";\n"));
}

TEST_F(ToStringTest, configScopeScopeNode)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope scope{&root, "xy"};
    ConfigScope* ptr = &scope;
    root.ensureScopeExists("scope-name", ptr);

    const auto str = toString(root, true);
    EXPECT_THAT(str, StrEq("scope-name {\n}\n"));
}

TEST_F(ToStringTest, configScopeMixedType)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope scope0{&root, "s0"};
    ConfigScope* ptr0 = &scope0;
    ConfigScope scope1{ptr0, "s1"};
    ConfigScope* ptr1 = &scope1;

    root.ensureScopeExists("sn0", ptr0);
    ptr0->addOrReplaceString("a", "b");
    ptr0->ensureScopeExists("sn1", ptr1);
    ptr1->addOrReplaceString("x", "y");

    const auto str = toString(root, true);
    EXPECT_THAT(str, StrEq("sn0 {\n    a = \"b\";\n    sn1 {\n        x = \"y\";\n    }\n}\n"));
}
