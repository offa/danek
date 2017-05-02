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

//--------
// #include's
//--------

#include "danek/internal/LexToken.h"
#include "danek/internal/LexBase.h"
#include <assert.h>

namespace danek
{
    //----------------------------------------------------------------------
    // Function:	Constructor
    //
    // Description:
    //----------------------------------------------------------------------

    LexToken::LexToken() : m_type(LexBase::LEX_UNKNOWN_SYM), m_lineNum(-1), m_funcType(FunctionType::None)
    {
    }

    LexToken::LexToken(const LexToken& other)
        : m_type(other.m_type),
          m_spelling(other.m_spelling),
          m_lineNum(other.m_lineNum),
          m_funcType(other.m_funcType)
    {
    }

    //----------------------------------------------------------------------
    // Function:	Constructor
    //
    // Description:
    //----------------------------------------------------------------------

    LexToken::LexToken(short type, int lineNum, const char* spelling)
        : m_type(type), m_spelling(spelling), m_lineNum(lineNum), m_funcType(FunctionType::None)
    {
    }

    //----------------------------------------------------------------------
    // Function:	Destructor
    //
    // Description:
    //----------------------------------------------------------------------

    LexToken::~LexToken()
    {
    }

    //----------------------------------------------------------------------
    // Function:	operator=
    //
    // Description:
    //----------------------------------------------------------------------

    LexToken& LexToken::operator=(const LexToken& other)
    {
        if (this != &other)
        {
            this->m_type = other.m_type;
            this->m_lineNum = other.m_lineNum;
            m_spelling = other.m_spelling;
            m_funcType = other.m_funcType;
        }
        return *this;
    }

    //----------------------------------------------------------------------
    // Function:	reset()
    //
    // Description:	Modifier function
    //----------------------------------------------------------------------

    void LexToken::reset(short type, int lineNum, const char* spelling)
    {
        m_type = type;
        m_lineNum = lineNum;
        m_spelling = spelling;
        m_funcType = FunctionType::None;
    }

    //----------------------------------------------------------------------
    // Function:	reset()
    //
    // Description:	Modifier function
    //----------------------------------------------------------------------

    void LexToken::reset(short type, int lineNum, const char* spelling, FunctionType funcType)
    {
        m_type = type;
        m_lineNum = lineNum;
        m_spelling = spelling;
        m_funcType = funcType;
    }

    //----------------------------------------------------------------------
    // Function:	reset()
    //
    // Description:	Modifier function
    //----------------------------------------------------------------------

    void LexToken::resetWithOwnership(short type, int lineNum, StringBuffer& str)
    {
        m_type = type;
        m_lineNum = lineNum;
        m_funcType = FunctionType::None;
        m_spelling.takeOwnershipOfStringIn(str);
    }

    bool LexToken::isStringFunc()
    {
        return m_funcType == FunctionType::String;
    }
    bool LexToken::isListFunc()
    {
        return m_funcType == FunctionType::List;
    }
    bool LexToken::isBoolFunc()
    {
        return m_funcType == FunctionType::Bool;
    }
}
