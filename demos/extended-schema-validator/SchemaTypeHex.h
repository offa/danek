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

#include "danek/SchemaValidator.h"
using danek::Configuration;
using danek::ConfigurationException;
using danek::SchemaValidator;
using danek::SchemaType;
using danek::StringBuffer;
using danek::StringVector;
using danek::ConfType;

class SchemaTypeHex : public SchemaType
{
public:
    SchemaTypeHex() : SchemaType("hex", "SchemaTypeHex", ConfType::String)
    {
    }
    virtual ~SchemaTypeHex();

    static int lookupHex(const Configuration* cfg, const char* scope, const char* localName);

    static int lookupHex(const Configuration* cfg, const char* scope, const char* localName,
        int defaultVal);
    static int stringToHex(const Configuration* cfg, const char* scope, const char* localName,
        const char* str, const char* typeName = "hex");

    static bool isHex(const char* str);

protected:
    virtual void checkRule(const SchemaValidator* sv, const Configuration* cfg, const char* typeName,
        const StringVector& typeArgs, const char* rule) const;

    virtual void validate(const SchemaValidator* sv, const Configuration* cfg, const char* scope,
        const char* name, const char* typeName, const char* origTypeName, const StringVector& typeArgs,
        int indentLevel) const;

    virtual bool isA(const SchemaValidator* sv, const Configuration* cfg, const char* value,
        const char* typeName, const StringVector& typeArgs, int indentlevel, StringBuffer& errSuffix) const;
};
