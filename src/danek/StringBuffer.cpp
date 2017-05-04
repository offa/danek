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

#include "danek/StringBuffer.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

namespace danek
{
    StringBuffer::StringBuffer() : m_string()
    {
    }

    StringBuffer::StringBuffer(const std::string& str) : m_string(str)
    {
    }

    const char* StringBuffer::c_str() const
    {
        return m_string.c_str();
    }

    std::size_t StringBuffer::size() const
    {
        return m_string.size();
    }

    char StringBuffer::lastChar() const
    {
        return *std::prev(m_string.cend());
    }

    char& StringBuffer::operator[](std::size_t index)
    {
        return m_string[index];
    }

    char StringBuffer::operator[](std::size_t index) const
    {
        return m_string[index];
    }

    const std::string& StringBuffer::str() const
    {
        return m_string;
    }

    void StringBuffer::clear()
    {
        std::string{}.swap(m_string);
    }

    void StringBuffer::deleteLastChar()
    {
        m_string.erase(std::prev(m_string.cend()));
    }

    StringBuffer& StringBuffer::append(char value)
    {
        m_string += value;
        return *this;
    }

    StringBuffer& StringBuffer::append(int value)
    {
        m_string += std::to_string(value);
        return *this;
    }

    StringBuffer& StringBuffer::append(float value)
    {
        m_string += std::to_string(value);
        return *this;
    }

    StringBuffer& StringBuffer::append(const std::string& str)
    {
        m_string += str;
        return *this;
    }

    StringBuffer& StringBuffer::append(StringBuffer other)
    {
        m_string += other.str();
        return *this;
    }


    StringBuffer& StringBuffer::operator<<(const StringBuffer& other)
    {
        append(other);
        return *this;
    }

    StringBuffer& StringBuffer::operator<<(const std::string& str)
    {
        append(str);
        return *this;
    }

    StringBuffer& StringBuffer::operator<<(int val)
    {
        append(val);
        return *this;
    }

    StringBuffer& StringBuffer::operator<<(float val)
    {
        append(val);
        return *this;
    }

    StringBuffer& StringBuffer::operator<<(char ch)
    {
        append(ch);
        return *this;
    }

    StringBuffer& StringBuffer::operator=(const std::string& str)
    {
        m_string = str;
        return *this;
    }

}
