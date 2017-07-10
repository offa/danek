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

#include <wchar.h>
#include "danek/Configuration.h"
#include "danek/internal/FunctionType.h"
#include "LexToken.h"
#include "MBChar.h"
#include "UidIdentifierProcessor.h"
#include "LexBaseSymbols.h"
#include <fstream>

namespace danek
{
    class LexBase
    {
    public:
        //--------
        // Public operations
        //--------
        void nextToken(LexToken& token);


        struct KeywordInfo
        {
            const char* m_spelling;
            short m_symbol;
        };
        struct FuncInfo
        {
            const char* m_spelling;
            FunctionType m_funcType;
            short m_symbol;
        };

    protected:
        //--------
        // Constructors and destructor
        //--------
        LexBase(Configuration::SourceType sourceType, const char* source,
            UidIdentifierProcessor* uidIdentifierProcessor);
        explicit LexBase(const char* str);
        virtual ~LexBase();

        //--------
        // The constructors of a subclass should initialize the
        // following variables.
        //--------
        KeywordInfo* m_keywordInfoArray;
        int m_keywordInfoArraySize;
        FuncInfo* m_funcInfoArray;
        int m_funcInfoArraySize;

    private:
        //--------
        // Implementation-specific operations
        //--------
        void searchForKeyword(const char* spelling, bool& found, short& symbol);

        void searchForFunction(const char* spelling, bool& found, FunctionType& funcType, short& symbol);

        void nextChar();
        char nextByte();
        void consumeString(LexToken& token);
        void consumeBlockString(LexToken& token);
        bool isKeywordChar(const MBChar& ch);
        bool isIdentifierChar(const MBChar& ch);

        //--------
        // Instance variables
        //--------
        UidIdentifierProcessor* m_uidIdentifierProcessor;
        bool m_amOwnerOfUidIdentifierProcessor;
        int m_lineNum; // Used for error reporting
        MBChar m_ch; // Lookahead character
        Configuration::SourceType m_sourceType;
        const char* m_source;
        bool m_atEOF;
        mbstate_t m_mbtowcState;

        //--------
        // CFG_INPUT_FILE   uses m_file
        // CFG_INPUT_STRING uses m_ptr and m_source
        // CFG_INPUT_EXEC   uses m_ptr and m_execOutput
        //--------
        std::ifstream m_file;
        const char* m_ptr;
        StringBuffer m_execOutput;

        //--------
        // Unsupported constructors and assignment operators
        //--------
        LexBase();
        LexBase(const LexBase&);
        LexBase& operator=(const LexBase&);
    };
}
