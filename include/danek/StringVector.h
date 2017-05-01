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
#include <vector>
#include <string>

namespace danek
{
    /**
     * @deprecated Will be replaced; it's kept for compatibility reasons. Once the new API
     * is in place, this class should get a [[deprecated]]; atm. this would fail the build though.
     */
    class StringVector
    {
    public:
        explicit StringVector(std::size_t initialCapacity = 10);
        explicit StringVector(const std::vector<std::string>& data); // For compatibility reasons only


        void add(const std::string& str);
        void add(const StringBuffer& strBuf);
        void add(const StringVector& other);

        void sort();
        bool bSearchContains(const std::string& str) const;

        std::size_t length() const;
        void ensureCapacity(std::size_t size);

        void empty();
        void removeLast();

        void replace(std::size_t index, const std::string& str);

        std::vector<std::string> get() const;

        const std::string& operator[](std::size_t index) const;

        /** @deprecated These methods will be replaced with a proper API. */
        void addWithOwnership(StringBuffer& strBuf);
        void addWithOwnership(StringVector& other);

    private:
        std::vector<std::string> m_data;
    };

}
