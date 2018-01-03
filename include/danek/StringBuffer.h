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

#pragma once

#include <string>

namespace danek
{
    class StringBuffer
    {
    public:

        StringBuffer();
        explicit StringBuffer(const std::string& str);

        std::size_t size() const;

        char& operator[](std::size_t index);
        char operator[](std::size_t index) const;

        const std::string& str() const;

        void clear();

        void deleteLastChar();

        StringBuffer& append(char value);
        StringBuffer& append(int value);
        StringBuffer& append(float value);
        StringBuffer& append(const std::string& str);
        StringBuffer& append(const StringBuffer& other);


        StringBuffer& operator<<(const StringBuffer& other);
        StringBuffer& operator<<(const std::string& str);
        StringBuffer& operator<<(int val);
        StringBuffer& operator<<(float val);
        StringBuffer& operator<<(char ch);

        StringBuffer& operator=(const std::string& str);

    private:

        std::string m_string;
    };

}
