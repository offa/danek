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

#include "danek/internal/SchemaTypeTuple.h"
#include "danek/internal/Common.h"
#include "danek/internal/Compat.h"

namespace danek
{
    void SchemaTypeTuple::checkRule(const SchemaValidator* sv, const Configuration* cfg, const char* typeName,
                                    const StringVector& typeArgs, const char* rule) const
    {
        unused(cfg);

        StringBuffer msg;

        //--------
        // Check there is at least one pair of type and name arguments.
        //--------
        int len = typeArgs.size();
        if ((len == 0) || (len % 2 != 0))
        {
            msg << "the '" << typeName << "' type requires pairs of type and "
                << "name arguments in rule '" << rule << "'";
            throw ConfigurationException(msg.str());
        }

        //--------
        // Check that all the type arguments are valid types.
        //--------
        for (int i = 0; i < len; i += 2)
        {
            const char* elemType = typeArgs[i + 0].c_str();
            SchemaType* typeDef = findType(sv, elemType);
            if (typeDef == nullptr)
            {
                msg << "unknown type '" << elemType << "' in rule '" << rule << "'";
                throw ConfigurationException(msg.str());
            }
            switch (typeDef->cfgType())
            {
                case ConfType::String:
                    break;
                case ConfType::List:
                    msg << "you cannot embed a list type ('" << elemType << "') inside a "
                        << "tuple in rule '" << rule << "'";
                    throw ConfigurationException(msg.str());
                case ConfType::Scope:
                    msg << "you cannot embed a scope type ('" << elemType << "') inside a "
                        << "tuple in rule '" << rule << "'";
                    throw ConfigurationException(msg.str());
                default:
                    throw std::exception{}; // Bug!
            }
        }
    }

    void SchemaTypeTuple::validate(const SchemaValidator* sv, const Configuration* cfg, const char* scope, const char* name,
                                   const char* typeName, const char* origTypeName, const StringVector& typeArgs,
                                   int indentLevel) const
    {
        unused(origTypeName);

        StringBuffer msg;
        StringBuffer errSuffix;
        StringBuffer fullyScopedName;
        StringVector emptyArgs;

        //--------
        // Check the length of the list matches the size of the tuple
        //--------
        int typeArgsSize = typeArgs.size();
        compat::checkAssertion(typeArgsSize != 0);
        compat::checkAssertion(typeArgsSize % 2 == 0);
        std::size_t numElems = typeArgsSize / 2;
        std::vector<std::string> data;
        cfg->lookupList(scope, name, data);
        if (data.size() != numElems)
        {
            cfg->mergeNames(scope, name, fullyScopedName);
            msg << cfg->fileName() << ": there should be " << static_cast<int>(numElems) << " entries in the '" << fullyScopedName
                << "' " << typeName << "; entries denote";
            for (std::size_t i = 0; i < numElems; i++)
            {
                msg << " '" << typeArgs[i * 2 + 0].c_str() << "'";
                if (i < numElems - 1)
                {
                    msg << ",";
                }
            }
            throw ConfigurationException(msg.str());
        }
        //--------
        // Check each item is of the type specified in the tuple
        //--------
        for (std::size_t i = 0; i < data.size(); i++)
        {
            int typeIndex = (i * 2 + 0) % typeArgsSize;
            int elemNameIndex = (i * 2 + 1) % typeArgsSize;
            const char* elemValue = data[i].c_str();
            const char* elemTypeName = typeArgs[typeIndex].c_str();
            SchemaType* elemTypeDef = findType(sv, elemTypeName);
            bool ok = callIsA(elemTypeDef, sv, cfg, elemValue, elemTypeName, emptyArgs, indentLevel + 1, errSuffix);
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
                msg << cfg->fileName() << ": bad " << elemTypeName << " value ('" << elemValue << "') for element "
                    << static_cast<int>(i + 1) << " ('" << typeArgs[elemNameIndex].c_str() << "') of the '" << fullyScopedName
                    << "' " << typeName << sep << errSuffix;
                throw ConfigurationException(msg.str());
            }
        }
    }
}
