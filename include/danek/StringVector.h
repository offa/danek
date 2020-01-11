// Copyright (c) 2017-2020 offa
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

#include <string>
#include <vector>

namespace danek
{
    /**
     * @deprecated Will be replaced; it's kept for compatibility reasons. Once the new API
     * is in place, this class should get a [[deprecated]]; atm. this would fail the build though.
     */
    class StringVector
    {
    public:
        explicit StringVector(std::size_t initialCapacity = 10)
        {
            m_data.reserve(initialCapacity);
        }

        explicit StringVector(const std::vector<std::string>& data)
            : m_data(data)
        {
        }


        void push_back(const std::string& str)
        {
            m_data.push_back(str);
        }

        auto begin()
        {
            return m_data.begin();
        }

        auto begin() const
        {
            return m_data.begin();
        }

        auto end()
        {
            return m_data.end();
        }

        auto end() const
        {
            return m_data.end();
        }

        std::size_t size() const
        {
            return m_data.size();
        }

        void reserve(std::size_t size)
        {
            m_data.reserve(size);
        }

        void clear()
        {
            m_data.clear();
        }

        void erase(std::vector<std::string>::const_iterator itr)
        {
            m_data.erase(itr);
        }

        std::vector<std::string> get() const
        {
            return m_data;
        }

        const std::string& operator[](std::size_t index) const
        {
            return m_data[index];
        }

    private:
        std::vector<std::string> m_data;
    };
}
