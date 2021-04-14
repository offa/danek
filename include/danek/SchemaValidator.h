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

#pragma once

#include "danek/Configuration.h"
#include "danek/SchemaType.h"
#include "danek/internal/SchemaIdRuleInfo.h"
#include "danek/internal/SchemaIgnoreRuleInfo.h"


namespace danek
{
    class SchemaValidator;
    class SchemaParser;
    class SchemaIdRuleInfo;
    class SchemaIgnoreRuleInfo;

    class SchemaValidator
    {
    public:
        enum class ForceMode
        {
            None,
            Optional,
            Required
        };

        SchemaValidator();
        SchemaValidator(const SchemaValidator&) = delete;
        virtual ~SchemaValidator();

        void wantDiagnostics(bool value)
        {
            m_wantDiagnostics = value;
        }

        bool wantDiagnostics() const
        {
            return m_wantDiagnostics;
        }

        void parseSchema(const char** schema, int schemaSize);
        void parseSchema(const char** nullTerminatedSchema);

        void validate(const Configuration* cfg, const char* scope, const char* localName,
                      ForceMode forceMode = ForceMode::None) const
        {
            validate(cfg, scope, localName, true, ConfType::ScopesAndVars, forceMode);
        }

        void validate(const Configuration* cfg, const char* scope, const char* localName, bool recurseIntoSubscopes,
                      ConfType typeMask, ForceMode forceMode = ForceMode::None) const;


        SchemaValidator& operator=(const SchemaValidator&) = delete;


    protected:
        // Operations that can be called by a subclass.
        void registerType(SchemaType* type);


    private:
        friend int compareSchemaIdRuleInfo(const void*, const void*);
        friend int compareSchemaType(const void*, const void*);
        friend class SchemaParser;
        friend class SchemaType;

        // Helper operations.
        SchemaType* findType(const char* name) const;

        void validate(const Configuration* cfg, const char* scope, const char* localName, const StringVector& itemNames,
                      ForceMode forceMode) const;
        void validateForceMode(const Configuration* cfg, const char* scope, const char* localName, ForceMode forceMode) const;
        void validateRequiredUidEntry(const Configuration* cfg, const char* fullScope, SchemaIdRuleInfo* idRule) const;

        void callCheckRule(const SchemaType* target, const Configuration* cfg, const char* typeName, const StringVector& typeArgs,
                           const char* rule, int indentLevel) const;

        void callValidate(const SchemaType* target, const Configuration* cfg, const char* scope, const char* localName,
                          const char* typeName, const char* origTypeName, const StringVector& typeArgs, int indentLevel) const;

        bool callIsA(const SchemaType* target, const Configuration* cfg, const char* value, const char* typeName,
                     const StringVector& typeArgs, int indentLevel, StringBuffer& errSuffix) const;

        void printTypeArgs(const StringVector& typeArgs, int indentLevel) const;
        void printTypeNameAndArgs(const char* typeName, const StringVector& typeArgs, int indentLevel) const;

        void indent(int indentLevel) const;

        void registerBuiltinTypes();
        void sortTypes();
        void checkTypeDoesNotExist(const char* typeName);
        void ensureSpaceInTypesArray();

        void registerTypedef( // called by the SchemaParser class
            const char* typeName, ConfType cfgType, const char* baseTypeName, const StringVector& baseTypeArgs);

        SchemaIdRuleInfo* findIdRule(const char* name) const;
        bool shouldIgnore(const Configuration* cfg, const char* scope, const char* expandedName,
                          const char* unexpandedName) const;

        // Instance variables are NOT visible to subclasses.
        SchemaIdRuleInfo** m_idRules;
        int m_idRulesCurrSize;
        int m_idRulesMaxSize;

        SchemaIgnoreRuleInfo** m_ignoreRules;
        int m_ignoreRulesCurrSize;
        int m_ignoreRulesMaxSize;

        SchemaType** m_types;
        int m_typesCurrSize;
        int m_typesMaxSize;
        bool m_areTypesSorted;
        bool m_wantDiagnostics;
    };
}
