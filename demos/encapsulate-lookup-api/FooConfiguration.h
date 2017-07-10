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

#include <string>
#include <vector>
#include <exception>

class FooConfiguration
{
public:
    FooConfiguration();
    FooConfiguration(const FooConfiguration&) = delete;
    virtual ~FooConfiguration();

    void parse(const char* cfgSource, const char* scope = "");

    // Lookup-style functions.
    const char* lookupString(const char* name) const;
    void lookupList(const char* name, std::vector<std::string>& data) const;

    virtual int lookupInt(const char* name) const;
    virtual float lookupFloat(const char* name) const;
    virtual bool lookupBoolean(const char* name) const;
    virtual int lookupDurationMilliseconds(const char* name) const;
    virtual int lookupDurationSeconds(const char* name) const;


    FooConfiguration& operator=(const FooConfiguration&) = delete;

private:

    std::string m_scope;
    void* m_cfg;

};
