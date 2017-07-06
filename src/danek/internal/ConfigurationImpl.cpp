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

#include "danek/internal/ConfigurationImpl.h"
#include "danek/internal/Util.h"
#include "danek/internal/platform.h"
#include "danek/internal/DefaultSecurityConfiguration.h"
#include "danek/internal/ConfigParser.h"
#include "danek/internal/ToString.h"
#include "danek/internal/ConfigItem.h"
#include "danek/internal/Common.h"
#include "danek/internal/Compat.h"
#include "danek/PatternMatch.h"
#include <algorithm>
#include <sstream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace danek
{
    //----------------------------------------------------------------------
    // Function:	Constructor
    //
    // Description:
    //----------------------------------------------------------------------

    ConfigurationImpl::ConfigurationImpl() : m_securityCfg(&DefaultSecurityConfiguration::singleton),
                                             m_fileName("<no file>"),
                                             m_rootScope(new ConfigScope{nullptr, ""}),
                                             m_currScope(m_rootScope),
                                             m_fallbackCfg(nullptr),
                                             m_amOwnerOfSecurityCfg(false),
                                             m_amOwnerOfFallbackCfg(false)
    {
    }

    //----------------------------------------------------------------------
    // Function:	Destructor
    //
    // Description:
    //----------------------------------------------------------------------

    ConfigurationImpl::~ConfigurationImpl()
    {
        delete m_rootScope;
        if (m_amOwnerOfSecurityCfg)
        {
            m_securityCfg->destroy();
        }
        if (m_amOwnerOfFallbackCfg)
        {
            m_fallbackCfg->destroy();
        }
    }

    void ConfigurationImpl::setFallbackConfiguration(Configuration* cfg)
    {
        if (m_amOwnerOfFallbackCfg)
        {
            m_fallbackCfg->destroy();
        }
        m_fallbackCfg = static_cast<ConfigurationImpl*>(cfg);
        m_amOwnerOfFallbackCfg = false;
    }

    void ConfigurationImpl::setFallbackConfiguration(Configuration::SourceType sourceType, const char* source,
        const char* sourceDescription)
    {
        Configuration* cfg;

        cfg = Configuration::create();
        try
        {
            cfg->parse(sourceType, source, sourceDescription);
        }
        catch (const ConfigurationException& ex)
        {
            cfg->destroy();
            std::stringstream msg;
            msg << "cannot set default configuration: " << ex.message();
            throw ConfigurationException(msg.str());
        }

        if (m_amOwnerOfFallbackCfg)
        {
            m_fallbackCfg->destroy();
        }
        m_fallbackCfg = static_cast<ConfigurationImpl*>(cfg);
        m_amOwnerOfFallbackCfg = true;
    }

    const Configuration* ConfigurationImpl::getFallbackConfiguration()
    {
        return m_fallbackCfg;
    }

    void ConfigurationImpl::setSecurityConfiguration(
        Configuration* cfg, bool takeOwnership, const char* scope)
    {
        StringVector dummyList;

        try
        {
            cfg->lookupList(scope, "allow_patterns", dummyList);
            cfg->lookupList(scope, "deny_patterns", dummyList);
            cfg->lookupList(scope, "trusted_directories", dummyList);
        }
        catch (const ConfigurationException& ex)
        {
            std::stringstream msg;
            msg << "cannot set security configuration: " << ex.message();
            throw ConfigurationException(msg.str());
        }

        m_securityCfg = cfg;
        m_securityCfgScope = scope;
        m_amOwnerOfSecurityCfg = takeOwnership;
    }

    void ConfigurationImpl::setSecurityConfiguration(const char* cfgInput, const char* scope)
    {
        Configuration* cfg;
        StringVector dummyList;

        cfg = Configuration::create();
        try
        {
            cfg->parse(cfgInput);
            cfg->lookupList(scope, "allow_patterns", dummyList);
            cfg->lookupList(scope, "deny_patterns", dummyList);
            cfg->lookupList(scope, "trusted_directories", dummyList);
        }
        catch (const ConfigurationException& ex)
        {
            cfg->destroy();
            std::stringstream msg;
            msg << "cannot set security configuration: " << ex.message();
            throw ConfigurationException(msg.str());
        }

        if (m_amOwnerOfSecurityCfg)
        {
            m_securityCfg->destroy();
        }
        m_securityCfg = cfg;
        m_securityCfgScope = scope;
        m_amOwnerOfSecurityCfg = true;
    }

    void ConfigurationImpl::getSecurityConfiguration(const Configuration*& cfg, const char*& scope)
    {
        cfg = m_securityCfg;
        scope = m_securityCfgScope.str().c_str();
    }

    //----------------------------------------------------------------------
    // Function:	parse()
    //
    // Description:
    //----------------------------------------------------------------------

    void ConfigurationImpl::parse(Configuration::SourceType sourceType, const char* source,
        const char* sourceDescription)
    {
        StringBuffer trustedCmdLine;

        switch (sourceType)
        {
            case Configuration::SourceType::File:
                m_fileName = source;
                break;
            case Configuration::SourceType::String:
                if (strcmp(sourceDescription, "") == 0)
                {
                    m_fileName = "<string-based configuration>";
                }
                else
                {
                    m_fileName = sourceDescription;
                }
                break;
            case Configuration::SourceType::Exec:
                if (strcmp(sourceDescription, "") == 0)
                {
                    m_fileName.clear();
                    m_fileName << "exec#" << source;
                }
                else
                {
                    m_fileName = sourceDescription;
                }
                if (!isExecAllowed(source, trustedCmdLine))
                {
                    std::stringstream msg;
                    msg << "cannot parse output of executing \"" << source << "\" "
                        << "due to security restrictions";
                    throw ConfigurationException(msg.str());
                }
                break;
            default:
                throw std::exception{}; // Bug!
                break;
        }
        ConfigParser parser(sourceType, source, trustedCmdLine.str().c_str(), m_fileName.str().c_str(), this);
    }

    //----------------------------------------------------------------------
    // Function:	type()
    //
    // Description:	Return the type of the named entry.
    //----------------------------------------------------------------------

    ConfType ConfigurationImpl::type(const char* scope, const char* localName) const
    {
        ConfType result;
        StringBuffer fullyScopedName;

        mergeNames(scope, localName, fullyScopedName);
        const ConfigItem* item = lookup(fullyScopedName.str().c_str(), localName);
        if (item == nullptr)
        {
            result = ConfType::NoValue;
        }
        else
        {
            result = item->type();
        }
        return result;
    }

    //----------------------------------------------------------------------
    // Function:	stringValue()
    //
    // Description:	Return the string, if any, associated with the named
    //		entry. Indicates success/failure via the "status" parameter.
    //----------------------------------------------------------------------

    void ConfigurationImpl::stringValue(
        const char* fullyScopedName, const char* localName, const char*& str, ConfType& type) const
    {
        const ConfigItem* item = lookup(fullyScopedName, localName);
        if (item == nullptr)
        {
            type = ConfType::NoValue;
            str = nullptr;
        }
        else
        {
            type = item->type();
            if (type == ConfType::String)
            {
                str = item->stringVal().c_str();
            }
            else
            {
                str = nullptr;
            }
        }
    }

    //----------------------------------------------------------------------
    // Function:	listValue()
    //
    // Description:	Return the list, if any, associated with the named
    //		entry.
    //----------------------------------------------------------------------

    void ConfigurationImpl::listValue(const char* fullyScopedName, const char* localName, StringVector& list, ConfType& type) const
    {
        const ConfigItem* item = lookup(fullyScopedName, localName);
        if (item == nullptr)
        {
            type = ConfType::NoValue;
            list.clear();
        }
        else
        {
            type = item->type();
            if (type == ConfType::List)
            {
                list = StringVector{item->listVal()};
            }
            else
            {
                list.clear();
            }
        }
    }

    //----------------------------------------------------------------------
    // Function:	listValue()
    //
    // Description:	Return the list, if any, associated with the named entry.
    //----------------------------------------------------------------------

    void ConfigurationImpl::listValue(const char* fullyScopedName, const char* localName, std::vector<std::string>& data, ConfType& type) const
    {
        const ConfigItem* item = lookup(fullyScopedName, localName);
        if (item == nullptr)
        {
            type = ConfType::NoValue;
            data = std::vector<std::string>{};
        }
        else
        {
            type = item->type();
            if (type == ConfType::List)
            {
                data = item->listVal();
            }
            else
            {
                data = std::vector<std::string>{};
            }
        }
    }

    //----------------------------------------------------------------------
    // Function:	insertString()
    //
    // Description:	Insert a named string into the symbol table.
    //
    // Notes:	Overwrites an existing entry of the same name.
    //----------------------------------------------------------------------

    void ConfigurationImpl::insertString(const char* scope, const char* localName, const char* str)
    {
        ConfigScope* scopeObj;
        StringBuffer fullyScopedName;

        mergeNames(scope, localName, fullyScopedName);
        StringVector vec{util::splitScopes(fullyScopedName.str())};
        const auto len = vec.size();
        ensureScopeExists(vec, 0, len - 2, scopeObj);
        if (!scopeObj->addOrReplaceString(vec[len - 1].c_str(), str))
        {
            std::stringstream msg;
            msg << fileName() << ": "
                << "variable '" << fullyScopedName.str() << "' was previously used as a scope";
            throw ConfigurationException(msg.str());
        }
    }

    //----------------------------------------------------------------------
    // Function:	ensureScopeExists()
    //
    // Description:
    //----------------------------------------------------------------------

    void ConfigurationImpl::ensureScopeExists(const char* scope, const char* localName)
    {
        StringBuffer fullyScopedName;
        ConfigScope* dummyScope;

        mergeNames(scope, localName, fullyScopedName);
        ensureScopeExists(fullyScopedName.str().c_str(), dummyScope);
    }

    //----------------------------------------------------------------------
    // Function:	insertList()
    //
    // Description:	Insert a named list into the symbol table.
    //
    // Notes:	Overwrites an existing entry of the same name.
    //----------------------------------------------------------------------

    void ConfigurationImpl::insertList(const char* scope, const char* localName, std::vector<std::string> data)
    {
        ConfigScope* scopeObj;
        StringBuffer fullyScopedName;

        mergeNames(scope, localName, fullyScopedName);
        StringVector vec{util::splitScopes(fullyScopedName.str())};
        const auto len = vec.size();
        ensureScopeExists(vec, 0, len - 2, scopeObj);

        if (!scopeObj->addOrReplaceList(vec[len - 1].c_str(), data))
        {
            std::stringstream msg;
            msg << fileName() << ": "
                << "variable '" << fullyScopedName.str() << "' was previously used as a scope";
            throw ConfigurationException(msg.str());
        }
    }

    //----------------------------------------------------------------------
    // Function:	insertList()
    //
    // Description:	Insert a named list into the symbol table.
    //
    // Notes:	Overwrites an existing entry of the same name.
    //----------------------------------------------------------------------

    void ConfigurationImpl::insertList(const char* scope, const char* localName, const StringVector& vec)
    {
        insertList(scope, localName, vec.get());
    }

    //----------------------------------------------------------------------
    // Function:	insertList()
    //
    // Description:	Insert a named list into the symbol table.
    //
    // Notes:	Overwrites an existing entry of the same name.
    //----------------------------------------------------------------------

    void ConfigurationImpl::insertList(const char* name, const StringVector& list)
    {
        ConfigScope* scope;

        StringVector vec{util::splitScopes(name)};
        const auto len = vec.size();
        ensureScopeExists(vec, 0, len - 2, scope);

        if (!scope->addOrReplaceList(vec[len - 1].c_str(), list.get()))
        {
            std::stringstream msg;
            msg << fileName() << ": "
                << "variable '" << name << "' was previously used as a scope";
            throw ConfigurationException(msg.str());
        }
    }

    //----------------------------------------------------------------------
    // Function:	remove()
    //
    // Description:	Remove the specified item.
    //----------------------------------------------------------------------

    void ConfigurationImpl::remove(const char* scope, const char* localName)
    {
        StringBuffer fullyScopedName;
        ConfigScope* scopeObj;
        int i;
        int len;

        scopeObj = m_currScope;
        mergeNames(scope, localName, fullyScopedName);
        StringVector vec{util::splitScopes(fullyScopedName.str())};
        len = vec.size();
        for (i = 0; i < len - 1; i++)
        {
            const ConfigItem* item = scopeObj->findItem(vec[i].c_str());
            if (item == nullptr || item->type() != ConfType::Scope)
            {
                std::stringstream msg;
                msg << fileName() << ": '" << fullyScopedName.str() << "' does not exist'";
                throw ConfigurationException(msg.str());
            }
            scopeObj = item->scopeVal();
            compat::checkAssertion(scopeObj != nullptr);
        }
        compat::checkAssertion(i == len - 1);
        compat::checkAssertion(scopeObj != nullptr);
        if (!scopeObj->removeItem(vec[i].c_str()))
        {
            std::stringstream msg;
            msg << fileName() << ": '" << fullyScopedName.str() << "' does not exist'";
            throw ConfigurationException(msg.str());
        }
    }

    //----------------------------------------------------------------------
    // Function:	empty()
    //
    // Description:	Re-initialize the configuration object
    //----------------------------------------------------------------------

    void ConfigurationImpl::empty()
    {
        delete m_rootScope;
        m_fileName = "<no file>";
        m_rootScope = new ConfigScope(nullptr, "");
        m_currScope = m_rootScope;
    }

    //----------------------------------------------------------------------
    // Function:	lookup()
    //
    // Description:
    //----------------------------------------------------------------------

    const ConfigItem* ConfigurationImpl::lookup(const char* fullyScopedName, const char* localName, bool startInRoot) const
    {
        StringVector vec;
        const ConfigScope* scope;

        if (fullyScopedName[0] == '\0')
        {
            return nullptr;
        }
        if (fullyScopedName[0] == '.')
        {
            //--------
            // Search only in the root scope and skip over '.'
            //--------
            vec = StringVector{util::splitScopes(&fullyScopedName[1])};
            scope = m_rootScope;
        }
        else if (startInRoot)
        {
            //--------
            // Search only in the root scope
            //--------
            vec = StringVector{util::splitScopes(fullyScopedName)};
            scope = m_rootScope;
        }
        else
        {
            //--------
            // Start search from the current scope
            //--------
            vec = StringVector{util::splitScopes(fullyScopedName)};
            scope = m_currScope;
        }
        const ConfigItem* item = nullptr;
        while (scope != nullptr)
        {
            item = lookupHelper(scope, vec);
            if (item != nullptr)
            {
                break;
            }
            scope = scope->parentScope();
        }
        if (item == nullptr && m_fallbackCfg != nullptr)
        {
            item = m_fallbackCfg->lookup(localName, localName, true);
        }
        return item;
    }

    const ConfigItem* ConfigurationImpl::lookupHelper(const ConfigScope* scope, const StringVector& vec) const
    {
        int len;
        int i;

        len = vec.size();
        for (i = 0; i < len - 1; i++)
        {
            const ConfigItem* item = scope->findItem(vec[i].c_str());
            if (item == nullptr || item->type() != ConfType::Scope)
            {
                return nullptr;
            }
            scope = item->scopeVal();
            compat::checkAssertion(scope != nullptr);
        }
        compat::checkAssertion(i == len - 1);
        compat::checkAssertion(scope != nullptr);
        return scope->findItem(vec[i].c_str());
    }

    //----------------------------------------------------------------------
    // Function:	dump()
    //
    // Description:	Generate a "printout" of the entire contents.
    //----------------------------------------------------------------------

    void ConfigurationImpl::dump(StringBuffer& buf, bool wantExpandedUidNames) const
    {
        buf = "";
        buf << toString(*m_rootScope, wantExpandedUidNames);
    }

    //----------------------------------------------------------------------
    // Function:	dump()
    //
    // Description:	Generate a "printout" of the specified entry.
    //----------------------------------------------------------------------

    void ConfigurationImpl::dump(StringBuffer& buf, bool wantExpandedUidNames, const char* scope,
        const char* localName) const
    {
        StringBuffer fullyScopedName;

        buf = "";
        mergeNames(scope, localName, fullyScopedName);
        if (strcmp(fullyScopedName.str().c_str(), "") == 0)
        {
            buf << toString(*m_rootScope, wantExpandedUidNames);
        }
        else
        {
            const ConfigItem* item = lookup(fullyScopedName.str().c_str(), localName, true);
            if (item == nullptr)
            {
                std::stringstream msg;
                msg << fileName() << ": "
                    << "'" << fullyScopedName.str() << "' is not an entry";
                throw ConfigurationException(msg.str());
            }

            buf << toString(*item, fullyScopedName.str(), wantExpandedUidNames);
        }
    }

    //----------------------------------------------------------------------
    // Function:	listFullyScopedNames()
    //
    // Description:
    //----------------------------------------------------------------------

    void ConfigurationImpl::listFullyScopedNames(const char* scope, const char* localName, ConfType typeMask,
        bool recursive, StringVector& names) const
    {
        StringVector filterPatterns;

        listFullyScopedNames(scope, localName, typeMask, recursive, filterPatterns, names);
    }

    void ConfigurationImpl::listFullyScopedNames(const char* scope, const char* localName, ConfType typeMask,
        bool recursive, const char* filterPattern, StringVector& names) const
    {
        StringVector filterPatterns;

        filterPatterns.push_back(filterPattern);
        listFullyScopedNames(scope, localName, typeMask, recursive, filterPatterns, names);
    }

    void ConfigurationImpl::listFullyScopedNames(const char* scope, const char* localName, ConfType typeMask,
        bool recursive, const StringVector& filterPatterns, StringVector& names) const

    {
        StringBuffer fullyScopedName;
        ConfigScope* scopeObj;

        mergeNames(scope, localName, fullyScopedName);
        if (strcmp(fullyScopedName.str().c_str(), "") == 0)
        {
            scopeObj = m_rootScope;
        }
        else
        {
            const ConfigItem* item = lookup(fullyScopedName.str().c_str(), localName, true);
            if (item == nullptr || item->type() != ConfType::Scope)
            {
                std::stringstream msg;
                msg << fileName() << ": "
                    << "'" << fullyScopedName.str() << "' is not a scope";
                throw ConfigurationException(msg.str());
            }
            scopeObj = item->scopeVal();
        }
        auto v = scopeObj->listFullyScopedNames(typeMask, recursive, filterPatterns.get());
        std::sort(v.begin(), v.end());
        names = StringVector{v};
    }

    //----------------------------------------------------------------------
    // Function:	listLocallyScopedNames()
    //
    // Description:
    //----------------------------------------------------------------------

    void ConfigurationImpl::listLocallyScopedNames(const char* scope, const char* localName, ConfType typeMask,
        bool recursive, const char* filterPattern, StringVector& names) const
    {
        StringVector filterPatterns;

        filterPatterns.push_back(filterPattern);
        listLocallyScopedNames(scope, localName, typeMask, recursive, filterPatterns, names);
    }

    void ConfigurationImpl::listLocallyScopedNames(const char* scope, const char* localName, ConfType typeMask,
        bool recursive, StringVector& names) const
    {
        StringVector filterPatterns;

        listLocallyScopedNames(scope, localName, typeMask, recursive, filterPatterns, names);
    }

    void ConfigurationImpl::listLocallyScopedNames(const char* scope, const char* localName, ConfType typeMask,
        bool recursive, const StringVector& filterPatterns, StringVector& names) const

    {
        StringBuffer fullyScopedName;
        ConfigScope* scopeObj;

        mergeNames(scope, localName, fullyScopedName);
        if (strcmp(fullyScopedName.str().c_str(), "") == 0)
        {
            scopeObj = m_rootScope;
        }
        else
        {
            const ConfigItem* item = lookup(fullyScopedName.str().c_str(), localName, true);
            if (item == nullptr || item->type() != ConfType::Scope)
            {
                std::stringstream msg;
                msg << fileName() << ": "
                    << "'" << fullyScopedName.str() << "' is not a scope";
                throw ConfigurationException(msg.str());
            }
            scopeObj = item->scopeVal();
        }
        auto v = scopeObj->listLocallyScopedNames(typeMask, recursive, filterPatterns.get());
        std::sort(v.begin(), v.end());
        names = StringVector{v};
    }

    const char* ConfigurationImpl::lookupString(
        const char* scope, const char* localName, const char* defaultVal) const
    {
        ConfType type;
        std::stringstream msg;
        const char* str;
        StringBuffer fullyScopedName;

        mergeNames(scope, localName, fullyScopedName);
        stringValue(fullyScopedName.str().c_str(), localName, str, type);
        switch (type)
        {
            case ConfType::String:
                break;
            case ConfType::NoValue:
                str = defaultVal;
                break;
            case ConfType::Scope:
                msg << fileName() << ": '" << fullyScopedName.str() << "' is a scope instead of a string";
                throw ConfigurationException(msg.str());
            case ConfType::List:
                msg << fileName() << ": '" << fullyScopedName.str() << "' is a list instead of a string";
                throw ConfigurationException(msg.str());
            default:
                throw std::exception{}; // Bug
        }
        return str;
    }

    const char* ConfigurationImpl::lookupString(const char* scope, const char* localName) const

    {
        ConfType type;
        std::stringstream msg;
        const char* str;
        StringBuffer fullyScopedName;

        mergeNames(scope, localName, fullyScopedName);
        stringValue(fullyScopedName.str().c_str(), localName, str, type);
        switch (type)
        {
            case ConfType::String:
                break;
            case ConfType::NoValue:
                msg << fileName() << ": no value specified for '" << fullyScopedName.str() << "'";
                throw ConfigurationException(msg.str());
            case ConfType::Scope:
                msg << fileName() << ": '" << fullyScopedName.str() << "' is a scope instead of a string";
                throw ConfigurationException(msg.str());
            case ConfType::List:
                msg << fileName() << ": "
                    << "'" << fullyScopedName.str() << "' is a list instead of a string";
                throw ConfigurationException(msg.str());
            default:
                throw std::exception{}; // Bug
        }
        return str;
    }

    void ConfigurationImpl::lookupList(const char* scope, const char* localName, std::vector<std::string>& data, const char** defaultArray, int defaultArraySize) const

    {
        ConfType type;
        std::stringstream msg;
        StringBuffer fullyScopedName;

        mergeNames(scope, localName, fullyScopedName);
        listValue(fullyScopedName.str().c_str(), localName, data, type);
        switch (type)
        {
            case ConfType::List:
                break;
            case ConfType::NoValue:
                std::vector<std::string>(defaultArraySize).swap(data);
                for (std::size_t i = 0; i < static_cast<std::size_t>(defaultArraySize); i++)
                {
                    data[i] = defaultArray[i];
                }
                break;
            case ConfType::Scope:
                msg << fileName() << ": "
                    << "'" << fullyScopedName.str() << "' is a scope instead of a list";
                throw ConfigurationException(msg.str());
            case ConfType::String:
                msg << fileName() << ": "
                    << "'" << fullyScopedName.str() << "' is a string instead of a list";
                throw ConfigurationException(msg.str());
            default:
                throw std::exception{}; // Bug
        }
    }

    void ConfigurationImpl::lookupList(const char* scope, const char* localName, std::vector<std::string>& data) const
    {
        ConfType type;
        std::stringstream msg;
        StringBuffer fullyScopedName;

        mergeNames(scope, localName, fullyScopedName);
        listValue(fullyScopedName.str().c_str(), localName, data, type);
        switch (type)
        {
            case ConfType::List:
                break;
            case ConfType::NoValue:
                msg << fileName() << ": no value specified for '" << fullyScopedName.str() << "'";
                throw ConfigurationException(msg.str());
            case ConfType::Scope:
                msg << fileName() << ": '" << fullyScopedName.str() << "' is a scope instead of a list";
                throw ConfigurationException(msg.str());
            case ConfType::String:
                msg << fileName() << ": "
                    << "'" << fullyScopedName.str() << "' is a string instead of a list";
                throw ConfigurationException(msg.str());
            default:
                throw std::exception{}; // Bug
        }
    }

    void ConfigurationImpl::lookupList(const char* scope, const char* localName, StringVector& list,
        const StringVector& defaultList) const
    {
        ConfType type;
        std::stringstream msg;
        StringBuffer fullyScopedName;
        std::vector<std::string> data;

        mergeNames(scope, localName, fullyScopedName);
        listValue(fullyScopedName.str().c_str(), localName, data, type);
        switch (type)
        {
            case ConfType::List:
                list = StringVector{data};
                break;
            case ConfType::NoValue:
                list = defaultList;
                break;
            case ConfType::Scope:
                msg << fileName() << ": '" << fullyScopedName.str() << "' is a scope instead of a list";
                throw ConfigurationException(msg.str());
            case ConfType::String:
                msg << fileName() << ": '" << fullyScopedName.str() << "' is a string instead of a list";
                throw ConfigurationException(msg.str());
            default:
                throw std::exception{}; // Bug
        }
    }

    void ConfigurationImpl::lookupList(const char* scope, const char* localName, StringVector& list) const

    {
        ConfType type;
        std::stringstream msg;
        StringBuffer fullyScopedName;
        std::vector<std::string> data;

        mergeNames(scope, localName, fullyScopedName);
        listValue(fullyScopedName.str().c_str(), localName, data, type);
        switch (type)
        {
            case ConfType::List:
                list = StringVector{data};
                break;
            case ConfType::NoValue:
                msg << fileName() << ": no value specified for '" << fullyScopedName.str() << "'";
                throw ConfigurationException(msg.str());
                break;
            case ConfType::Scope:
                msg << fileName() << ": '" << fullyScopedName.str() << "' is a scope instead of a list";
                throw ConfigurationException(msg.str());
            case ConfType::String:
                msg << fileName() << ": "
                    << "'" << fullyScopedName.str() << "' is a string instead of a list";
                throw ConfigurationException(msg.str());
            default:
                throw std::exception{}; // Bug
        }
    }

    int ConfigurationImpl::lookupEnum(const char* scope, const char* localName, const char* typeName,
        const EnumNameAndValue* enumInfo, int numEnums, const char* defaultVal) const

    {
        int result;
        StringBuffer fullyScopedName;
        std::stringstream msg;

        const char* strValue = lookupString(scope, localName, defaultVal);

        //--------
        // Check if the value matches anything in the enumInfo list.
        //--------
        if (!enumVal(strValue, enumInfo, numEnums, result))
        {
            mergeNames(scope, localName, fullyScopedName);
            msg << fileName() << ": bad " << typeName << " value ('" << strValue << "') specified for '"
                << fullyScopedName.str() << "'; should be one of:";
            for (int i = 0; i < numEnums; i++)
            {
                if (i < numEnums - 1)
                {
                    msg << " '" << enumInfo[i].name << "',";
                }
                else
                {
                    msg << " '" << enumInfo[i].name << "'";
                }
            }
            throw ConfigurationException(msg.str());
        }
        return result;
    }

    int ConfigurationImpl::lookupEnum(const char* scope, const char* localName, const char* typeName,
        const EnumNameAndValue* enumInfo, int numEnums, int defaultVal) const
    {
        int result;
        StringBuffer fullyScopedName;

        if (type(scope, localName) == ConfType::NoValue)
        {
            return defaultVal;
        }

        const char* strValue = lookupString(scope, localName);

        //--------
        // Check if the value matches anything in the enumInfo list.
        //--------
        if (!enumVal(strValue, enumInfo, numEnums, result))
        {
            std::stringstream msg;
            mergeNames(scope, localName, fullyScopedName);
            msg << fileName() << ": bad " << typeName << " value ('" << strValue << "') specified for '"
                << fullyScopedName.str() << "'; should be one of:";
            for (int i = 0; i < numEnums; i++)
            {
                if (i < numEnums - 1)
                {
                    msg << " '" << enumInfo[i].name << "',";
                }
                else
                {
                    msg << " '" << enumInfo[i].name << "'";
                }
            }
            throw ConfigurationException(msg.str());
        }
        return result;
    }

    int ConfigurationImpl::lookupEnum(const char* scope, const char* localName, const char* typeName,
        const EnumNameAndValue* enumInfo, int numEnums) const
    {
        int result;
        StringBuffer fullyScopedName;

        const char* strValue = lookupString(scope, localName);

        //--------
        // Check if the value matches anything in the enumInfo list.
        //--------
        if (!enumVal(strValue, enumInfo, numEnums, result))
        {
            std::stringstream msg;
            mergeNames(scope, localName, fullyScopedName);
            msg << fileName() << ": bad " << typeName << " value ('" << strValue << "') specified for '"
                << fullyScopedName.str() << "'; should be one of:";
            for (int i = 0; i < numEnums; i++)
            {
                if (i < numEnums - 1)
                {
                    msg << " '" << enumInfo[i].name << "',";
                }
                else
                {
                    msg << " '" << enumInfo[i].name << "'";
                }
            }
            throw ConfigurationException(msg.str());
        }
        return result;
    }

    //----------------------------------------------------------------------
    // Function:	EnumVal()
    //
    // Description:	Compare "name"'s spelling against the list
    //		of supplied enumerated (spelling, value) pairs. If
    //		present in the list, return the correct value. In any
    //		event, return success/failure.
    //----------------------------------------------------------------------

    bool ConfigurationImpl::enumVal(
        const char* name, const EnumNameAndValue* enumInfo, int numEnums, int& val) const
    {
        int i;

        for (i = 0; i < numEnums; i++)
        {
            if (!strcmp(name, enumInfo[i].name))
            {
                //--------
                // Found it.
                //--------
                val = enumInfo[i].value;
                return true;
            }
        }

        //--------
        // Failure.
        //--------
        return false;
    }

    static EnumNameAndValue boolInfo[] = {{"false", 0}, {"true", 1}};

    int countBoolInfo = sizeof(boolInfo) / sizeof(boolInfo[0]);

    bool ConfigurationImpl::lookupBoolean(const char* scope, const char* localName, bool defaultVal) const

    {
        int intVal;
        const char* defaultStrVal;

        if (defaultVal)
        {
            defaultStrVal = "true";
        }
        else
        {
            defaultStrVal = "false";
        }
        intVal = lookupEnum(scope, localName, "boolean", boolInfo, countBoolInfo, defaultStrVal);
        return intVal != 0;
    }

    bool ConfigurationImpl::lookupBoolean(const char* scope, const char* localName) const

    {
        int intVal;

        intVal = lookupEnum(scope, localName, "boolean", boolInfo, countBoolInfo);
        return intVal != 0;
    }

    int ConfigurationImpl::lookupInt(const char* scope, const char* localName, int defaultVal) const

    {
        const char* strValue;
        int result;
        char defaultStrVal[64]; // Big enough

        sprintf(defaultStrVal, "%d", defaultVal);
        strValue = lookupString(scope, localName, defaultStrVal);
        result = stringToInt(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupInt(const char* scope, const char* localName) const

    {
        const char* strValue;
        int result;

        strValue = lookupString(scope, localName);
        result = stringToInt(scope, localName, strValue);
        return result;
    }

    bool ConfigurationImpl::isInt(const char* str) const
    {
        int i;
        int intValue;
        char dummy;

        i = sscanf(str, "%d%c", &intValue, &dummy);
        return i == 1;
    }

    int ConfigurationImpl::stringToInt(const char* scope, const char* localName, const char* str) const

    {
        int result;
        char dummy;
        int i;
        StringBuffer fullyScopedName;

        //--------
        // Convert the string value into an int value.
        //--------
        i = sscanf(str, "%d%c", &result, &dummy);
        if (i != 1)
        {
            //--------
            // The number is badly formatted. Report an error.
            //--------
            mergeNames(scope, localName, fullyScopedName);
            std::stringstream msg;
            msg << fileName() << ": non-integer value for '" << fullyScopedName.str() << "'";
            throw ConfigurationException(msg.str());
        }
        return result;
    }

    bool ConfigurationImpl::isFloat(const char* str) const
    {
        int i;
        float floatValue;
        char dummy;

        i = sscanf(str, "%f%c", &floatValue, &dummy);
        return i == 1;
    }

    float ConfigurationImpl::stringToFloat(const char* scope, const char* localName, const char* str) const

    {
        float result;
        char dummy;
        int i;
        StringBuffer fullyScopedName;

        //--------
        // Convert the string value into a float value.
        //--------
        i = sscanf(str, "%f%c", &result, &dummy);
        if (i != 1)
        {
            //--------
            // The number is badly formatted. Report an error.
            //--------
            mergeNames(scope, localName, fullyScopedName);
            std::stringstream msg;
            msg << fileName() << ": non-numeric value for '" << fullyScopedName.str() << "'";
            throw ConfigurationException(msg.str());
        }
        return result;
    }

    bool ConfigurationImpl::isEnum(const char* str, const EnumNameAndValue* enumInfo, int numEnums) const
    {
        int dummyValue;
        bool result;

        result = enumVal(str, enumInfo, numEnums, dummyValue);
        return result;
    }

    bool ConfigurationImpl::isBoolean(const char* str) const
    {
        int dummyValue;
        bool result;

        result = enumVal(str, boolInfo, countBoolInfo, dummyValue);
        return result;
    }

    int ConfigurationImpl::stringToEnum(const char* scope, const char* localName, const char* typeName,
        const char* str, const EnumNameAndValue* enumInfo, int numEnums) const
    {
        StringBuffer fullyScopedName;
        int result;

        //--------
        // Check if the value matches anything in the enumInfo list.
        //--------
        if (!enumVal(str, enumInfo, numEnums, result))
        {
            mergeNames(scope, localName, fullyScopedName);
            std::stringstream msg;
            msg << fileName() << ": bad " << typeName << " value specified for '" << fullyScopedName.str()
                << "'; should be one of:";
            for (int i = 0; i < numEnums; i++)
            {
                if (i < numEnums - 1)
                {
                    msg << " '" << enumInfo[i].name << "',";
                }
                else
                {
                    msg << " '" << enumInfo[i].name << "'";
                }
            }
            throw ConfigurationException(msg.str());
        }
        return result;
    }

    bool ConfigurationImpl::stringToBoolean(const char* scope, const char* localName, const char* str) const

    {
        int result;

        result = stringToEnum(scope, localName, "boolean", str, boolInfo, countBoolInfo);
        return result != 0;
    }

    void ConfigurationImpl::lookupFloatWithUnits(const char* scope, const char* localName,
        const char* typeName, const char** allowedUnits, int allowedUnitsSize, float& floatResult,
        const char*& unitsResult) const
    {
        const char* str;

        str = lookupString(scope, localName);
        stringToFloatWithUnits(
            scope, localName, typeName, str, allowedUnits, allowedUnitsSize, floatResult, unitsResult);
    }

    void ConfigurationImpl::lookupFloatWithUnits(const char* scope, const char* localName,
        const char* typeName, const char** allowedUnits, int allowedUnitsSize, float& floatResult,
        const char*& unitsResult, float defaultFloat, const char* defaultUnits) const

    {
        if (type(scope, localName) == ConfType::NoValue)
        {
            floatResult = defaultFloat;
            unitsResult = defaultUnits;
        }
        else
        {
            lookupFloatWithUnits(scope,
                localName,
                typeName,
                allowedUnits,
                allowedUnitsSize,
                floatResult,
                unitsResult,
                defaultFloat,
                defaultUnits);
        }
    }

    bool ConfigurationImpl::isFloatWithUnits(const char* str, const char** allowedUnits, int allowedUnitsSize) const
    {
        // See if it is in the form "<float> <units>"
        std::stringstream ss{str};
        float fVal;
        std::string unitSpelling;

        ss >> fVal >> unitSpelling;

        if( ss.fail() == true )
        {
            return false;
        }

        // The entry appears to be in the correct format. Find out
        // what the specified units are.
        for (int i=0; i<allowedUnitsSize; ++i)
        {
            if (strcmp(unitSpelling.c_str(), allowedUnits[i]) == 0)
            {
                return true;
            }
        }

        // An unknown unit was specified.
        return false;
    }

    void ConfigurationImpl::lookupUnitsWithFloat(const char* scope, const char* localName,
        const char* typeName, const char** allowedUnits, int allowedUnitsSize, float& floatResult,
        const char*& unitsResult) const
    {
        const char* str;

        str = lookupString(scope, localName);
        stringToUnitsWithFloat(
            scope, localName, typeName, str, allowedUnits, allowedUnitsSize, floatResult, unitsResult);
    }

    void ConfigurationImpl::lookupUnitsWithFloat(const char* scope, const char* localName,
        const char* typeName, const char** allowedUnits, int allowedUnitsSize, float& floatResult,
        const char*& unitsResult, float defaultFloat, const char* defaultUnits) const

    {
        if (type(scope, localName) == ConfType::NoValue)
        {
            floatResult = defaultFloat;
            unitsResult = defaultUnits;
        }
        else
        {
            lookupFloatWithUnits(scope,
                localName,
                typeName,
                allowedUnits,
                allowedUnitsSize,
                floatResult,
                unitsResult,
                defaultFloat,
                defaultUnits);
        }
    }

    bool ConfigurationImpl::isUnitsWithFloat(
        const char* str, const char** allowedUnits, int allowedUnitsSize) const
    {
        int maxUnitsLen = 0;
        for (int index = 0; index < allowedUnitsSize; index++)
        {
            int len = strlen(allowedUnits[index]);
            if (len > maxUnitsLen)
            {
                maxUnitsLen = len;
            }
        }
        char* formatStr = new char[maxUnitsLen + 7]; // big enough
        char* unitSpelling = new char[strlen(str) + 1]; // big enough

        //--------
        // See if the string is in the form "allowedUnits[index] <float>"
        //--------
        for (int index = 0; index < allowedUnitsSize; index++)
        {
            sprintf(formatStr, "%s %%f%%c", allowedUnits[index]);
            float fVal;
            char dummyCh;
            int i = sscanf(str, formatStr, &fVal, &dummyCh);
            if (i == 1)
            {
                delete[] formatStr;
                delete[] unitSpelling;
                return true;
            }
        }

        delete[] formatStr;
        delete[] unitSpelling;
        return false;
    }

    void ConfigurationImpl::stringToIntWithUnits(const char* scope, const char* localName,
        const char* typeName, const char* str, const char** allowedUnits, int allowedUnitsSize,
        int& intResult, const char*& unitsResult) const
    {
        StringBuffer fullyScopedName;
        std::stringstream msg;

        // See if the string is in the form "<int> <units>"
        std::stringstream ss{str};
        int intVal;
        std::string unitSpelling;

        ss >> intVal >> unitSpelling;

        if ( ss.fail() == true )
        {
            mergeNames(scope, localName, fullyScopedName);
            msg << fileName() << ": invalid " << typeName << " ('" << str << "') specified for '"
                << fullyScopedName.str() << "': should be"
                << " '<int> <units>' where <units> are";
            for (int i = 0; i < allowedUnitsSize; ++i)
            {
                msg << " '" << allowedUnits[i] << "'";
                if (i < allowedUnitsSize - 1)
                {
                    msg << ",";
                }
            }
            throw ConfigurationException(msg.str());
        }

        // The entry appears to be in the correct format. Find out
        // what the specified units are.
        for (int i = 0; i < allowedUnitsSize; ++i)
        {
            if (strcmp(unitSpelling.c_str(), allowedUnits[i]) == 0)
            {
                // Success!
                intResult = intVal;
                unitsResult = allowedUnits[i];
                return;
            }
        }

        // Error: an unknown unit was specified.
        mergeNames(scope, localName, fullyScopedName);
        msg << fileName() << ": invalid " << typeName << " ('" << str << "') specified for '"
            << fullyScopedName.str() << "': should be"
            << " '<int> <units>' where <units> are";
        for (int i = 0; i < allowedUnitsSize; ++i)
        {
            msg << " '" << allowedUnits[i] << "'";
            if (i < allowedUnitsSize - 1)
            {
                msg << ",";
            }
        }
        throw ConfigurationException(msg.str());
    }

    void ConfigurationImpl::lookupIntWithUnits(const char* scope, const char* localName, const char* typeName,
        const char** allowedUnits, int allowedUnitsSize, int& intResult, const char*& unitsResult) const

    {
        const char* str;

        str = lookupString(scope, localName);
        stringToIntWithUnits(
            scope, localName, typeName, str, allowedUnits, allowedUnitsSize, intResult, unitsResult);
    }

    void ConfigurationImpl::lookupIntWithUnits(const char* scope, const char* localName, const char* typeName,
        const char** allowedUnits, int allowedUnitsSize, int& intResult, const char*& unitsResult,
        int defaultInt, const char* defaultUnits) const
    {
        if (type(scope, localName) == ConfType::NoValue)
        {
            intResult = defaultInt;
            unitsResult = defaultUnits;
        }
        else
        {
            lookupIntWithUnits(scope,
                localName,
                typeName,
                allowedUnits,
                allowedUnitsSize,
                intResult,
                unitsResult,
                defaultInt,
                defaultUnits);
        }
    }

    bool ConfigurationImpl::isIntWithUnits(
        const char* str, const char** allowedUnits, int allowedUnitsSize) const
    {
        // See if it is in the form "<int> <units>"
        std::stringstream ss{str};
        int intVal;
        std::string unitSpelling;

        ss >> intVal >> unitSpelling;

        if ( ss.fail() == true )
        {
            return false;
        }

        // The entry appears to be in the correct format. Find out
        // what the specified units are.
        for (int i = 0; i < allowedUnitsSize; i++)
        {
            if (strcmp(unitSpelling.c_str(), allowedUnits[i]) == 0)
            {
                return true;
            }
        }

        // An unknown unit was specified.
        return false;
    }

    void ConfigurationImpl::stringToUnitsWithInt(const char* scope, const char* localName,
        const char* typeName, const char* str, const char** allowedUnits, int allowedUnitsSize,
        int& intResult, const char*& unitsResult) const
    {
        StringBuffer fullyScopedName;
        int maxUnitsLen = 0;
        std::stringstream msg;

        for (int index = 0; index < allowedUnitsSize; index++)
        {
            int len = strlen(allowedUnits[index]);
            if (len > maxUnitsLen)
            {
                maxUnitsLen = len;
            }
        }
        char* formatStr = new char[maxUnitsLen + 7]; // big enough
        char* unitSpelling = new char[strlen(str) + 1]; // big enough

        //--------
        // See if the string is in the form "allowedUnits[index] <int>"
        //--------
        for (int index = 0; index < allowedUnitsSize; index++)
        {
            sprintf(formatStr, "%s %%d%%c", allowedUnits[index]);
            int intVal;
            char dummyCh;
            int i = sscanf(str, formatStr, &intVal, &dummyCh);
            if (i == 1)
            {
                unitsResult = allowedUnits[index];
                intResult = intVal;
                delete[] unitSpelling;
                delete[] formatStr;
                return;
            }
        }

        //--------
        // Incorrect format. Report an error.
        //--------
        delete[] unitSpelling;
        delete[] formatStr;
        mergeNames(scope, localName, fullyScopedName);
        msg << fileName() << ": invalid " << typeName << " ('" << str << "') specified for '"
            << fullyScopedName.str() << "': should be"
            << " '<units> <int>' where <units> are";
        for (int i = 0; i < allowedUnitsSize; i++)
        {
            msg << " '" << allowedUnits[i] << "'";
            if (i < allowedUnitsSize - 1)
            {
                msg << ",";
            }
        }
        throw ConfigurationException(msg.str());
    }

    void ConfigurationImpl::lookupUnitsWithInt(const char* scope, const char* localName, const char* typeName,
        const char** allowedUnits, int allowedUnitsSize, int& intResult, const char*& unitsResult) const

    {
        const char* str;

        str = lookupString(scope, localName);
        stringToUnitsWithInt(
            scope, localName, typeName, str, allowedUnits, allowedUnitsSize, intResult, unitsResult);
    }

    void ConfigurationImpl::lookupUnitsWithInt(const char* scope, const char* localName, const char* typeName,
        const char** allowedUnits, int allowedUnitsSize, int& intResult, const char*& unitsResult,
        int defaultInt, const char* defaultUnits) const
    {
        if (type(scope, localName) == ConfType::NoValue)
        {
            intResult = defaultInt;
            unitsResult = defaultUnits;
        }
        else
        {
            lookupIntWithUnits(scope,
                localName,
                typeName,
                allowedUnits,
                allowedUnitsSize,
                intResult,
                unitsResult,
                defaultInt,
                defaultUnits);
        }
    }

    bool ConfigurationImpl::isUnitsWithInt(
        const char* str, const char** allowedUnits, int allowedUnitsSize) const
    {
        int maxUnitsLen = 0;

        for (int index = 0; index < allowedUnitsSize; index++)
        {
            int len = strlen(allowedUnits[index]);
            if (len > maxUnitsLen)
            {
                maxUnitsLen = len;
            }
        }
        char* formatStr = new char[maxUnitsLen + 7]; // big enough
        char* unitSpelling = new char[strlen(str) + 1]; // big enough

        //--------
        // See if the string is in the form "allowedUnits[index] <int>"
        //--------
        for (int index = 0; index < allowedUnitsSize; index++)
        {
            sprintf(formatStr, "%s %%d%%c", allowedUnits[index]);
            int intVal;
            char dummyCh;
            int i = sscanf(str, formatStr, &intVal, &dummyCh);
            if (i == 1)
            {
                delete[] formatStr;
                delete[] unitSpelling;
                return true;
            }
        }

        delete[] formatStr;
        delete[] unitSpelling;
        return false;
    }

    static SpellingAndValue durationMicrosecondsUnitsInfo[] = {
        {"microsecond", 1},
        {"microseconds", 1},
        {"millisecond", 1000},
        {"milliseconds", 1000},
        {"second", 1000 * 1000},
        {"seconds", 1000 * 1000},
        {"minute", 1000 * 1000 * 60},
        {"minutes", 1000 * 1000 * 60},
    };
    static const int countDurationMicrosecondsInfo =
        sizeof(durationMicrosecondsUnitsInfo) / sizeof(durationMicrosecondsUnitsInfo[0]);

    static const char* allowedDurationMicrosecondsUnits[] = {
        "millisecond", "milliseconds", "second", "seconds", "minute", "minutes",
    };
    static const int countAllowedDurationMicrosecondsUnits =
        sizeof(allowedDurationMicrosecondsUnits) / sizeof(allowedDurationMicrosecondsUnits[0]);

    static SpellingAndValue durationMillisecondsUnitsInfo[] = {
        {"millisecond", 1},
        {"milliseconds", 1},
        {"second", 1000},
        {"seconds", 1000},
        {"minute", 1000 * 60},
        {"minutes", 1000 * 60},
        {"hour", 1000 * 60 * 60},
        {"hours", 1000 * 60 * 60},
        {"day", 1000 * 60 * 60 * 24},
        {"days", 1000 * 60 * 60 * 24},
        {"week", 1000 * 60 * 60 * 24 * 7},
        {"weeks", 1000 * 60 * 60 * 24 * 7},
    };
    static const int countDurationMillisecondsInfo =
        sizeof(durationMillisecondsUnitsInfo) / sizeof(durationMillisecondsUnitsInfo[0]);

    static const char* allowedDurationMillisecondsUnits[] = {"millisecond",
        "milliseconds",
        "second",
        "seconds",
        "minute",
        "minutes",
        "hour",
        "hours",
        "day",
        "days",
        "week",
        "weeks"};
    static const int countAllowedDurationMillisecondsUnits =
        sizeof(allowedDurationMillisecondsUnits) / sizeof(allowedDurationMillisecondsUnits[0]);

    static SpellingAndValue durationSecondsUnitsInfo[] = {
        {"second", 1},
        {"seconds", 1},
        {"minute", 60},
        {"minutes", 60},
        {"hour", 60 * 60},
        {"hours", 60 * 60},
        {"day", 60 * 60 * 24},
        {"days", 60 * 60 * 24},
        {"week", 60 * 60 * 24 * 7},
        {"weeks", 60 * 60 * 24 * 7},
    };
    static const int countDurationSecondsInfo =
        sizeof(durationSecondsUnitsInfo) / sizeof(durationSecondsUnitsInfo[0]);

    static const char* allowedDurationSecondsUnits[] = {
        "second", "seconds", "minute", "minutes", "hour", "hours", "day", "days", "week", "weeks"};
    static const int countAllowedDurationSecondsUnits =
        sizeof(allowedDurationSecondsUnits) / sizeof(allowedDurationSecondsUnits[0]);

    bool ConfigurationImpl::isDurationMicroseconds(const char* str) const
    {
        if (!strcmp(str, "infinite"))
        {
            return true;
        }
        return isFloatWithUnits(str, allowedDurationMicrosecondsUnits, countAllowedDurationMicrosecondsUnits);
    }

    bool ConfigurationImpl::isDurationMilliseconds(const char* str) const
    {
        if (!strcmp(str, "infinite"))
        {
            return true;
        }
        return isFloatWithUnits(str, allowedDurationMillisecondsUnits, countAllowedDurationMillisecondsUnits);
    }

    bool ConfigurationImpl::isDurationSeconds(const char* str) const
    {
        if (!strcmp(str, "infinite"))
        {
            return true;
        }
        return isFloatWithUnits(str, allowedDurationSecondsUnits, countAllowedDurationSecondsUnits);
    }

    static SpellingAndValue MemorySizeBytesUnitsInfo[] = {
        {"byte", 1}, {"bytes", 1}, {"KB", 1024}, {"MB", 1024 * 1024}, {"GB", 1024 * 1024 * 1024},
    };

    static SpellingAndValue MemorySizeKBUnitsInfo[] = {
        {"KB", 1}, {"MB", 1024}, {"GB", 1024 * 1024}, {"TB", 1024 * 1024 * 1024},
    };

    static SpellingAndValue MemorySizeMBUnitsInfo[] = {
        {"MB", 1}, {"GB", 1024}, {"TB", 1024 * 1024}, {"PB", 1024 * 1024 * 1024},
    };

    bool ConfigurationImpl::isMemorySizeBytes(const char* str) const
    {
        static const char* allowedUnits[] = {"byte", "bytes", "KB", "MB", "GB"};
        return isFloatWithUnits(str, allowedUnits, 5);
    }

    bool ConfigurationImpl::isMemorySizeKB(const char* str) const
    {
        static const char* allowedUnits[] = {"KB", "MB", "GB", "TB"};
        return isFloatWithUnits(str, allowedUnits, 4);
    }

    bool ConfigurationImpl::isMemorySizeMB(const char* str) const
    {
        static const char* allowedUnits[] = {"MB", "GB", "TB", "PB"};
        return isFloatWithUnits(str, allowedUnits, 4);
    }

    void ConfigurationImpl::stringToUnitsWithFloat(const char* scope, const char* localName,
        const char* typeName, const char* str, const char** allowedUnits, int allowedUnitsSize,
        float& floatResult, const char*& unitsResult) const
    {
        std::stringstream msg;
        StringBuffer fullyScopedName;

        int maxUnitsLen = 0;
        for (int index = 0; index < allowedUnitsSize; index++)
        {
            int len = strlen(allowedUnits[index]);
            if (len > maxUnitsLen)
            {
                maxUnitsLen = len;
            }
        }
        char* formatStr = new char[maxUnitsLen + 7]; // big enough
        char* unitSpelling = new char[strlen(str) + 1]; // big enough

        //--------
        // See if the string is in the form "allowedUnits[index] <float>"
        //--------
        for (int index = 0; index < allowedUnitsSize; index++)
        {
            sprintf(formatStr, "%s %%f%%c", allowedUnits[index]);
            char dummyCh;
            float fVal;
            int i = sscanf(str, formatStr, &fVal, &dummyCh);
            if (i == 1)
            {
                unitsResult = allowedUnits[index];
                floatResult = fVal;
                delete[] formatStr;
                delete[] unitSpelling;
                return;
            }
        }

        //--------
        // Incorrect format. Report an error.
        //--------
        delete[] unitSpelling;
        delete[] formatStr;
        mergeNames(scope, localName, fullyScopedName);
        msg << fileName() << ": invalid " << typeName << " ('" << str << "') specified for '"
            << fullyScopedName.str() << "': should be"
            << " '<units> <float>' where <units> are";
        for (int i = 0; i < allowedUnitsSize; i++)
        {
            msg << " '" << allowedUnits[i] << "'";
            if (i < allowedUnitsSize - 1)
            {
                msg << ",";
            }
        }
        throw ConfigurationException(msg.str());
    }

    void ConfigurationImpl::stringToFloatWithUnits(const char* scope, const char* localName,
        const char* typeName, const char* str, const char** allowedUnits, int allowedUnitsSize,
        float& floatResult, const char*& unitsResult) const
    {
        StringBuffer fullyScopedName;
        std::stringstream msg;

        // See if the string is in the form "<float> <units>"
        std::stringstream ss{str};
        float fVal;
        std::string unitSpelling;
        ss >> fVal >> unitSpelling;

        if ( ss.fail() == true )
        {
            mergeNames(scope, localName, fullyScopedName);
            msg << fileName() << ": invalid " << typeName << " ('" << str << "') specified for '"
                << fullyScopedName.str() << "': should be"
                << " '<float> <units>' where <units> are";
            for (int i = 0; i < allowedUnitsSize; ++i)
            {
                msg << " '" << allowedUnits[i] << "'";
                if (i < allowedUnitsSize - 1)
                {
                    msg << ",";
                }
            }
            throw ConfigurationException(msg.str());
        }

        // The entry appears to be in the correct format. Find out
        // what the specified units are.
        for (int i = 0; i < allowedUnitsSize; ++i)
        {
            if (strcmp(unitSpelling.c_str(), allowedUnits[i]) == 0)
            {
                // Success!
                floatResult = fVal;
                unitsResult = allowedUnits[i];
                return;
            }
        }

        // Error: an unknown unit was specified.
        mergeNames(scope, localName, fullyScopedName);
        msg << fileName() << ": invalid " << typeName << " ('" << str << "') specified for '"
            << fullyScopedName.str() << "': should be"
            << " '<float> <units>' where <units> are";
        for ( int i = 0; i < allowedUnitsSize; ++i)
        {
            msg << " '" << allowedUnits[i] << "'";
            if (i < allowedUnitsSize - 1)
            {
                msg << ",";
            }
        }
        throw ConfigurationException(msg.str());
    }

    int ConfigurationImpl::stringToDurationMicroseconds(
        const char* scope, const char* localName, const char* str) const
    {
        float floatVal;
        const char* units;
        int i;
        int result;
        int unitsVal;

        //--------
        // Is the duration "infinite"?
        //--------
        if (!strcmp(str, "infinite"))
        {
            return -1;
        }

        //--------
        // Use stringToFloatWithUnits()
        //--------
        try
        {
            stringToFloatWithUnits(scope,
                localName,
                "durationMicroseconds",
                str,
                allowedDurationMicrosecondsUnits,
                countAllowedDurationMicrosecondsUnits,
                floatVal,
                units);
        }
        catch (const ConfigurationException& ex)
        {
            std::stringstream msg;
            msg << ex.message() << "; alternatively, you can use 'infinite'";
            throw ConfigurationException(msg.str());
        }
        compat::checkAssertion(countDurationMicrosecondsInfo == countAllowedDurationMicrosecondsUnits);
        result = -1; // avoid compiler warning about an unitialized variable
        for (i = 0; i < countDurationMicrosecondsInfo; i++)
        {
            if (strcmp(durationMicrosecondsUnitsInfo[i].spelling, units) == 0)
            {
                unitsVal = durationMicrosecondsUnitsInfo[i].val;
                result = static_cast<int>(floatVal * unitsVal);
                break;
            }
        }
        compat::checkAssertion(i < countDurationMicrosecondsInfo);
        return result;
    }

    int ConfigurationImpl::stringToDurationMilliseconds(
        const char* scope, const char* localName, const char* str) const
    {
        float floatVal;
        const char* units;
        int i;
        int result;
        int unitsVal;

        //--------
        // Is the duration "infinite"?
        //--------
        if (!strcmp(str, "infinite"))
        {
            return -1;
        }

        //--------
        // Use stringToFloatWithUnits()
        //--------
        try
        {
            stringToFloatWithUnits(scope,
                localName,
                "durationMilliseconds",
                str,
                allowedDurationMillisecondsUnits,
                countAllowedDurationMillisecondsUnits,
                floatVal,
                units);
        }
        catch (const ConfigurationException& ex)
        {
            std::stringstream msg;
            msg << ex.message() << "; alternatively, you can use 'infinite'";
            throw ConfigurationException(msg.str());
        }
        compat::checkAssertion(countDurationMillisecondsInfo == countAllowedDurationMillisecondsUnits);
        result = -1; // avoid compiler warning about an unitialized variable
        for (i = 0; i < countDurationMillisecondsInfo; i++)
        {
            if (strcmp(durationMillisecondsUnitsInfo[i].spelling, units) == 0)
            {
                unitsVal = durationMillisecondsUnitsInfo[i].val;
                result = static_cast<int>(floatVal * unitsVal);
                break;
            }
        }
        compat::checkAssertion(i < countDurationMillisecondsInfo);
        return result;
    }

    int ConfigurationImpl::stringToDurationSeconds(
        const char* scope, const char* localName, const char* str) const
    {
        float floatVal;
        const char* units;
        int i;
        int result;
        int unitsVal;

        //--------
        // Is the duration "infinite"?
        //--------
        if (!strcmp(str, "infinite"))
        {
            return -1;
        }

        //--------
        // Use stringToFloatWithUnits()
        //--------
        try
        {
            stringToFloatWithUnits(scope,
                localName,
                "durationSeconds",
                str,
                allowedDurationSecondsUnits,
                countAllowedDurationSecondsUnits,
                floatVal,
                units);
        }
        catch (const ConfigurationException& ex)
        {
            std::stringstream msg;
            msg << ex.message() << "; alternatively, you can use 'infinite'";
            throw ConfigurationException(msg.str());
        }
        compat::checkAssertion(countDurationSecondsInfo == countAllowedDurationSecondsUnits);
        result = -1; // avoid compiler warning about an unitialized variable
        for (i = 0; i < countDurationSecondsInfo; i++)
        {
            if (strcmp(durationSecondsUnitsInfo[i].spelling, units) == 0)
            {
                unitsVal = durationSecondsUnitsInfo[i].val;
                result = static_cast<int>(floatVal * unitsVal);
                break;
            }
        }
        compat::checkAssertion(i < countDurationSecondsInfo);
        return result;
    }

    int ConfigurationImpl::lookupDurationMicroseconds(
        const char* scope, const char* localName, int defaultVal) const
    {
        char defaultStrValue[128]; // big enough
        const char* strValue;
        int result;

        if (defaultVal == -1)
        {
            sprintf(defaultStrValue, "infinite");
        }
        else
        {
            sprintf(defaultStrValue, "%d microseconds", defaultVal);
        }
        strValue = lookupString(scope, localName, defaultStrValue);
        result = stringToDurationMicroseconds(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupDurationMicroseconds(const char* scope, const char* localName) const

    {
        const char* strValue;
        int result;

        strValue = lookupString(scope, localName);
        result = stringToDurationMicroseconds(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupDurationMilliseconds(
        const char* scope, const char* localName, int defaultVal) const
    {
        char defaultStrValue[128]; // big enough
        const char* strValue;
        int result;

        if (defaultVal == -1)
        {
            sprintf(defaultStrValue, "infinite");
        }
        else
        {
            sprintf(defaultStrValue, "%d milliseconds", defaultVal);
        }
        strValue = lookupString(scope, localName, defaultStrValue);
        result = stringToDurationMilliseconds(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupDurationMilliseconds(const char* scope, const char* localName) const

    {
        const char* strValue;
        int result;

        strValue = lookupString(scope, localName);
        result = stringToDurationMilliseconds(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupDurationSeconds(
        const char* scope, const char* localName, int defaultVal) const
    {
        char defaultStrValue[128]; // big enough
        const char* strValue;
        int result;

        if (defaultVal == -1)
        {
            sprintf(defaultStrValue, "infinite");
        }
        else
        {
            sprintf(defaultStrValue, "%d seconds", defaultVal);
        }
        strValue = lookupString(scope, localName, defaultStrValue);
        result = stringToDurationSeconds(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupDurationSeconds(const char* scope, const char* localName) const

    {
        const char* strValue;
        int result;

        strValue = lookupString(scope, localName);
        result = stringToDurationSeconds(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::stringToMemorySizeGeneric(const char* typeName, SpellingAndValue unitsInfo[],
        int unitsInfoSize, const char* allowedUnits[], const char* scope, const char* localName,
        const char* str) const
    {
        float floatVal;
        const char* units;
        int i;
        int result;
        int unitsVal;

        stringToFloatWithUnits(scope, localName, typeName, str, allowedUnits, unitsInfoSize, floatVal, units);
        result = -1; // avoid compiler warning about an unitialized variable
        for (i = 0; i < unitsInfoSize; i++)
        {
            if (strcmp(unitsInfo[i].spelling, units) == 0)
            {
                unitsVal = unitsInfo[i].val;
                result = static_cast<int>(floatVal * unitsVal);
                break;
            }
        }
        compat::checkAssertion(i < unitsInfoSize);
        return result;
    }

    int ConfigurationImpl::stringToMemorySizeBytes(
        const char* scope, const char* localName, const char* str) const
    {
        static const char* allowedUnits[] = {"byte", "bytes", "KB", "MB", "GB"};
        return stringToMemorySizeGeneric(
            "memorySizeBytes", MemorySizeBytesUnitsInfo, 5, allowedUnits, scope, localName, str);
    }

    int ConfigurationImpl::stringToMemorySizeKB(
        const char* scope, const char* localName, const char* str) const
    {
        static const char* allowedUnits[] = {"KB", "MB", "GB", "TB"};
        return stringToMemorySizeGeneric(
            "memorySizeKB", MemorySizeKBUnitsInfo, 4, allowedUnits, scope, localName, str);
    }

    int ConfigurationImpl::stringToMemorySizeMB(
        const char* scope, const char* localName, const char* str) const
    {
        static const char* allowedUnits[] = {"MB", "GB", "TB", "PB"};
        return stringToMemorySizeGeneric(
            "memorySizeMB", MemorySizeMBUnitsInfo, 4, allowedUnits, scope, localName, str);
    }

    int ConfigurationImpl::lookupMemorySizeBytes(
        const char* scope, const char* localName, int defaultVal) const
    {
        char defaultStrValue[64]; // big enough
        const char* strValue;
        int result;

        sprintf(defaultStrValue, "%d milliseconds", defaultVal);
        strValue = lookupString(scope, localName, defaultStrValue);
        result = stringToMemorySizeBytes(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupMemorySizeBytes(const char* scope, const char* localName) const

    {
        const char* strValue;
        int result;

        strValue = lookupString(scope, localName);
        result = stringToMemorySizeBytes(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupMemorySizeKB(const char* scope, const char* localName, int defaultVal) const

    {
        char defaultStrValue[64]; // big enough
        const char* strValue;
        int result;

        sprintf(defaultStrValue, "%d KB", defaultVal);
        strValue = lookupString(scope, localName, defaultStrValue);
        result = stringToMemorySizeKB(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupMemorySizeKB(const char* scope, const char* localName) const

    {
        const char* strValue;
        int result;

        strValue = lookupString(scope, localName);
        result = stringToMemorySizeKB(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupMemorySizeMB(const char* scope, const char* localName, int defaultVal) const

    {
        char defaultStrValue[64]; // big enough
        const char* strValue;
        int result;

        sprintf(defaultStrValue, "%d MB", defaultVal);
        strValue = lookupString(scope, localName, defaultStrValue);
        result = stringToMemorySizeMB(scope, localName, strValue);
        return result;
    }

    int ConfigurationImpl::lookupMemorySizeMB(const char* scope, const char* localName) const

    {
        const char* strValue;
        int result;

        strValue = lookupString(scope, localName);
        result = stringToMemorySizeMB(scope, localName, strValue);
        return result;
    }

    float ConfigurationImpl::lookupFloat(const char* scope, const char* localName, float defaultVal) const

    {
        const char* strValue;
        float result;
        char defaultStrVal[64]; // Big enough

        sprintf(defaultStrVal, "%f", defaultVal);
        strValue = lookupString(scope, localName, defaultStrVal);
        result = stringToFloat(scope, localName, strValue);
        return result;
    }

    float ConfigurationImpl::lookupFloat(const char* scope, const char* localName) const

    {
        const char* strValue;
        float result;

        strValue = lookupString(scope, localName);
        result = stringToFloat(scope, localName, strValue);
        return result;
    }

    void ConfigurationImpl::lookupScope(const char* scope, const char* localName) const

    {
        std::stringstream msg;
        StringBuffer fullyScopedName;

        mergeNames(scope, localName, fullyScopedName);
        switch (type(scope, localName))
        {
            case ConfType::Scope:
                // Okay
                break;
            case ConfType::String:
                msg << fileName() << ": '" << fullyScopedName.str() << "' is a string instead of a scope";
                throw ConfigurationException(msg.str());
            case ConfType::List:
                msg << fileName() << ": '" << fullyScopedName.str() << "' is a list instead of a scope";
                throw ConfigurationException(msg.str());
            case ConfType::NoValue:
                msg << fileName() << ": scope '" << fullyScopedName.str() << "' does not exist";
                throw ConfigurationException(msg.str());
            default:
                throw std::exception{}; // Bug
        }
    }

    void ConfigurationImpl::pushIncludedFilename(const char* fileName)
    {
        m_fileNameStack.push_back(fileName);
    }

    void ConfigurationImpl::popIncludedFilename(const char* fileName)
    {
        unused(fileName);

        int size = m_fileNameStack.size();
        unused(size); // Prevent build failure in release builds
        compat::checkAssertion(size > 0);
        compat::checkAssertion(strcmp(m_fileNameStack[size - 1].c_str(), fileName) == 0);
        m_fileNameStack.erase(m_fileNameStack.end() - 1);
    }

    void ConfigurationImpl::checkForCircularIncludes(const char* file, int includeLineNum)
    {
        const auto size = m_fileNameStack.size();
        for (std::size_t i = 0; i < size; i++)
        {
            if (strcmp(m_fileNameStack[i].c_str(), file) == 0)
            {
                std::stringstream msg;
                msg << fileName() << ": line " << includeLineNum << ", circular include of '" << file << "'";
                throw ConfigurationException(msg.str());
            }
        }
    }

    bool ConfigurationImpl::uidEquals(const char* s1, const char* s2) const
    {
        const auto us1 = m_uidIdentifierProcessor.unexpand(s1);
        const auto us2 = m_uidIdentifierProcessor.unexpand(s2);
        return strcmp(us1.c_str(), us2.c_str()) == 0;
    }

    void ConfigurationImpl::expandUid(StringBuffer& spelling)
    {
        spelling = m_uidIdentifierProcessor.expand(spelling.str());
    }

    std::string ConfigurationImpl::unexpandUid(const char* spelling, StringBuffer& buf) const
    {
        unused(buf);
        return m_uidIdentifierProcessor.unexpand(spelling);
    }

    void ConfigurationImpl::ensureScopeExists(const char* name, ConfigScope*& scope)
    {
        StringVector vec{util::splitScopes(name)};
        ensureScopeExists(vec, 0, vec.size() - 1, scope);
    }

    void ConfigurationImpl::ensureScopeExists(const StringVector& vec, int firstIndex, int lastIndex,
        ConfigScope*& scope)
    {
        int i;
        int j;
        std::stringstream msg;

        scope = m_currScope;
        for (i = firstIndex; i <= lastIndex; i++)
        {
            if (!scope->ensureScopeExists(vec[i].c_str(), scope))
            {
                msg << fileName() << ": "
                    << "scope '";
                for (j = firstIndex; j <= i; j++)
                {
                    msg << vec[j].c_str();
                    if (j < i)
                    {
                        msg << ".";
                    }
                }
                msg << "' was previously used as a variable name";
                throw ConfigurationException(msg.str());
            }
        }
    }

    bool ConfigurationImpl::isExecAllowed(const char* cmdLine, StringBuffer& trustedCmdLine)
    {
        StringVector allowPatterns;
        StringVector denyPatterns;
        StringVector trustedDirs;
        StringBuffer cmd;
        const char* ptr;
        const char* scope;
        std::size_t i;
        std::size_t j;
        std::size_t len;

        if (this == &DefaultSecurityConfiguration::singleton || m_securityCfg == nullptr)
        {
            return false;
        }
        scope = m_securityCfgScope.str().c_str();

        m_securityCfg->lookupList(scope, "allow_patterns", allowPatterns);
        m_securityCfg->lookupList(scope, "deny_patterns", denyPatterns);
        m_securityCfg->lookupList(scope, "trusted_directories", trustedDirs);

        //--------
        // Check if there is any rule to deny execution.
        //--------
        len = denyPatterns.size();
        for (i = 0; i < len; i++)
        {
            if (patternMatch(cmdLine, denyPatterns[i].c_str()))
            {
                return false;
            }
        }

        //--------
        // Check if there is any rule to allow execution *and* the
        // command can be found in trusted_directories.
        //--------
        len = allowPatterns.size();
        for (i = 0; i < len; i++)
        {
            if (!patternMatch(cmdLine, allowPatterns[i].c_str()))
            {
                continue;
            }
            //--------
            // Found cmdLine in allow_patterns. Now extract the
            // first word from cmdLine to get the actual command.
            //--------
            cmd = "";
            ptr = cmdLine;
            while (*ptr != '\0' && !isspace(*ptr))
            {
                cmd.append(*ptr);
                ptr++;
            }

            //--------
            // Check if cmd resides in a directory in
            // trusted_directories.
            //--------
            for (j = 0; j < trustedDirs.size(); j++)
            {
                if (isCmdInDir(cmd.str().c_str(), trustedDirs[j].c_str()))
                {
                    trustedCmdLine = "";
                    trustedCmdLine << trustedDirs[j].c_str() << CONFIG4CPP_DIR_SEP << cmd
                                   << &cmdLine[strlen(cmd.str().c_str())];
                    return true;
                }
            }
        }
        return false;
    }
}
