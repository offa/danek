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

#include "danek/internal/UidIdentifierProcessor.h"
#include "danek/internal/Util.h"
#include "danek/StringVector.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>

namespace danek
{
    UidIdentifierProcessor::UidIdentifierProcessor() : m_count(0)
    {
        static_assert(sizeof(long) >= 4, "At least 32-bits");
    }

    UidIdentifierProcessor::~UidIdentifierProcessor()
    {
        // Nothing to do
    }

    //----------------------------------------------------------------------
    // Spelling must be in one of the following forms:
    //		- "foo"                 -->  "foo"
    //		- "uid-<foo>"           -->  "uid-<digits>-<foo>"
    //		- "uid-<digits>-<foo>"  -->  "uid-<new-digits>-<foo>"
    // where "<foo>" does NOT start with a digit or "-"
    //----------------------------------------------------------------------

    void UidIdentifierProcessor::expand(StringBuffer& spelling)
    {
        //--------
        // Common case optimizations
        //--------
        if (strchr(spelling.str().c_str(), '.') == nullptr)
        {
            expandOne(spelling);
            return;
        }
        if (strstr(spelling.str().c_str(), "uid-") == nullptr)
        {
            return;
        }

        //--------
        // Let's break apart the scoped name, expand each local part
        // and then recombine the parts into an expanded scoped name.
        //--------
        StringBuffer buf;
        StringBuffer msg;
        StringBuffer result;
        int i;
        int len;

        StringVector vec{util::splitScopes(spelling.str())};
        len = vec.size();
        for (i = 0; i < len; i++)
        {
            buf = vec[i];
            try
            {
                expandOne(buf);
            }
            catch (const ConfigurationException&)
            {
                msg << "'" << spelling << "' is not a legal identifier";
                throw ConfigurationException(msg.str());
            }
            result.append(buf);
            if (i < len - 1)
            {
                result.append(".");
            }
        }
        spelling = result;
    }

    void UidIdentifierProcessor::expandOne(StringBuffer& spelling)
    {
        int count;
        const char* ptr;
        StringBuffer msg;

        msg << "'" << spelling << "' is not a legal identifier";

        //--------
        // If spelling does not start with "uid-" then do nothing.
        //--------
        ptr = spelling.str().c_str();
        if (strncmp(ptr, "uid-", 4) != 0)
        {
            return;
        }

        StringBuffer suffix;
        char digits[10]; // big enough for 9 digits

        //--------
        // Check for "uid-" (with no suffix), because that is illegal
        //--------
        if (ptr[4] == '\0')
        {
            throw ConfigurationException(msg.str());
        }
        if (ptr[4] == '-')
        {
            // illegal: "uid--foo"
            throw ConfigurationException(msg.str());
        }

        if (!isdigit(ptr[4]))
        {
            //--------
            // "uid-foo"  --> "uid-<digits>-foo"
            //--------
            assert(m_count < 1000 * 1000 * 1000);
            sprintf(digits, "%09ld", m_count);
            m_count++;
            suffix = &(spelling.str().c_str()[4]); // deep copy
            spelling.clear();
            spelling << "uid-" << digits << "-" << suffix;
            return;
        }

        ptr += 4; // skip over "uid-"
        count = 0;
        while (isdigit(*ptr))
        {
            ptr++;
            count++;
        }
        assert(count > 0);
        if (*ptr == '\0' || *ptr != '-')
        {
            // illegal: "uid-<digits>" or "uid-<digits>foo"
            throw ConfigurationException(msg.str());
        }
        ptr++; // point to just after "uid-<digits>-"
        if (*ptr == '\0')
        {
            // illegal: "uid-<digits>-"
            throw ConfigurationException(msg.str());
        }
        if (*ptr == '-')
        {
            // illegal: "uid-<digits>--"
            throw ConfigurationException(msg.str());
        }
        if (isdigit(*(ptr)))
        {
            // illegal: "uid-<digits>-<digits>foo"
            throw ConfigurationException(msg.str());
        }
        assert(m_count < 1000 * 1000 * 1000);
        sprintf(digits, "%09ld", m_count);
        m_count++;
        suffix = ptr; // deep copy just after "uid-<digits>-"
        spelling.clear();
        spelling << "uid-" << digits << "-" << suffix;
    }

    const char* UidIdentifierProcessor::unexpand(const char* spelling, StringBuffer& buf) const

    {
        //--------
        // Common case optimizations
        //--------
        if (strchr(spelling, '.') == nullptr)
        {
            return unexpandOne(spelling, buf);
        }
        if (strstr(spelling, "uid-") == nullptr)
        {
            return spelling;
        }

        //--------
        // Let's break apart the scoped name, unexpand each local part
        // and then recombine the parts into an unexpanded scoped name.
        //--------
        StringBuffer result;
        StringBuffer msg;
        const char* str;
        int i;
        int len;

        StringVector vec{util::splitScopes(spelling)};
        len = vec.size();
        for (i = 0; i < len; i++)
        {
            try
            {
                str = unexpandOne(vec[i].c_str(), buf);
            }
            catch (const ConfigurationException&)
            {
                msg << "'" << spelling << "' is not a legal identifier";
                throw ConfigurationException(msg.str());
            }
            result.append(str);
            if (i < len - 1)
            {
                result.append(".");
            }
        }
        buf = result.str();
        return buf.str().c_str();
    }

    const char* UidIdentifierProcessor::unexpandOne(const char* spelling, StringBuffer& buf) const

    {
        int count;
        const char* ptr;

        //--------
        // If spelling does not start with "uid-<digits>-" then do nothing.
        //--------
        ptr = spelling;
        if (strncmp(ptr, "uid-", 4) != 0)
        {
            return spelling;
        }
        ptr += 4; // skip over "uid-"
        count = 0;
        while (isdigit(*ptr))
        {
            ptr++;
            count++;
        }
        if (count == 0 || *ptr != '-')
        {
            return spelling;
        }

        //--------
        // Okay, let's returned a modified spelling.
        //--------
        StringBuffer suffix;

        suffix = (ptr + 1); // deep copy from just after "uid-<digits>-"
        buf.clear();
        buf << "uid-" << suffix;
        return buf.str().c_str();
    }
}