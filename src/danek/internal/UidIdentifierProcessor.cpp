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
#include <iterator>
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

    std::string UidIdentifierProcessor::expand(const std::string& spelling)
    {
        if( spelling.find('.') == std::string::npos )
        {
            return expandOne(spelling);
        }

        if( spelling.find(m_uidToken) == std::string::npos )
        {
            return spelling;
        }

        using OItr = std::ostream_iterator<std::string>;

        const auto scopes = util::splitScopes(spelling);
        const auto f = [this](auto s) { return expandOne(s); };
        std::ostringstream result;

        std::transform(scopes.cbegin(), std::prev(scopes.cend()), OItr{result, "."}, f);
        std::transform(std::prev(scopes.cend()), scopes.cend(), OItr{result}, f);

        return result.str();
    }

    std::string UidIdentifierProcessor::expandOne(const std::string& spelling)
    {
        if( startsWithUidToken(spelling) == false )
        {
            return spelling;
        }

        checkCondition(hasValidPrefix(spelling), spelling);

        const auto startPayload = std::next(spelling.cbegin(), m_uidToken.size());
        auto digitsEnd = std::find_if_not(startPayload, spelling.cend(), [](auto v) { return std::isdigit(v); });

        if( digitsEnd == startPayload )
        {
            return formatExpanded(std::string(digitsEnd, spelling.cend()));
        }

        checkCondition(hasValidPayload(digitsEnd, spelling.cend()), spelling);

        return formatExpanded(std::string(std::next(digitsEnd), spelling.cend()));
    }

    std::string UidIdentifierProcessor::unexpand(const char* spelling, StringBuffer& buf) const
    {
        //--------
        // Common case optimizations
        //--------
        if (strchr(spelling, '.') == nullptr)
        {
            const auto result = unexpandOne(spelling);
            buf = result;
            return result;
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

        StringVector vec{util::splitScopes(spelling)};
        const int len = vec.size();
        for (int i = 0; i < len; ++i)
        {
            std::string str;
            try
            {
                str = unexpandOne(vec[i]);
                buf = str;
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

    std::string UidIdentifierProcessor::unexpandOne(const std::string& spelling) const
    {
        if( startsWithUidToken(spelling) == false )
        {
            return spelling;
        }

        const auto startPayload = std::next(spelling.cbegin(), m_uidToken.size());
        auto digitsEnd = std::find_if_not(startPayload, spelling.cend(), [](auto v) { return std::isdigit(v); });

        if( digitsEnd == startPayload )
        {
            return spelling;
        }

        return formatUnexpanded(std::string(std::next(digitsEnd), spelling.cend()));
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

    std::string UidIdentifierProcessor::formatUnexpanded(const std::string& suffix) const
    {
        return m_uidToken + suffix;
    }

    void UidIdentifierProcessor::checkCondition(bool result, const std::string& input) const
    {
        if( result == false )
        {
            throw ConfigurationException{"'" + input + "' is not a legal identifier"};
        }
    }

}
