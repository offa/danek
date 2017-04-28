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

#include "SchemaTypeFloatWithUnits.h"
#include "Common.h"

namespace danek
{
    void SchemaTypeFloatWithUnits::checkRule(const SchemaValidator* sv, const Configuration* cfg,
        const char* typeName, const StringVector& typeArgs, const char* rule) const
        throw(ConfigurationException)
    {
        unused(sv);
        unused(cfg);

        StringBuffer msg;

        if (typeArgs.length() == 0)
        {
            msg << "The '" << typeName << "' type should take one or more "
                << "arguments (denoting units) in rule '" << rule << "'";
            throw ConfigurationException(msg.c_str());
        }
    }

    bool SchemaTypeFloatWithUnits::isA(const SchemaValidator* sv, const Configuration* cfg, const char* value,
        const char* typeName, const StringVector& typeArgs, int indentLevel, StringBuffer& errSuffix) const
    {
        unused(sv);
        unused(typeName);
        unused(indentLevel);

        const char** allowedUnits;
        int allowedUnitsSize;

        typeArgs.c_array(allowedUnits, allowedUnitsSize);
        bool result = cfg->isFloatWithUnits(value, allowedUnits, allowedUnitsSize);
        if (result == false)
        {
            errSuffix << "the value should be in the format '<float> <units>' "
                      << "where <units> is one of:";
            int len = typeArgs.length();
            for (int i = 0; i < len; i++)
            {
                if (i < len - 1)
                {
                    errSuffix << " '" << typeArgs[i] << "',";
                }
                else
                {
                    errSuffix << " '" << typeArgs[i] << "'";
                }
            }
        }
        return result;
    }

} // namespace danek
