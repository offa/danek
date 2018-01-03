// Copyright (c) 2017-2018 offa
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

namespace danek
{
    namespace lex
    {

        //--------
        // Constants for lexical symbols, for everything except
        // keywords and function names. Constants for those are
        // defined in a subclass.
        //--------
        enum LexBaseSymbols
        {
            LEX_IDENT_SYM = 1,
            LEX_SEMICOLON_SYM = 2,
            LEX_PLUS_SYM = 3,
            LEX_QUESTION_EQUALS_SYM = 4,
            LEX_EQUALS_SYM = 5,
            LEX_EQUALS_EQUALS_SYM = 6,
            LEX_NOT_EQUALS_SYM = 7,
            LEX_STRING_SYM = 8,
            LEX_COMMA_SYM = 9,
            LEX_AND_SYM = 10,
            LEX_OR_SYM = 11,
            LEX_NOT_SYM = 12,
            LEX_AT_SYM = 13,
            LEX_OPEN_BRACKET_SYM = 14,
            LEX_CLOSE_BRACKET_SYM = 15,
            LEX_OPEN_BRACE_SYM = 16,
            LEX_CLOSE_BRACE_SYM = 17,
            LEX_OPEN_PAREN_SYM = 18,
            LEX_CLOSE_PAREN_SYM = 19,
            LEX_EOF_SYM = 20,
            LEX_UNKNOWN_SYM = 21,
            LEX_UNKNOWN_FUNC_SYM = 22,
            LEX_IDENT_TOO_LONG_SYM = 23,
            LEX_STRING_WITH_EOL_SYM = 24,
            LEX_ILLEGAL_IDENT_SYM = 25,
            LEX_TWO_DOTS_IDENT_SYM = 26,
            LEX_BLOCK_STRING_WITH_EOF_SYM = 27,
            LEX_SOLE_DOT_IDENT_SYM = 28
        };
    }

}
