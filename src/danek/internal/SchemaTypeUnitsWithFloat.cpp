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

#include "danek/internal/SchemaTypeUnitsWithFloat.h"
#include "danek/internal/Common.h"

namespace danek
{
    void SchemaTypeUnitsWithFloat::checkRule(const SchemaValidator* sv, const Configuration* cfg, const char* typeName,
                                             const StringVector& typeArgs, const char* rule) const

    {
        unused(sv);
        unused(cfg);

        StringBuffer msg;

        if (typeArgs.size() == 0)
        {
            msg << "The '" << typeName << "' type should take one or more "
                << "arguments (denoting units) in rule '" << rule << "'";
            throw ConfigurationException(msg.str());
        }
    }

    bool SchemaTypeUnitsWithFloat::isA(const SchemaValidator* sv, const Configuration* cfg, const char* value,
                                       const char* typeName, const StringVector& typeArgs, int indentLevel,
                                       StringBuffer& errSuffix) const
    {
        unused(sv);
        unused(typeName);
        unused(indentLevel);

        const auto data = typeArgs.get();
        std::vector<const char*> buffer; // Deprecated conversion; kept for compatibility

        for (const auto& str : data)
        {
            buffer.push_back(&str.front());
        }

        const bool result = cfg->isUnitsWithFloat(value, buffer.data(), buffer.size());
        if (result == false)
        {
            errSuffix << "the value should be in the format '<units> <float>' "
                      << "where <units> is one of:";
            const auto len = typeArgs.size();
            for (std::size_t i = 0; i < len; i++)
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
}
