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

#pragma once

#include "danek/ConfigurationException.h"
#include "danek/StringBuffer.h"
#include "danek/internal/FunctionType.h"

namespace danek
{
    class LexToken
    {
    public:

        LexToken();
        LexToken(short type, int lineNum, const char* spelling);

        const char* spelling() const;
        int lineNum() const;
        short type() const;
        const char* typeAsString();

        bool isStringFunc() const;
        bool isListFunc() const;
        bool isBoolFunc() const;

        void reset(short type, int lineNum, const char* spelling);
        void reset(short type, int lineNum, const char* spelling, FunctionType funcType);

        void resetWithOwnership(short type, int lineNum, StringBuffer& str);

    protected:
        short m_type;   //  LexBaseSymbols
        StringBuffer m_spelling;
        int m_lineNum;
        FunctionType m_funcType;
    };
}
