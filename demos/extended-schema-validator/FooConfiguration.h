// Copyright (c) 2017-2019 offa
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

#include <vector>

class FooConfiguration
{
public:
    explicit FooConfiguration(bool wantDiagnostics = false);
    ~FooConfiguration();
    FooConfiguration(const FooConfiguration&) = delete;

    void parse(const char* cfgInput, const char* cfgScope = "", const char* secInput = "",
               const char* secScope = "");

    // Acccessors for configuration variables.
    int getTimeout() const
    {
        return m_timeout;
    }

    const char* getHost() const
    {
        return m_host;
    }

    int getHexByte() const
    {
        return m_hexByte;
    }

    int getHexWord() const
    {
        return m_hexWord;
    }

    void getHexList(const int*& array, int& arraySize)
    {
        array = m_hexList.data();
        arraySize = m_hexListSize;
    }

    FooConfiguration& operator=(const FooConfiguration&) = delete;


private:
    void* m_cfg; // opaque pointer to Config4Cpp config object
    bool m_wantDiagnostics;

    // Instance variables to cache configuration variables.
    int m_timeout;
    const char* m_host;
    int m_hexByte;
    int m_hexWord;
    std::vector<int> m_hexList;
    int m_hexListSize;
};
