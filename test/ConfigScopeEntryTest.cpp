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

#include "danek/internal/ConfigScopeEntry.h"
#include "danek/internal/ConfigScope.h"
#include "danek/internal/ConfigItem.h"
#include <gmock/gmock.h>

using namespace danek;
using namespace testing;

class ConfigScopeEntryTest : public testing::Test
{
};

TEST_F(ConfigScopeEntryTest, values)
{
    ConfigItem* item = new ConfigItem("item-name", "value");
    ConfigScopeEntry entry{item, nullptr};
    EXPECT_THAT(entry.name(), StrEq("item-name"));
    EXPECT_THAT(entry.item(), Eq(item));
    EXPECT_THAT(entry.type(), Eq(item->type()));
}

TEST_F(ConfigScopeEntryTest, changeItem)
{
    ConfigItem* itemOld = new ConfigItem("name", "value");
    ConfigItem* itemNew = new ConfigItem("new-name", "new-value");
    ConfigScopeEntry entry{itemOld, nullptr};
    entry.setItem(itemNew);
    EXPECT_THAT(entry.name(), StrEq("new-name"));
    EXPECT_THAT(entry.item(), Eq(itemNew));
    EXPECT_THAT(entry.type(), Eq(itemNew->type()));
}

