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

#include "danek/StringBuffer.h"
#include <string>

namespace danek
{
    class UidIdentifierProcessor
    {
    public:

        UidIdentifierProcessor();
        virtual ~UidIdentifierProcessor() = default;

        virtual std::string expand(const std::string& spelling);
        virtual std::string unexpand(const std::string& spelling) const;


    private:

        std::string expandOne(const std::string& spelling);
        std::string unexpandOne(const std::string& spelling) const;

        bool startsWithUidToken(const std::string& str) const;
        bool hasValidPrefix(const std::string& str) const;

        template<class Itr>
        bool hasValidPayload(Itr begin, Itr end) const
        {
            if( (begin == end) || (*begin != '-') )
            {
                return false;
            }
            const auto itr = std::next(begin);
            return ( std::distance(itr, end) > 0 ) && ( *itr != '-' ) && ( std::isdigit(*itr) == false );
        }

        std::string formatExpanded(const std::string& suffix);
        std::string formatUnexpanded(const std::string& suffix) const;
        void checkCondition(bool result, const std::string& input) const;


        std::size_t m_count;
        const std::string m_uidToken;
    };
}
