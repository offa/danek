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

#include "danek/SchemaValidator.h"
#include "danek/internal/SchemaParser.h"
#include "danek/internal/SchemaIdRuleInfo.h"
#include "danek/internal/SchemaTypeBoolean.h"
#include "danek/internal/SchemaTypeDurationMicroseconds.h"
#include "danek/internal/SchemaTypeDurationMilliseconds.h"
#include "danek/internal/SchemaTypeDurationSeconds.h"
#include "danek/internal/SchemaTypeDummy.h"
#include "danek/internal/SchemaTypeEnum.h"
#include "danek/internal/SchemaTypeFloat.h"
#include "danek/internal/SchemaTypeFloatWithUnits.h"
#include "danek/internal/SchemaTypeInt.h"
#include "danek/internal/SchemaTypeIntWithUnits.h"
#include "danek/internal/SchemaTypeList.h"
#include "danek/internal/SchemaTypeMemorySizeBytes.h"
#include "danek/internal/SchemaTypeMemorySizeKB.h"
#include "danek/internal/SchemaTypeMemorySizeMB.h"
#include "danek/internal/SchemaTypeScope.h"
#include "danek/internal/SchemaTypeString.h"
#include "danek/internal/SchemaTypeTable.h"
#include "danek/internal/SchemaTypeTuple.h"
#include "danek/internal/SchemaTypeTypedef.h"
#include "danek/internal/SchemaTypeUnitsWithFloat.h"
#include "danek/internal/SchemaTypeUnitsWithInt.h"
#include "danek/internal/Compat.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

namespace danek
{
    int compareSchemaIdRuleInfo(const void* p1, const void* p2)
    {
        const auto r1 = static_cast<const SchemaIdRuleInfo* const*>(p1);
        const auto r2 = static_cast<const SchemaIdRuleInfo* const*>(p2);
        return strcmp((*r1)->locallyScopedName().c_str(), (*r2)->locallyScopedName().c_str());
    }

    extern "C" int danek_compareSchemaIdRuleInfo_c(const void* p1, const void* p2)
    {
        return compareSchemaIdRuleInfo(p1, p2);
    }

    int compareSchemaType(const void* p1, const void* p2)
    {
        const auto r1 = static_cast<const SchemaType* const*>(p1);
        const auto r2 = static_cast<const SchemaType* const*>(p2);
        return strcmp((*r1)->typeName(), (*r2)->typeName());
    }

    extern "C" int danek_compareSchemaType_c(const void* p1, const void* p2)
    {
        return compareSchemaType(p1, p2);
    }

    SchemaIdRuleInfo* SchemaValidator::findIdRule(const char* name) const
    {
        SchemaIdRuleInfo search;
        SchemaIdRuleInfo* searchPtr;

        search.setLocallyScopedName(name);
        searchPtr = &search;
        SchemaIdRuleInfo** result = static_cast<SchemaIdRuleInfo**>(bsearch(&searchPtr,
                                                                            m_idRules,
                                                                            m_idRulesCurrSize,
                                                                            sizeof(SchemaIdRuleInfo*),
                                                                            danek_compareSchemaIdRuleInfo_c));
        if (result == nullptr)
        {
            return nullptr;
        }
        return *result;
    }

    void SchemaValidator::sortTypes()
    {
        qsort(m_types, m_typesCurrSize, sizeof(SchemaType*), danek_compareSchemaType_c);
        m_areTypesSorted = true;
    }

    SchemaType* SchemaValidator::findType(const char* name) const
    {
        SchemaTypeDummy search(name);

        if (m_areTypesSorted)
        {
            SchemaType* searchPtr = &search;
            SchemaType** result = static_cast<SchemaType**>(bsearch(
                &searchPtr, m_types, m_typesCurrSize, sizeof(SchemaType*), danek_compareSchemaType_c));
            if (result == nullptr)
            {
                return nullptr;
            }
            compat::checkAssertion(*result != nullptr);
            return *result;
        }
        else
        {
            for (int i = 0; i < m_typesCurrSize; i++)
            {
                SchemaType* typeDef = m_types[i];
                if (strcmp(typeDef->typeName(), name) == 0)
                {
                    compat::checkAssertion(typeDef != nullptr);
                    return typeDef;
                }
            }
        }
        return nullptr;
    }

    //----------------------------------------------------------------------
    // Function:	Constructor
    //
    // Description:
    //----------------------------------------------------------------------

    SchemaValidator::SchemaValidator()
    {
        try
        {
            m_wantDiagnostics = false;
            m_idRulesCurrSize = 0;
            m_idRulesMaxSize = 0;
            m_idRules = nullptr;
            m_ignoreRulesCurrSize = 0;
            m_ignoreRulesMaxSize = 0;
            m_ignoreRules = nullptr;
            m_typesMaxSize = 25; // can grow bigger, if necessary
            m_types = new SchemaType*[m_typesMaxSize];
            m_typesCurrSize = 0;
            m_areTypesSorted = false;
            registerBuiltinTypes();
        }
        catch (const ConfigurationException&)
        {
            throw std::exception{}; // Bug!
        }
    }

    //----------------------------------------------------------------------
    // Function:	Destructor
    //
    // Description:
    //----------------------------------------------------------------------

    SchemaValidator::~SchemaValidator()
    {
        int i;

        for (i = 0; i < m_idRulesCurrSize; i++)
        {
            delete m_idRules[i];
        }
        delete[] m_idRules;

        for (i = 0; i < m_ignoreRulesCurrSize; i++)
        {
            delete m_ignoreRules[i];
        }
        delete[] m_ignoreRules;

        for (i = 0; i < m_typesCurrSize; i++)
        {
            delete m_types[i];
        }

        delete[] m_types;
    }

    void SchemaValidator::registerBuiltinTypes()
    {
        registerType(new SchemaTypeScope());

        //--------
        // List-based types
        //--------
        registerType(new SchemaTypeList());
        registerType(new SchemaTypeTable());
        registerType(new SchemaTypeTuple());

        //--------
        // String-based types
        //--------
        registerType(new SchemaTypeString());
        registerType(new SchemaTypeBoolean());
        registerType(new SchemaTypeDurationMicroseconds());
        registerType(new SchemaTypeDurationMilliseconds());
        registerType(new SchemaTypeDurationSeconds());
        registerType(new SchemaTypeEnum());
        registerType(new SchemaTypeFloat());
        registerType(new SchemaTypeFloatWithUnits());
        registerType(new SchemaTypeInt());
        registerType(new SchemaTypeIntWithUnits());
        registerType(new SchemaTypeUnitsWithFloat());
        registerType(new SchemaTypeUnitsWithInt());
        registerType(new SchemaTypeMemorySizeBytes());
        registerType(new SchemaTypeMemorySizeKB());
        registerType(new SchemaTypeMemorySizeMB());
    }

    void SchemaValidator::registerType(SchemaType* type)
    {
        checkTypeDoesNotExist(type->typeName());
        ensureSpaceInTypesArray();
        m_types[m_typesCurrSize] = type;
        m_typesCurrSize++;
        m_areTypesSorted = false;
    }

    void SchemaValidator::registerTypedef(const char* typeName, ConfType cfgType,
        const char* baseTypeName, const StringVector& baseTypeArgs)
    {
        checkTypeDoesNotExist(typeName);
        ensureSpaceInTypesArray();
        m_types[m_typesCurrSize] = new SchemaTypeTypedef(typeName, cfgType, baseTypeName, baseTypeArgs);
        m_typesCurrSize++;
        m_areTypesSorted = false;
    }

    void SchemaValidator::checkTypeDoesNotExist(const char* typeName)
    {
        StringBuffer msg;
        int i;

        for (i = 0; i < m_typesCurrSize; i++)
        {
            if (strcmp(m_types[i]->typeName(), typeName) == 0)
            {
                msg << "schema type '" << typeName << "' is already registed";
                throw ConfigurationException(msg.str());
            }
        }
    }

    void SchemaValidator::ensureSpaceInTypesArray()
    {
        if (m_typesCurrSize == m_typesMaxSize)
        {
            m_typesMaxSize = m_typesMaxSize * 2;
            SchemaType** newArray = new SchemaType*[m_typesMaxSize];
            for (int i = 0; i < m_typesCurrSize; i++)
            {
                newArray[i] = m_types[i];
            }
            delete[] m_types;
            m_types = newArray;
        }
    }

    void SchemaValidator::parseSchema(const char** nullTerminatedRulesArray)
    {
        int size;

        for (size = 0; nullTerminatedRulesArray[size] != nullptr; size++)
        {
        }
        parseSchema(nullTerminatedRulesArray, size);
    }

    void SchemaValidator::parseSchema(const char** schema, int schemaSize)
    {
        SchemaParser schemaParser(this);
        const char* prefix = "---- danek::SchemaValidator::parseSchema()";

        if (m_wantDiagnostics)
        {
            printf("\n%s: start\n", prefix);
        }
        try
        {
            schemaParser.parse(schema, schemaSize);
        }
        catch (const ConfigurationException& ex)
        {
            if (m_wantDiagnostics)
            {
                printf("\n%s: error: %s\n\n", prefix, ex.what());
            }
            throw;
        }
        if (m_wantDiagnostics)
        {
            printf("\n%s: end\n\n", prefix);
        }
    }

    void SchemaValidator::validate(const Configuration* cfg, const char* scope, const char* localName,
        bool recurseIntoSubscopes, ConfType typeMask, ForceMode forceMode) const

    {
        StringBuffer fullyScopedName;
        StringVector itemNames;

        //--------
        // Get a list of the entries in the scope.
        //--------
        cfg->mergeNames(scope, localName, fullyScopedName);
        cfg->listLocallyScopedNames(scope, localName, typeMask, recurseIntoSubscopes, itemNames);

        //--------
        // Now validte those names
        //--------
        validate(cfg, scope, localName, itemNames, forceMode);
    }

    void SchemaValidator::validate(const Configuration* cfg, const char* scope, const char* localName,
        const StringVector& itemNames, ForceMode forceMode) const
    {
        StringBuffer fullyScopedName;
        StringBuffer unlistedName;
        StringBuffer msg;
        StringBuffer buf;
        const char* prefix = "---- danek::SchemaValidator::validate()";

        cfg->mergeNames(scope, localName, fullyScopedName);

        if (m_wantDiagnostics)
        {
            printf("\n%s: start\n", prefix);
        }
        //--------
        // Compare every name in itemNames with m_ignoreRules and m_idRules.
        //--------
        int len = itemNames.size();
        for (int i = 0; i < len; i++)
        {
            const char* iName = itemNames[i].data();
            const std::string unexpandedName = cfg->unexpandUid(iName, buf);
            if (shouldIgnore(cfg, scope, iName, unexpandedName.c_str()))
            {
                if (m_wantDiagnostics)
                {
                    printf("\n  ignoring '%s'\n", iName);
                }
                continue;
            }
            SchemaIdRuleInfo* idRule = findIdRule(unexpandedName.c_str());
            if (idRule == nullptr)
            {
                //--------
                // Can't find an idRule for the entry
                //--------
                cfg->mergeNames(fullyScopedName.str().c_str(), iName, unlistedName);
                switch (cfg->type(unlistedName.str().c_str(), ""))
                {
                    case ConfType::Scope:
                        msg << cfg->fileName() << ": "
                            << "the '" << unlistedName << "' scope is unknown.";
                        break;
                    case ConfType::List:
                    case ConfType::String:
                        msg << cfg->fileName() << ": "
                            << "the '" << unlistedName << "' variable is unknown.";
                        break;
                    default:
                        throw std::exception{}; // Bug!
                }
                if (m_wantDiagnostics)
                {
                    printf("\n%s: error: %s\n", prefix, msg.str().c_str());
                }
                throw ConfigurationException(msg.str());
            }

            //--------
            // There is an idRule for the entry. Look up the idRule's
            // type, and invoke its validate() operation.
            //--------
            const char* typeName = idRule->typeName().c_str();
            SchemaType* typeDef = findType(typeName);
            compat::checkAssertion(typeDef != nullptr);
            try
            {
                callValidate(typeDef, cfg, fullyScopedName.str().c_str(), iName, typeName, typeName, StringVector(idRule->args()), 1);
            }
            catch (const ConfigurationException& ex)
            {
                if (m_wantDiagnostics)
                {
                    printf("\n%s: end\n\n", prefix);
                }
                throw;
            }
        }

        validateForceMode(cfg, scope, localName, forceMode);

        if (m_wantDiagnostics)
        {
            printf("\n%s: end\n\n", prefix);
        }
    }

    void SchemaValidator::validateForceMode(const Configuration* cfg, const char* scope,
        const char* localName, ForceMode forceMode) const
    {
        StringBuffer fullyScopedName;
        StringBuffer nameOfMissingEntry;
        StringBuffer msg;

        if (forceMode == ForceMode::Optional)
        {
            return;
        }
        cfg->mergeNames(scope, localName, fullyScopedName);
        for (int i = 0; i < m_idRulesCurrSize; i++)
        {
            SchemaIdRuleInfo* idRule = m_idRules[i];
            bool isOptional = idRule->isOptional();
            if (forceMode == ForceMode::None && isOptional)
            {
                continue;
            }
            const char* nameInRule = idRule->locallyScopedName().c_str();
            if (strstr(nameInRule, "uid-") != nullptr)
            {
                validateRequiredUidEntry(cfg, fullyScopedName.str().c_str(), idRule);
            }
            else
            {
                if (cfg->type(fullyScopedName.str().c_str(), nameInRule) == ConfType::NoValue)
                {
                    cfg->mergeNames(fullyScopedName.str().c_str(), nameInRule, nameOfMissingEntry);
                    const auto typeName = idRule->typeName();
                    msg << cfg->fileName() << ": the " << typeName << " '" << nameOfMissingEntry
                        << "' does not exist";
                    throw ConfigurationException(msg.str());
                }
            }
        }
    }

    void SchemaValidator::validateRequiredUidEntry(const Configuration* cfg, const char* fullScope,
        SchemaIdRuleInfo* idRule) const
    {
        const char* lastDot;
        StringBuffer parentScopePattern;
        StringBuffer nameOfMissingEntry;
        StringBuffer msg;
        StringVector parentScopes;
        const char* ptr;

        const char* nameInRule = idRule->locallyScopedName().c_str();
        compat::checkAssertion(strstr(nameInRule, "uid-") != nullptr);
        lastDot = strrchr(nameInRule, '.');
        if (lastDot == nullptr || strstr(lastDot + 1, "uid-") != nullptr)
        {
            return;
        }
        parentScopePattern = fullScope;
        if (fullScope[0] != '\0')
        {
            parentScopePattern.append('.');
        }
        for (ptr = nameInRule; ptr != lastDot; ptr++)
        {
            parentScopePattern.append(*ptr);
        }
        cfg->listFullyScopedNames(
            fullScope, "", ConfType::Scope, true, parentScopePattern.str().c_str(), parentScopes);
        int len = parentScopes.size();
        for (int i = 0; i < len; i++)
        {
            if (cfg->type(parentScopes[i].c_str(), lastDot + 1) == ConfType::NoValue)
            {
                cfg->mergeNames(parentScopes[i].c_str(), lastDot + 1, nameOfMissingEntry);
                const auto typeName = idRule->typeName();
                msg << cfg->fileName() << ": the " << typeName << " '" << nameOfMissingEntry
                    << "' does not exist";
                throw ConfigurationException(msg.str());
            }
        }
    }

    bool SchemaValidator::shouldIgnore(const Configuration* cfg, const char* scope, const char* expandedName,
        const char* unexpandedName) const
    {
        ConfType cfgType = ConfType::NoValue;

        for (int i = 0; i < m_ignoreRulesCurrSize; i++)
        {
            //--------
            // Does unexpandedName start with rule.m_locallyScopedName
            // followed by "."?
            //--------
            const char* name = m_ignoreRules[i]->locallyScopedName().c_str();
            const auto len = strlen(name);
            if (strncmp(unexpandedName, name, len) != 0)
            {
                continue;
            }
            if (unexpandedName[len] != '.')
            {
                continue;
            }

            //--------
            // It does. Whether we ignore the item depends on the
            // "@ignore<something>" keyword used.
            //--------
            const short symbol = m_ignoreRules[i]->symbol();
            switch (symbol)
            {
                case SchemaLex::LEX_IGNORE_EVERYTHING_IN_SYM:
                    return true;
                case SchemaLex::LEX_IGNORE_SCOPES_IN_SYM:
                case SchemaLex::LEX_IGNORE_VARIABLES_IN_SYM:
                    break;
                default:
                    throw std::exception{}; // Bug!
                    break;
            }
            const char* nameAfterPrefix = unexpandedName + len + 1;
            bool hasDotAfterPrefix = (strchr(nameAfterPrefix, '.') != nullptr);
            try
            {
                cfgType = cfg->type(scope, expandedName);
            }
            catch (const ConfigurationException& ex)
            {
                throw std::exception{}; // Bug!
            }
            if (symbol == SchemaLex::LEX_IGNORE_VARIABLES_IN_SYM)
            {
                if (hasDotAfterPrefix)
                {
                    //--------
                    // The item is a variable in a nested scope so
                    // the "@ignoreVariablesIn" rule does not apply.
                    //--------
                    continue;
                }
                //--------
                // The item is directly in the scope, so the
                // "@ignoreVariablesIn" rule applies if the item
                // is a variable.
                //--------
                if ((static_cast<int>(cfgType) & static_cast<int>(ConfType::Variables)) != 0)
                {
                    return true;
                }
                else
                {
                    continue;
                }
            }

            compat::checkAssertion(symbol == SchemaLex::LEX_IGNORE_SCOPES_IN_SYM);
            if (hasDotAfterPrefix)
            {
                //--------
                // The item is in a *nested* scope, so we ignore it.
                //--------
                return true;
            }
            //--------
            // The item is directly in the ignore-able scope,
            // so we ignore it only if the item is a scope.
            //--------
            if (cfgType == ConfType::Scope)
            {
                return true;
            }
            else
            {
                continue;
            }
        }
        return false;
    }

    void SchemaValidator::indent(int indentLevel) const
    {
        int i;

        for (i = 0; i < indentLevel; i++)
        {
            printf("  ");
        }
    }

    void SchemaValidator::printTypeArgs(const StringVector& typeArgs, int indentLevel) const
    {
        int len;
        int i;

        indent(indentLevel);
        printf("typeArgs = [");
        len = typeArgs.size();
        for (i = 0; i < len; i++)
        {
            printf("\"%s\"", typeArgs[i].c_str());
            if (i < len - 1)
            {
                printf(", ");
            }
        }
        printf("]\n");
    }

    void SchemaValidator::printTypeNameAndArgs(
        const char* typeName, const StringVector& typeArgs, int indentLevel) const
    {
        int len;
        int i;

        indent(indentLevel);
        printf("typeName = \"%s\"; typeArgs = [", typeName);
        len = typeArgs.size();
        for (i = 0; i < len; i++)
        {
            printf("\"%s\"", typeArgs[i].c_str());
            if (i < len - 1)
            {
                printf(", ");
            }
        }
        printf("]\n");
    }

    void SchemaValidator::callCheckRule(const SchemaType* target, const Configuration* cfg,
        const char* typeName, const StringVector& typeArgs, const char* rule, int indentLevel) const
    {
        try
        {
            if (m_wantDiagnostics)
            {
                printf("\n");
                indent(indentLevel);
                printf("start %s::checkRule()\n", target->className());
                indent(indentLevel + 1);
                printf("rule = \"%s\"\n", rule);
                printTypeNameAndArgs(typeName, typeArgs, indentLevel + 1);
            }
            target->checkRule(this, cfg, typeName, typeArgs, rule);
            if (m_wantDiagnostics)
            {
                indent(indentLevel);
                printf("end %s::checkRule()\n", target->className());
            }
        }
        catch (const ConfigurationException& ex)
        {
            if (m_wantDiagnostics)
            {
                printf("\n");
                indent(indentLevel);
                printf("exception thrown from %s::checkRule(): %s\n", target->className(), ex.what());
            }
            throw;
        }
    }

    void SchemaValidator::callValidate(const SchemaType* target, const Configuration* cfg, const char* scope,
        const char* name, const char* typeName, const char* origTypeName, const StringVector& typeArgs,
        int indentLevel) const
    {
        try
        {
            if (m_wantDiagnostics)
            {
                printf("\n");
                indent(indentLevel);
                printf("start %s::validate()\n", target->className());
                indent(indentLevel + 1);
                printf("scope = \"%s\"; name = \"%s\"\n", scope, name);
                indent(indentLevel + 1);
                printf("typeName = \"%s\"; origTypeName = \"%s\"\n", typeName, origTypeName);
                printTypeArgs(typeArgs, indentLevel + 1);
            }
            target->validate(this, cfg, scope, name, typeName, origTypeName, typeArgs, indentLevel + 1);
            if (m_wantDiagnostics)
            {
                indent(indentLevel);
                printf("end %s::validate()\n", target->className());
            }
        }
        catch (const ConfigurationException& ex)
        {
            if (m_wantDiagnostics)
            {
                printf("\n");
                indent(indentLevel);
                printf("exception thrown from %s::validate(): %s\n", target->className(), ex.what());
            }
            throw;
        }
    }

    bool SchemaValidator::callIsA(const SchemaType* target, const Configuration* cfg, const char* value,
        const char* typeName, const StringVector& typeArgs, int indentLevel, StringBuffer& errSuffix) const
    {
        bool result;

        try
        {
            if (m_wantDiagnostics)
            {
                printf("\n");
                indent(indentLevel);
                printf("start %s::isA()\n", target->className());
                indent(indentLevel + 1);
                printf("value = \"%s\"\n", value);
                printTypeNameAndArgs(typeName, typeArgs, indentLevel + 1);
            }
            result = target->isA(this, cfg, value, typeName, typeArgs, indentLevel + 1, errSuffix);
            if (m_wantDiagnostics)
            {
                indent(indentLevel);
                printf("end %s::isA()\n", target->className());
                indent(indentLevel + 1);
                printf("result = %s; errSuffix = \"%s\"\n", (result ? "true" : "false"), errSuffix.str().c_str());
            }
        }
        catch (const ConfigurationException& ex)
        {
            if (m_wantDiagnostics)
            {
                printf("\n");
                indent(indentLevel);
                printf("exception thrown from %s::isA(): %s\n", target->className(), ex.what());
            }
            throw;
        }
        return result;
    }
}
