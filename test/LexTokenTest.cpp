// Copyright (c) 2017-2019 offa
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


#include "danek/internal/LexToken.h"
#include "danek/internal/LexBaseSymbols.h"
#include <gmock/gmock.h>

using namespace danek;
using namespace testing;

class LexTokenTest : public testing::Test
{
};

TEST_F(LexTokenTest, initWithDefaultValues)
{
    const LexToken t;
    EXPECT_THAT(t.type(), Eq(lex::LEX_UNKNOWN_SYM));
    EXPECT_THAT(t.lineNum(), Eq(-1));
    EXPECT_THAT(t.spelling(), StrEq(""));
    EXPECT_FALSE(t.isStringFunc());
    EXPECT_FALSE(t.isListFunc());
    EXPECT_FALSE(t.isBoolFunc());
}

TEST_F(LexTokenTest, initWithValues)
{
    const LexToken t{lex::LEX_PLUS_SYM, 15, "abc"};
    EXPECT_THAT(t.type(), Eq(lex::LEX_PLUS_SYM));
    EXPECT_THAT(t.lineNum(), Eq(15));
    EXPECT_THAT(t.spelling(), StrEq("abc"));
    EXPECT_FALSE(t.isStringFunc());
    EXPECT_FALSE(t.isListFunc());
    EXPECT_FALSE(t.isBoolFunc());
}

TEST_F(LexTokenTest, stringFunction)
{
    LexToken t;
    t.reset(lex::LEX_PLUS_SYM, 0, "", FunctionType::String);
    EXPECT_TRUE(t.isStringFunc());
    EXPECT_FALSE(t.isListFunc());
    EXPECT_FALSE(t.isBoolFunc());
}

TEST_F(LexTokenTest, listFunction)
{
    LexToken t;
    t.reset(lex::LEX_PLUS_SYM, 0, "", FunctionType::List);
    EXPECT_FALSE(t.isStringFunc());
    EXPECT_TRUE(t.isListFunc());
    EXPECT_FALSE(t.isBoolFunc());
}

TEST_F(LexTokenTest, boolFunction)
{
    LexToken t;
    t.reset(lex::LEX_PLUS_SYM, 0, "", FunctionType::Bool);
    EXPECT_FALSE(t.isStringFunc());
    EXPECT_FALSE(t.isListFunc());
    EXPECT_TRUE(t.isBoolFunc());
}

TEST_F(LexTokenTest, noneFunctionType)
{
    LexToken t;
    t.reset(lex::LEX_PLUS_SYM, 0, "", FunctionType::None);
    EXPECT_FALSE(t.isStringFunc());
    EXPECT_FALSE(t.isListFunc());
    EXPECT_FALSE(t.isBoolFunc());
}

TEST_F(LexTokenTest, reset)
{
    LexToken t{lex::LEX_PLUS_SYM, 15, "abc"};
    t.reset(lex::LEX_UNKNOWN_SYM, 9, "aaa", FunctionType::None);
    EXPECT_THAT(t.type(), Eq(lex::LEX_UNKNOWN_SYM));
    EXPECT_THAT(t.lineNum(), Eq(9));
    EXPECT_THAT(t.spelling(), StrEq("aaa"));
    EXPECT_FALSE(t.isStringFunc());
    EXPECT_FALSE(t.isListFunc());
    EXPECT_FALSE(t.isBoolFunc());
}

TEST_F(LexTokenTest, resetWithFunctionType)
{
    LexToken t{lex::LEX_PLUS_SYM, 15, "abc"};
    t.reset(lex::LEX_UNKNOWN_SYM, 8, "xyz", FunctionType::Bool);
    EXPECT_THAT(t.type(), Eq(lex::LEX_UNKNOWN_SYM));
    EXPECT_THAT(t.lineNum(), Eq(8));
    EXPECT_THAT(t.spelling(), StrEq("xyz"));
    EXPECT_TRUE(t.isBoolFunc());
}
