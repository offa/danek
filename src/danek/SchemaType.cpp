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

#include "danek/SchemaType.h"
#include "danek/SchemaValidator.h"
#include "danek/internal/Common.h"

namespace danek
{
    class SchemaValidator;

    SchemaType::SchemaType(const char* typeName, const char* className, ConfType cfgType)
        : m_typeName(typeName), m_className(className), m_cfgType(cfgType)
    {
    }

    void SchemaType::validate(const SchemaValidator* sv, const Configuration* cfg, const char* scope, const char* name,
                              const char* typeName, const char* origTypeName, const StringVector& typeArgs, int indentLevel) const
    {
        unused(origTypeName);

        const char* value;
        StringBuffer msg;
        StringBuffer fullyScopedName;
        StringBuffer errSuffix;

        value = cfg->lookupString(scope, name);
        if (!sv->callIsA(this, cfg, value, typeName, typeArgs, indentLevel + 1, errSuffix))
        {
            const char* sep;
            cfg->mergeNames(scope, name, fullyScopedName);
            if (errSuffix.size() == 0)
            {
                sep = "";
            }
            else
            {
                sep = "; ";
            }
            msg << cfg->fileName() << ": bad " << typeName << " value ('" << value << "') for '" << fullyScopedName << "'" << sep
                << errSuffix;
            throw ConfigurationException(msg.str());
        }
    }

    bool SchemaType::isA(const SchemaValidator* sv, const Configuration* cfg, const char* value, const char* typeName,
                         const StringVector& typeArgs, int indentLevel, StringBuffer& errSuffix) const
    {
        unused(sv);
        unused(cfg);
        unused(value);
        unused(typeName);
        unused(typeArgs);
        unused(indentLevel);
        unused(errSuffix);

        return false;
    }

    SchemaType* SchemaType::findType(const SchemaValidator* sv, const char* name) const
    {
        return sv->findType(name);
    }

    void SchemaType::callValidate(const SchemaType* target, const SchemaValidator* sv, const Configuration* cfg,
                                  const char* scope, const char* name, const char* typeName, const char* origTypeName,
                                  const StringVector& typeArgs, int indentLevel) const

    {
        sv->callValidate(target, cfg, scope, name, typeName, origTypeName, typeArgs, indentLevel);
    }

    bool SchemaType::callIsA(const SchemaType* target, const SchemaValidator* sv, const Configuration* cfg, const char* value,
                             const char* typeName, const StringVector& typeArgs, int indentLevel, StringBuffer& errSuffix) const
    {
        return sv->callIsA(target, cfg, value, typeName, typeArgs, indentLevel, errSuffix);
    }
}
