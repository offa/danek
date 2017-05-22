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

#include "danek/internal/SchemaTypeList.h"
#include "danek/internal/Common.h"
#include "danek/SchemaValidator.h"
#include <assert.h>

namespace danek
{
    void SchemaTypeList::checkRule(const SchemaValidator* sv, const Configuration* cfg, const char* typeName,
        const StringVector& typeArgs, const char* rule) const
    {
        unused(cfg);

        StringBuffer msg;
        int len;
        const char* listElementTypeName;
        SchemaType* typeDef;

        //--------
        // Check there is one argument.
        //--------
        len = typeArgs.size();
        if (len != 1)
        {
            msg << "the '" << typeName << "' type requires one type argument "
                << "in rule '" << rule << "'";
            throw ConfigurationException(msg.str());
        }

        //--------
        // The argument must be the name of a string-based type.
        //--------
        listElementTypeName = typeArgs[0].c_str();
        typeDef = findType(sv, listElementTypeName);
        if (typeDef == nullptr)
        {
            msg << "unknown type '" << listElementTypeName << "' in rule '" << rule << "'";
            throw ConfigurationException(msg.str());
        }
        switch (typeDef->cfgType())
        {
            case ConfType::String:
                break;
            case ConfType::List:
                msg << "you cannot embed a list type ('" << listElementTypeName
                    << "') inside another list in rule '" << rule << "'";
                throw ConfigurationException(msg.str());
            case ConfType::Scope:
                msg << "you cannot embed a scope type ('" << listElementTypeName
                    << "') inside a list in rule '" << rule << "'";
                throw ConfigurationException(msg.str());
            default:
                assert(0); // Bug!
        }
    }

    void SchemaTypeList::validate(const SchemaValidator* sv, const Configuration* cfg, const char* scope,
        const char* name, const char* typeName, const char* origTypeName, const StringVector& typeArgs,
        int indentLevel) const
    {
        unused(typeName);
        unused(origTypeName);

        StringBuffer msg;
        StringBuffer fullyScopedName;
        StringBuffer errSuffix;
        StringVector emptyArgs;
        std::vector<std::string> data;

        assert(typeArgs.size() == 1);
        const char* elemTypeName = typeArgs[0].c_str();
        SchemaType* elemTypeDef = findType(sv, elemTypeName);
        assert(elemTypeDef->cfgType() == ConfType::String);

        cfg->lookupList(scope, name, data);
        for (std::size_t i = 0; i < data.size(); i++)
        {
            const char* elemValue = data[i].c_str();
            bool ok =
                callIsA(elemTypeDef, sv, cfg, elemValue, elemTypeName, emptyArgs, indentLevel + 1, errSuffix);
            if (!ok)
            {
                const char* sep;
                if (errSuffix.size() == 0)
                {
                    sep = "";
                }
                else
                {
                    sep = "; ";
                }
                cfg->mergeNames(scope, name, fullyScopedName);
                msg << cfg->fileName() << ": bad " << elemTypeName << " value ('" << elemValue << "') for '"
                    << fullyScopedName << "[" << static_cast<int>(i) << "]'" << sep << errSuffix;
                throw ConfigurationException(msg.str());
            }
        }
    }
}
