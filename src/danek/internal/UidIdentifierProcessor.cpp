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
#include "danek/internal/Compat.h"
#include "danek/ConfigurationException.h"
#include "danek/StringVector.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

namespace danek
{

    UidIdentifierProcessor::UidIdentifierProcessor() : m_count(0), m_uidToken("uid-")
    {
    }

    //----------------------------------------------------------------------
    // Spelling must be in one of the following forms:
    //      - "foo"                 -->  "foo"
    //      - "uid-<foo>"           -->  "uid-<digits>-<foo>"
    //      - "uid-<digits>-<foo>"  -->  "uid-<new-digits>-<foo>"
    // where "<foo>" does NOT start with a digit or "-"
    //----------------------------------------------------------------------

    void UidIdentifierProcessor::expand(StringBuffer& spelling)
    {
        //--------
        // Common case optimizations
        //--------
        if (strchr(spelling.str().c_str(), '.') == nullptr)
        {
            spelling = expandOne(spelling.str());
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

        StringVector vec{util::splitScopes(spelling.str())};
        const int len = vec.size();
        for (int i = 0; i < len; ++i)
        {
            buf = vec[i];
            try
            {
                buf = expandOne(buf.str());
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

    std::string UidIdentifierProcessor::expandOne(const std::string& spelling)
    {
        if( startsWithUidToken(spelling) == false )
        {
            return spelling;
        }

        const std::string errorMessage = "'" + spelling + "' is not a legal identifier";

        if( hasValidPrefix(spelling) == false )
        {
            throw ConfigurationException(errorMessage);
        }

        const auto startPayload = std::next(spelling.cbegin(), m_uidToken.size());
        auto itr = std::find_if_not(startPayload, spelling.cend(), [](auto v) { return std::isdigit(v); });

        if( itr == startPayload )
        {
            // "uid-foo"  --> "uid-<digits>-foo"
            return formatExpanded(std::string(itr, spelling.cend()));
        }

        if (itr == spelling.cend() || *itr != '-')
        {
            // illegal: "uid-<digits>" or "uid-<digits>foo"
            throw ConfigurationException(errorMessage);
        }
        std::advance(itr, 1); // point to just after "uid-<digits>-"
        if (itr == spelling.cend())
        {
            // illegal: "uid-<digits>-"
            throw ConfigurationException(errorMessage);
        }
        if (*itr == '-')
        {
            // illegal: "uid-<digits>--"
            throw ConfigurationException(errorMessage);
        }
        if( std::isdigit(*(itr)) == true )
        {
            // illegal: "uid-<digits>-<digits>foo"
            throw ConfigurationException(errorMessage);
        }

        return formatExpanded(std::string(itr, spelling.cend()));
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

        StringVector vec{util::splitScopes(spelling)};
        const int len = vec.size();
        for (int i = 0; i < len; ++i)
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
            ++ptr;
            ++count;
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

    bool UidIdentifierProcessor::startsWithUidToken(const std::string& str) const
    {
        return std::equal(m_uidToken.cbegin(), m_uidToken.cend(), str.cbegin());
    }

    bool UidIdentifierProcessor::hasValidPrefix(const std::string& str) const
    {
        return ( str != m_uidToken ) && ( str.at(m_uidToken.size()) != '-' );
    }

    std::string UidIdentifierProcessor::formatExpanded(const std::string& suffix)
    {
        compat::checkAssertion(m_count < 1000 * 1000 * 1000);
        constexpr std::size_t numDigits = 9;

        std::ostringstream ss;
        ss << m_uidToken << std::setw(numDigits) << std::setfill('0') << (m_count++) << "-" << suffix;

        return ss.str();
    }

}
