// Copyright (c) 2017-2021 offa
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

#include "danek/Configuration.h"
#include "danek/internal/Compat.h"
#include "danek/internal/ConfigurationImpl.h"
#include "danek/internal/MBChar.h"
#include <stdlib.h>
#include <string.h>

namespace danek
{
    Configuration* Configuration::create()
    {
        return new ConfigurationImpl();
    }

    void Configuration::destroy()
    {
        delete this;
    }

    void Configuration::mergeNames(const char* scope, const char* localName, StringBuffer& fullyScopedName)
    {
        if (scope[0] == '\0')
        {
            fullyScopedName = localName;
        }
        else if (localName[0] == '\0')
        {
            fullyScopedName = scope;
        }
        else
        {
            fullyScopedName.clear();
            fullyScopedName << scope << "." << localName;
        }
    }

    int Configuration::mbstrlen(const char* str)
    {
        char byte;
        wchar_t wChar;
        MBChar ch;
        mbstate_t mbtowcState;

        memset(&mbtowcState, 0, sizeof(mbtowcState));
        int count = 0;
        const char* ptr = str;
        while (*ptr != '\0')
        {
            int status = -1;
            while (status == -1)
            {
                byte = *ptr;
                ptr++;
                if (byte == '\0' && !ch.isEmpty())
                {
                    return -1; // invalid multi-byte string
                }
                if (byte == '\0')
                {
                    break;
                }
                if (!ch.add(byte))
                {
                    return -1; // invalid multi-byte string
                }
                status = mbrtowc(&wChar, ch.c_str(), ch.length(), &mbtowcState);
                if (status == -1 && ch.isFull())
                {
                    return -1; // invalid multi-byte string
                }
            }
            if (byte != '\0')
            {
                ++count;
            }
            ch.reset();
        }
        return count;
    }
}
