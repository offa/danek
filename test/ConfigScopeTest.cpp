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

#include "danek/internal/ConfigScope.h"
#include "danek/internal/ConfigItem.h"
#include <gmock/gmock.h>

using namespace danek;
using namespace testing;

class ConfigScopeTest : public testing::Test
{
};

TEST_F(ConfigScopeTest, initRootElement)
{
    ConfigScope s{nullptr, "\0"};
    EXPECT_THAT(s.scopedName(), StrEq(""));
    EXPECT_THAT(s.parentScope(), Eq(nullptr));
}

TEST_F(ConfigScopeTest, initRootElementThrowsIfNameIsPassed)
{
    EXPECT_THROW(ConfigScope(nullptr, "invalid"), std::invalid_argument);
}

TEST_F(ConfigScopeTest, initNodeElement)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope node{&root, "abc"};
    EXPECT_THAT(node.scopedName(), StrEq("abc"));
    EXPECT_THAT(node.parentScope(), Eq(&root));
}

TEST_F(ConfigScopeTest, initNodeElementAddsScopeOfParentElement)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope parent{&root, "xyz"};
    ConfigScope node{&parent, "abc"};
    EXPECT_THAT(node.scopedName(), StrEq("xyz.abc"));
    EXPECT_THAT(node.parentScope(), Eq(&parent));
}

TEST_F(ConfigScopeTest, rootScope)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope parent{&root, "xyz"};
    ConfigScope node{&parent, "abc"};
    ConfigScope node2{&parent, "def"};
    ConfigScope node3{&node2, "ghi"};
    EXPECT_THAT(parent.rootScope(), Eq(&root));
    EXPECT_THAT(node.rootScope(), Eq(&root));
    EXPECT_THAT(node2.rootScope(), Eq(&root));
    EXPECT_THAT(node3.rootScope(), Eq(&root));
}

TEST_F(ConfigScopeTest, rootScopeOnRootElement)
{
    ConfigScope root{nullptr, "\0"};
    EXPECT_THAT(root.rootScope(), Eq(&root));
}

TEST_F(ConfigScopeTest, findItemIfEmpty)
{
    ConfigScope root{nullptr, "\0"};
    const auto item = root.findItem("x");
    EXPECT_THAT(item, Eq(nullptr));
}

TEST_F(ConfigScopeTest, findItemIfFound)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "123");
    root.addOrReplaceString("n2", "456");

    const auto item = root.findItem("n2");
    EXPECT_THAT(item->name(), StrEq("n2"));
    EXPECT_THAT(item->stringVal(), StrEq("456"));
    EXPECT_THAT(item->type(), Eq(ConfType::String));
}

TEST_F(ConfigScopeTest, findItemIfNotFound)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "123");
    root.addOrReplaceString("n2", "456");

    const auto item = root.findItem("not-found");
    EXPECT_THAT(item, Eq(nullptr));
}

TEST_F(ConfigScopeTest, addStringItemAddsNewElement)
{
    ConfigScope root{nullptr, "\0"};
    const auto result = root.addOrReplaceString("e1", "123");

    EXPECT_TRUE(result);
    const auto found = root.findItem("e1");
    EXPECT_THAT(found->name(), StrEq("e1"));
    EXPECT_THAT(found->stringVal(), StrEq("123"));
    EXPECT_THAT(found->type(), Eq(ConfType::String));
}

TEST_F(ConfigScopeTest, addStringItemReplacesExistingItemOfSameName)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("e", "123");
    const auto result = root.addOrReplaceString("e", "new-value");

    EXPECT_TRUE(result);
    const auto found = root.findItem("e");
    EXPECT_THAT(found->name(), StrEq("e"));
    EXPECT_THAT(found->stringVal(), StrEq("new-value"));
    EXPECT_THAT(found->type(), Eq(ConfType::String));
}

TEST_F(ConfigScopeTest, addStringItemDoesNothingIfExistingScopeOfSameName)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope scope{&root, "xy"};
    ConfigScope* ptr = &scope;
    root.ensureScopeExists("scope-name", ptr);

    const auto result = root.addOrReplaceString("scope-name", "value");
    EXPECT_FALSE(result);
}

TEST_F(ConfigScopeTest, addListItemAddsNewElement)
{
    const std::vector<std::string> v = {"a", "b"};
    ConfigScope root{nullptr, "\0"};
    const auto result = root.addOrReplaceList("el", StringVector{v});

    EXPECT_TRUE(result);
    const auto found = root.findItem("el");
    EXPECT_THAT(found->name(), StrEq("el"));
    EXPECT_THAT(found->listVal(), ElementsAre("a", "b"));
    EXPECT_THAT(found->type(), Eq(ConfType::List));
}

TEST_F(ConfigScopeTest, addListItemReplacesExistingItemOfSameName)
{
    const std::vector<std::string> old = {"1", "2"};
    const std::vector<std::string> v = {"4", "5"};
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceList("el", StringVector{old});
    const auto result = root.addOrReplaceList("el", StringVector{v});

    EXPECT_TRUE(result);
    const auto found = root.findItem("el");
    EXPECT_THAT(found->name(), StrEq("el"));
    EXPECT_THAT(found->listVal(), ElementsAre("4", "5"));
    EXPECT_THAT(found->type(), Eq(ConfType::List));
}

TEST_F(ConfigScopeTest, addListItemDoesNothingIfExistingScopeOfSameName)
{
    const std::vector<std::string> v = {"4", "5"};
    ConfigScope root{nullptr, "\0"};
    ConfigScope scope{&root, "xy"};
    ConfigScope* ptr = &scope;
    root.ensureScopeExists("scope-name", ptr);

    const auto result = root.addOrReplaceList("scope-name", StringVector{v});
    EXPECT_FALSE(result);
}

TEST_F(ConfigScopeTest, addItemReplacesElementOfSameName)
{
    const std::vector<std::string> v = {"4", "5"};
    ConfigScope root{nullptr, "\0"};

    EXPECT_TRUE(root.addOrReplaceList("name", StringVector{v}));
    EXPECT_TRUE(root.addOrReplaceString("name", "abc"));

    const auto found = root.findItem("name");
    EXPECT_THAT(found->name(), StrEq("name"));
    EXPECT_THAT(found->stringVal(), StrEq("abc"));
    EXPECT_THAT(found->type(), Eq(ConfType::String));
}

TEST_F(ConfigScopeTest, addScopeAddsNewScopeElement)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope scope{&root, "xy"};
    ConfigScope* ptr = &scope;

    const auto result = root.ensureScopeExists("scope-name", ptr);
    EXPECT_TRUE(result);
    const auto found = root.findItem("scope-name");
    EXPECT_THAT(found->name(), StrEq("scope-name"));
    EXPECT_THAT(found->type(), Eq(ConfType::Scope));
}

TEST_F(ConfigScopeTest, addScopeDoesNothingIfExisting)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope scope{&root, "xy"};
    ConfigScope* ptr = &scope;

    root.ensureScopeExists("scope-name", ptr);
    const auto result = root.ensureScopeExists("scope-name", ptr);
    EXPECT_TRUE(result);
    const auto found = root.findItem("scope-name");
    EXPECT_THAT(found->name(), StrEq("scope-name"));
    EXPECT_THAT(found->type(), Eq(ConfType::Scope));
}

TEST_F(ConfigScopeTest, addScopeDoesNothingIfExistingElementOfSameName)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope scope{&root, "xy"};
    ConfigScope* ptr = &scope;

    root.addOrReplaceString("item-name", "123");
    root.ensureScopeExists("item-name", ptr);
    const auto result = root.ensureScopeExists("item-name", ptr);
    EXPECT_FALSE(result);
    const auto found = root.findItem("item-name");
    EXPECT_THAT(found->name(), StrEq("item-name"));
    EXPECT_THAT(found->type(), Eq(ConfType::String));
}

TEST_F(ConfigScopeTest, removeItemRemovesItem)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "123");
    root.addOrReplaceString("n2", "456");
    root.addOrReplaceString("n3", "789");
    const auto result = root.removeItem("n2");

    EXPECT_TRUE(result);
    EXPECT_FALSE(root.contains("n2"));
}

TEST_F(ConfigScopeTest, removeItemDoesNothingIfNotFound)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "123");
    root.addOrReplaceString("n2", "456");
    const auto result = root.removeItem("n3");

    EXPECT_FALSE(result);
    EXPECT_TRUE(root.contains("n1"));
    EXPECT_TRUE(root.contains("n2"));
}

TEST_F(ConfigScopeTest, containsIfFound)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "123");
    root.addOrReplaceString("n2", "456");

    const auto result = root.contains("n2");
    EXPECT_TRUE(result);
}

TEST_F(ConfigScopeTest, containsIfNotFound)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "123");
    root.addOrReplaceString("n2", "456");

    const auto result = root.contains("n3");
    EXPECT_FALSE(result);
}

TEST_F(ConfigScopeTest, listFullyScopedNames)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "1");
    root.addOrReplaceString("n2", "2");

    const auto v = root.listFullyScopedNames(ConfType::String, false);
    EXPECT_THAT(v, UnorderedElementsAre("n1", "n2"));
}

TEST_F(ConfigScopeTest, listFullyScopedNamesWithFilter)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "1");
    root.addOrReplaceString("n2", "2");

    std::vector<std::string> filter{"n2"};
    const auto v = root.listFullyScopedNames(ConfType::String, false, filter);
    EXPECT_THAT(v, UnorderedElementsAre("n2"));
}

TEST_F(ConfigScopeTest, listFullyScopedNamesOfMixedType)
{
    const std::vector<std::string> elements = {"a", "b"};
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "1");
    root.addOrReplaceString("n2", "2");
    root.addOrReplaceList("n3", StringVector{elements});

    const auto v = root.listFullyScopedNames(ConfType::Variables, false);
    EXPECT_THAT(v, UnorderedElementsAre("n1", "n2", "n3"));
}

TEST_F(ConfigScopeTest, listFullyScopedNamesFiltersElements)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "1");
    root.addOrReplaceString("n2", "2");

    const auto v = root.listFullyScopedNames(ConfType::List, false);
    EXPECT_THAT(v, IsEmpty());
}

TEST_F(ConfigScopeTest, listFullyScopedNamesWithScope)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope scope0{&root, "s0"};
    ConfigScope* ptr0 = &scope0;
    ConfigScope scope1{&root, "s1"};
    ConfigScope* ptr1 = &scope1;

    root.ensureScopeExists("sn-0", ptr0);
    root.ensureScopeExists("sn-1", ptr1);

    const auto v = root.listFullyScopedNames(ConfType::ScopesAndVars, false);
    EXPECT_THAT(v, UnorderedElementsAre("sn-0", "sn-1"));
}

TEST_F(ConfigScopeTest, listFullyScopedNamesOfMultipleNodes)
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

    const auto v = ptr1->listFullyScopedNames(ConfType::ScopesAndVars, false);
    EXPECT_THAT(v, UnorderedElementsAre("sn0.sn1.x"));
}

TEST_F(ConfigScopeTest, listFullyScopedNamesOfMultipleNodesRecursive)
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

    const auto v = ptr0->listFullyScopedNames(ConfType::ScopesAndVars, true);
    EXPECT_THAT(v, UnorderedElementsAre("sn0.a", "sn0.sn1", "sn0.sn1.x"));
}

TEST_F(ConfigScopeTest, listFullyScopedNamesOfMultipleNodesRecursiveScope)
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

    const auto v = ptr0->listFullyScopedNames(ConfType::Scope, true);
    EXPECT_THAT(v, UnorderedElementsAre("sn0.sn1"));
}

TEST_F(ConfigScopeTest, listFullyScopedNamesOfMultipleNodesRecursiveWithFilter)
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

    std::vector<std::string> filter{"sn0.sn1*"};
    const auto v = ptr0->listFullyScopedNames(ConfType::ScopesAndVars, true, filter);
    EXPECT_THAT(v, UnorderedElementsAre("sn0.sn1", "sn0.sn1.x"));
}

TEST_F(ConfigScopeTest, listLocallyScopedNames)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "1");
    root.addOrReplaceString("n2", "2");

    std::vector<std::string> filter{};
    const auto v = root.listLocallyScopedNames(ConfType::String, false, filter);
    EXPECT_THAT(v, UnorderedElementsAre("n1", "n2"));
}

TEST_F(ConfigScopeTest, listLocallyScopedNamesWithFilter)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "1");
    root.addOrReplaceString("n2", "2");

    std::vector<std::string> filter{"n2"};
    const auto v = root.listLocallyScopedNames(ConfType::String, false, filter);
    EXPECT_THAT(v, UnorderedElementsAre("n2"));
}

TEST_F(ConfigScopeTest, listLocallyScopedNamesOfMixedType)
{
    const std::vector<std::string> elements = {"a", "b"};
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "1");
    root.addOrReplaceString("n2", "2");
    root.addOrReplaceList("n3", StringVector{elements});

    std::vector<std::string> filter{"n*"};
    const auto v = root.listLocallyScopedNames(ConfType::Variables, false, filter);
    EXPECT_THAT(v, UnorderedElementsAre("n1", "n2", "n3"));
}

TEST_F(ConfigScopeTest, listLocallyScopedNamesFiltersElements)
{
    ConfigScope root{nullptr, "\0"};
    root.addOrReplaceString("n1", "1");
    root.addOrReplaceString("n2", "2");

    std::vector<std::string> filter{"n*"};
    const auto v = root.listLocallyScopedNames(ConfType::List, false, filter);
    EXPECT_THAT(v, IsEmpty());
}

TEST_F(ConfigScopeTest, listLocallyScopedNamesWithScope)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope scope0{&root, "s0"};
    ConfigScope* ptr0 = &scope0;
    ConfigScope scope1{&root, "s1"};
    ConfigScope* ptr1 = &scope1;

    root.ensureScopeExists("sn-0", ptr0);
    root.ensureScopeExists("sn-1", ptr1);

    std::vector<std::string> filter{"*"};
    const auto v = root.listLocallyScopedNames(ConfType::ScopesAndVars, false, filter);

    EXPECT_THAT(v, UnorderedElementsAre("sn-0", "sn-1"));
}

TEST_F(ConfigScopeTest, listLocallyScopedNamesOfMultipleNodes)
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

    std::vector<std::string> filter{"*"};
    const auto v = ptr1->listLocallyScopedNames(ConfType::ScopesAndVars, false, filter);

    EXPECT_THAT(v, UnorderedElementsAre("x"));
}

TEST_F(ConfigScopeTest, listLocallyScopedNamesOfMultipleNodesRecursive)
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

    std::vector<std::string> filter{"*"};
    const auto v = ptr0->listLocallyScopedNames(ConfType::ScopesAndVars, true, filter);

    EXPECT_THAT(v, UnorderedElementsAre("a", "sn1", "sn1.x"));
}

TEST_F(ConfigScopeTest, listLocallyScopedNamesOfMultipleNodesRecursiveScope)
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

    std::vector<std::string> filter{"*"};
    const auto v = ptr0->listLocallyScopedNames(ConfType::Scope, true, filter);

    EXPECT_THAT(v, UnorderedElementsAre("sn1"));
}

TEST_F(ConfigScopeTest, dumpRoot)
{
    ConfigScope root{nullptr, "\0"};
    StringBuffer buffer;
    root.dump(buffer, true);
    EXPECT_THAT(buffer.str(), StrEq(""));
}

TEST_F(ConfigScopeTest, dumpStringNode)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope node{&root, "abc"};
    node.addOrReplaceString("x", "y");
    node.addOrReplaceString("1", "2");
    StringBuffer buffer;
    node.dump(buffer, true);
    EXPECT_THAT(buffer.str(), StrEq("1 = \"2\";\nx = \"y\";\n"));
}

TEST_F(ConfigScopeTest, dumpStringNodeWithIndention)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope node{&root, "abc"};
    node.addOrReplaceString("x", "y");
    node.addOrReplaceString("1", "2");
    StringBuffer buffer;
    node.dump(buffer, true, 1);
    EXPECT_THAT(buffer.str(), StrEq("    1 = \"2\";\n    x = \"y\";\n"));
}

TEST_F(ConfigScopeTest, dumpScopeNode)
{
    ConfigScope root{nullptr, "\0"};
    ConfigScope scope{&root, "xy"};
    ConfigScope* ptr = &scope;

    root.ensureScopeExists("scope-name", ptr);
    StringBuffer buffer;
    root.dump(buffer, true);
    EXPECT_THAT(buffer.str(), StrEq("scope-name {\n}\n"));
}

TEST_F(ConfigScopeTest, dumpMixedType)
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

    StringBuffer buffer;
    root.dump(buffer, true);
    EXPECT_THAT(buffer.str(), StrEq("sn0 {\n    a = \"b\";\n    sn1 {\n            x = \"y\";\n}\n}\n"));
}

