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

#include "danek/StringVector.h"
#include <algorithm>

namespace danek
{

    StringVector::StringVector(std::size_t initialCapacity)
    {
        m_data.reserve(initialCapacity);
    }

    StringVector::StringVector(const std::vector<std::string>& data) : m_data(data)
    {
    }

    void StringVector::add(const std::string& str)
    {
        m_data.push_back(str);
    }

    void StringVector::add(const StringBuffer& strBuf)
    {
        add(strBuf.c_str());
    }

    void StringVector::replace(std::size_t index, const std::string& str)
    {
        m_data.at(index) = str;
    }

    std::vector<std::string> StringVector::get() const
    {
        return m_data;
    }

    std::size_t StringVector::length() const
    {
        return m_data.size();
    }

    void StringVector::ensureCapacity(std::size_t size)
    {
        m_data.reserve(size);
    }

    void StringVector::add(const StringVector& other)
    {
        const auto otherLen = other.length();
        m_data.reserve(m_data.size() + otherLen);

        for (std::size_t i = 0; i < otherLen; i++)
        {
            add(other[i]);
        }
    }

    void StringVector::empty()
    {
        std::vector<std::string>().swap(m_data);
    }

    void StringVector::removeLast()
    {
        m_data.erase(m_data.end() - 1);
    }

    const std::string& StringVector::operator[](std::size_t index) const
    {
        return m_data[index];
    }
}
