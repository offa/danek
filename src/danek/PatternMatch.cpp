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

#include "danek/PatternMatch.h"
#include "danek/internal/Compat.h"
#include <stdlib.h>
#include <string.h>

namespace danek
{
    namespace
    {
        bool patternMatchInternal(const wchar_t* wStr, int wStrIndex, int wStrLen, const wchar_t* wPattern, int wPatternIndex, int wPatternLen)
        {
            while (wPatternIndex < wPatternLen)
            {
                if (wPattern[wPatternIndex] != '*')
                {
                    if (wPattern[wPatternIndex] != wStr[wStrIndex])
                    {
                        return false;
                    }
                    wPatternIndex++;
                    if (wStrIndex < wStrLen)
                    {
                        wStrIndex++;
                    }
                }
                else
                {
                    wPatternIndex++;
                    while (wPattern[wPatternIndex] == '*')
                    {
                        wPatternIndex++;
                    }
                    if (wPatternIndex == wPatternLen)
                    {
                        return true;
                    }
                    for (; wStrIndex < wStrLen; wStrIndex++)
                    {
                        if (patternMatchInternal(wStr, wStrIndex, wStrLen, wPattern, wPatternIndex, wPatternLen))
                        {
                            return true;
                        }
                    }
                }
            }
            if (wPattern[wPatternIndex] != wStr[wStrIndex])
            {
                return false;
            }
            return true;
        }
    }


    //----------------------------------------------------------------------
    // Function:    patternMatch()
    //
    // Description: Returns true if the specified pattern matches the
    //          specified string.
    //
    // Note:    The only wildcard supported is "*". It acts like the
    //          "*" wildcard in UNIX and DOS shells, that is, it
    //          matches zero or more characters.
    //----------------------------------------------------------------------

    bool patternMatch(const char* str, const char* pattern)
    {
        wchar_t* wStr;
        int wStrLen;
        wchar_t* wPattern;
        int wPatternLen;
        bool result;
        int strLen;
        int patternLen;

        strLen = strlen(str);
        wStr = new wchar_t[strLen + 1];
        wStrLen = mbstowcs(wStr, str, strLen + 1);
        compat::checkAssertion(wStrLen != -1);

        patternLen = strlen(pattern);
        wPattern = new wchar_t[patternLen + 1];
        wPatternLen = mbstowcs(wPattern, pattern, patternLen + 1);
        compat::checkAssertion(wPatternLen != -1);

        result = patternMatchInternal(wStr, 0, wStrLen, wPattern, 0, wPatternLen);
        delete[] wStr;
        delete[] wPattern;
        return result;
    }
}
