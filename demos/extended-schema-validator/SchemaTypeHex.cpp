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

#include "SchemaTypeHex.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

SchemaTypeHex::~SchemaTypeHex()
{
}

void SchemaTypeHex::checkRule(const SchemaValidator* sv, const Configuration* cfg, const char* typeName,
    const StringVector& typeArgs, const char* rule) const
{
    (void) sv;
    StringBuffer msg;
    int len;
    int maxDigits;

    len = typeArgs.size();
    if (len == 0)
    {
        return;
    }
    else if (len > 1)
    {
        msg << "schema error: the '" << typeName << "' type should "
            << "take either no arguments or 1 argument (denoting "
            << "max-digits) in rule '" << rule << "'";
        throw ConfigurationException(msg.str());
    }
    try
    {
        maxDigits = cfg->stringToInt("", "", typeArgs[0].c_str());
    }
    catch (const ConfigurationException& ex)
    {
        msg << "schema error: non-integer value for the 'max-digits' "
            << "argument in rule '" << rule << "'";
        throw ConfigurationException(msg.str());
    }
    if (maxDigits < 1)
    {
        msg << "schema error: the 'max-digits' argument must be 1 or "
            << "greater in rule '" << rule << "'";
        throw ConfigurationException(msg.str());
    }
}

void SchemaTypeHex::validate(const SchemaValidator* sv, const Configuration* cfg, const char* scope,
    const char* name, const char* typeName, const char* origTypeName, const StringVector& typeArgs,
    int indentLevel) const
{
    (void) sv;
    (void) cfg;
    (void) scope;
    (void) name;
    (void) typeName;
    (void) origTypeName;
    (void) typeArgs;
    (void) indentLevel;
}

bool SchemaTypeHex::isA(const SchemaValidator* sv, const Configuration* cfg, const char* value,
    const char* typeName, const StringVector& typeArgs, int indentLevel, StringBuffer& errSuffix) const
{
    (void) sv;
    (void) typeName;
    (void) indentLevel;

    if (!isHex(value))
    {
        errSuffix << "the value is not a hexadecimal number";
        return false;
    }
    if (typeArgs.size() == 1)
    {
        //--------
        // Check if there are too many hex digits in the value
        //--------
        size_t maxDigits = cfg->stringToInt("", "", typeArgs[0].c_str());
        if (strlen(value) > maxDigits)
        {
            errSuffix << "the value must not contain more than " << static_cast<int>(maxDigits) << " digits";
            return false;
        }
    }
    return true;
}

int SchemaTypeHex::lookupHex(const Configuration* cfg, const char* scope, const char* localName)
{
    const char* str;

    str = cfg->lookupString(scope, localName);
    return stringToHex(cfg, scope, localName, str);
}

int SchemaTypeHex::lookupHex(const Configuration* cfg, const char* scope, const char* localName,
    int defaultVal)
{
    const char* str;

    if (cfg->type(scope, localName) == ConfType::NoValue)
    {
        return defaultVal;
    }
    str = cfg->lookupString(scope, localName);
    return stringToHex(cfg, scope, localName, str);
}

int SchemaTypeHex::stringToHex(const Configuration* cfg, const char* scope, const char* localName,
    const char* str, const char* typeName)
{
    unsigned int value;
    int status;
    StringBuffer msg;
    StringBuffer fullyScopedName;

    status = sscanf(str, "%x", &value);
    if (status != 1)
    {
        cfg->mergeNames(scope, localName, fullyScopedName);
        msg << cfg->fileName() << ": bad " << typeName << " value ('" << str << "') specified for '"
            << fullyScopedName;
        throw ConfigurationException(msg.str());
    }
    return static_cast<int>(value);
}

bool SchemaTypeHex::isHex(const char* str)
{
    int i;

    for (i = 0; str[i] != '\0'; i++)
    {
        if (!isxdigit(str[i]))
        {
            return false;
        }
    }
    return i > 0;
}
