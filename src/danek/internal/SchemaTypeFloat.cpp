// Copyright (c) 2017-2021 offa
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

#include "danek/internal/SchemaTypeFloat.h"
#include "danek/internal/Common.h"

namespace danek
{
    void SchemaTypeFloat::checkRule(const SchemaValidator* sv, const Configuration* cfg, const char* typeName,
                                    const StringVector& typeArgs, const char* rule) const
    {
        unused(sv);

        StringBuffer msg;
        int len;
        float min;
        float max;

        len = typeArgs.size();
        if (len == 0)
        {
            return;
        }
        if (len != 2)
        {
            msg << "the '" << typeName << "' type should take either no arguments "
                << "or 2 arguments (denoting "
                << "min and max values) in rule '" << rule << "'";
            throw ConfigurationException(msg.str());
        }
        try
        {
            min = cfg->stringToFloat("", "", typeArgs[0].c_str());
        }
        catch (const ConfigurationException& ex)
        {
            msg << "non-float value for the first ('min') argument in rule '" << rule << "'";
            throw ConfigurationException(msg.str());
        }
        try
        {
            max = cfg->stringToFloat("", "", typeArgs[1].c_str());
        }
        catch (const ConfigurationException& ex)
        {
            msg << "non-float value for the second ('max') argument in rule '" << rule << "'";
            throw ConfigurationException(msg.str());
        }
        if (min > max)
        {
            msg << "the first ('min') value is larger than the second ('max') "
                << "argument in "
                << "rule '" << rule << "'";
            throw ConfigurationException(msg.str());
        }
    }

    bool SchemaTypeFloat::isA(const SchemaValidator* sv, const Configuration* cfg, const char* value, const char* typeName,
                              const StringVector& typeArgs, int indentLevel, StringBuffer& errSuffix) const
    {
        unused(sv);
        unused(typeName);
        unused(indentLevel);

        float val;
        float min;
        float max;

        try
        {
            val = cfg->stringToFloat("", "", value);
        }
        catch (const ConfigurationException& ex)
        {
            return false;
        }
        if (typeArgs.size() == 0)
        {
            return true;
        }
        min = cfg->stringToFloat("", "", typeArgs[0].c_str());
        max = cfg->stringToFloat("", "", typeArgs[1].c_str());
        if (val < min || val > max)
        {
            errSuffix << "the value is outside the permitted range [" << typeArgs[0].c_str() << ", " << typeArgs[1].c_str()
                      << "]";
            return false;
        }
        return true;
    }
}
