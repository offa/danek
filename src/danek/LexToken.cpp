// Copyright (c) 2017 offa
// Copyright 2011 Ciaran McHale.
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
#include "danek/internal/LexBase.h"
#include <assert.h>

namespace danek
{
    LexToken::LexToken() : LexToken(LexBase::LEX_UNKNOWN_SYM, -1, "")
    {
    }

    LexToken::LexToken(short type, std::int32_t lineNum, const std::string& spelling) : m_type(type),
                                                                                    m_spelling(spelling),
                                                                                    m_lineNum(lineNum),
                                                                                    m_funcType(FunctionType::None)
    {
    }

    void LexToken::reset(short type, std::int32_t lineNum, const std::string& spelling)
    {
        reset(type, lineNum, spelling, FunctionType::None);
    }

    void LexToken::reset(short type, std::int32_t lineNum, const std::string& spelling, FunctionType funcType)
    {
        m_type = type;
        m_lineNum = lineNum;
        m_spelling = spelling;
        m_funcType = funcType;
    }

    const std::string& LexToken::spelling() const
    {
        return m_spelling;
    }

    std::int32_t LexToken::lineNum() const
    {
        return m_lineNum;
    }

    short LexToken::type() const
    {
        return m_type;
    }

    bool LexToken::isStringFunc() const
    {
        return m_funcType == FunctionType::String;
    }

    bool LexToken::isListFunc() const
    {
        return m_funcType == FunctionType::List;
    }

    bool LexToken::isBoolFunc() const
    {
        return m_funcType == FunctionType::Bool;
    }
}
