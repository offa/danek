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

#include "danek/internal/ConfigScope.h"
#include "danek/internal/ConfigItem.h"
#include "danek/internal/UidIdentifierProcessor.h"
#include "danek/internal/ToString.h"
#include "danek/Configuration.h"
#include <algorithm>
#include <sstream>

namespace danek
{

    ConfigScope::ConfigScope(ConfigScope* parentScope, const std::string& name) : m_parentScope(parentScope)
    {
        if (m_parentScope == nullptr)
        {
            if( name.empty() == false )
            {
                throw std::invalid_argument("Name is invalid on root element");
            }
            m_scopedName = "";
        }
        else
        {
            m_scopedName = m_parentScope->m_scopedName;
            if (m_parentScope->m_parentScope != nullptr)
            {
                m_scopedName.append(".");
            }
            m_scopedName.append(name);
        }
    }

    const std::string& ConfigScope::scopedName() const
    {
        return m_scopedName;
    }

    const ConfigScope* ConfigScope::parentScope() const
    {
        return m_parentScope;
    }

    const ConfigScope* ConfigScope::rootScope() const
    {
        const ConfigScope* scope = this;

        while (scope->m_parentScope != nullptr)
        {
            scope = scope->m_parentScope;
        }
        return scope;
    }

    bool ConfigScope::addOrReplaceString(const std::string& name, const std::string& str)
    {
        auto pos = std::find_if(m_table.begin(), m_table.end(), [&name](const auto& v) { return v->name() == name; });

        if( pos != m_table.cend() )
        {
            if( (*pos)->type() == ConfType::Scope )
            {
                return false;
            }

            *pos = std::make_unique<ConfigItem>(name, str);
        }
        else
        {
            m_table.push_back(std::make_unique<ConfigItem>(name, str));
        }

        return true;
    }

    bool ConfigScope::addOrReplaceList(const std::string& name, const StringVector& list)
    {
        auto pos = std::find_if(m_table.begin(), m_table.end(), [&name](const auto& v) { return v->name() == name; });

        if( pos != m_table.cend() )
        {
            if( (*pos)->type() == ConfType::Scope )
            {
                return false;
            }

            *pos = std::make_unique<ConfigItem>(name, list.get());
        }
        else
        {
            m_table.push_back(std::make_unique<ConfigItem>(name, list.get()));
        }

        return true;
    }

    bool ConfigScope::ensureScopeExists(const std::string& name, ConfigScope*& scope)
    {
        auto pos = std::find_if(m_table.begin(), m_table.end(), [&name](const auto& v) { return v->name() == name; });

        if( pos != m_table.cend() )
        {
            if( (*pos)->type() != ConfType::Scope )
            {
                scope = nullptr;
                return false;
            }

            scope = (*pos)->scopeVal();
        }
        else
        {
            auto item = std::make_unique<ConfigItem>(name, std::make_unique<ConfigScope>(this, name));
            scope = item->scopeVal();
            m_table.push_back(std::move(item));
        }

        return true;
    }

    const ConfigItem* ConfigScope::findItem(const std::string& name) const
    {
        auto pos = std::find_if(m_table.cbegin(), m_table.cend(), [&name](const auto& v) { return v->name() == name; });

        if( pos != m_table.cend() )
        {
            return pos->get();
        }

        return nullptr;
    }

    bool ConfigScope::removeItem(const std::string& name)
    {
        auto pos = std::find_if(m_table.cbegin(), m_table.cend(), [&name](const auto& v) { return v->name() == name; });

        if( pos != m_table.cend() )
        {
            m_table.erase(pos);
            return true;
        }

        return false;
    }

    bool ConfigScope::contains(const std::string& name) const
    {
        return std::find_if(m_table.cbegin(), m_table.cend(), [&name](const auto& v) { return v->name() == name; }) != m_table.cend();
    }

    void ConfigScope::listFullyScopedNames(ConfType typeMask, bool recursive, StringVector& vec) const
    {
        StringVector filterPatterns;
        listScopedNamesHelper(m_scopedName.c_str(), typeMask, recursive, filterPatterns, vec);
    }

    void ConfigScope::listFullyScopedNames(ConfType typeMask, bool recursive, const StringVector& filterPatterns, StringVector& vec) const
    {
        vec.clear();
        listScopedNamesHelper(m_scopedName.c_str(), typeMask, recursive, filterPatterns, vec);
    }

    void ConfigScope::listLocallyScopedNames(ConfType typeMask, bool recursive, const StringVector& filterPatterns, StringVector& vec) const
    {
        vec.clear();
        listScopedNamesHelper("", typeMask, recursive, filterPatterns, vec);
    }

    std::vector<std::string> ConfigScope::listLocalNames(ConfType typeMask) const
    {
        std::vector<std::string> vec;
        vec.reserve(m_table.size());

        std::for_each(m_table.cbegin(), m_table.cend(), [typeMask, &vec](const auto& v)
        {
            if( static_cast<int>(v->type()) & static_cast<int>(typeMask) )
            {
                vec.push_back(v->name());
            }
        });

        return vec;
    }

    void ConfigScope::listScopedNamesHelper(const std::string& prefix, ConfType typeMask, bool recursive, const StringVector& filterPatterns, StringVector& vec) const
    {
        vec.reserve(vec.size() + m_table.size());

        std::for_each(m_table.cbegin(), m_table.cend(), [this, typeMask, &filterPatterns, &prefix, &vec, recursive](const auto& v)
        {
            std::stringstream scopedName;
            scopedName << prefix;

            if( prefix.empty() == false )
            {
                scopedName << ".";
            }
            scopedName << v->name();

            if ((static_cast<int>(v->type()) & static_cast<int>(typeMask)) && this->listFilter(scopedName.str(), filterPatterns))
            {
                vec.push_back(scopedName.str());
            }

            if (recursive && v->type() == ConfType::Scope)
            {
                v->scopeVal()->listScopedNamesHelper(scopedName.str(), typeMask, true, filterPatterns, vec);
            }
        });
    }

    bool ConfigScope::listFilter(const std::string& name, const StringVector& filterPatterns) const
    {
        UidIdentifierProcessor uidProc;
        const std::size_t len = filterPatterns.size();

        if (len == 0)
        {
            return true;
        }

        StringBuffer buf;
        const char* unexpandedName = uidProc.unexpand(name.c_str(), buf);

        for (std::size_t i = 0; i < len; ++i)
        {
            const char* pattern = filterPatterns[i].c_str();
            if (Configuration::patternMatch(unexpandedName, pattern))
            {
                return true;
            }
        }
        return false;
    }

    //----------------------------------------------------------------------
    // Function:	dump()
    //
    // Description:	Dump the contents of the entire hash table to a file.
    //
    // Notes:	This is intended for debugging purposes.
    //----------------------------------------------------------------------

    void ConfigScope::dump(StringBuffer& buf, bool wantExpandedUidNames, int indentLevel) const
    {
        //--------
        // First pass. Dump the variables
        //--------
        auto variables = listLocalNames(ConfType::Variables);
        std::sort(variables.begin(), variables.end());

        for (std::size_t i = 0; i < variables.size(); ++i)
        {
            const ConfigItem* item = findItem(variables[i]);
            const auto str = toString(*item, item->name(), wantExpandedUidNames, indentLevel);
            buf << str;
        }

        //--------
        // Second pass. Dump the nested scopes
        //--------
        auto scopes = listLocalNames(ConfType::Scope);
        std::sort(scopes.begin(), scopes.end());

        for (std::size_t i = 0; i < scopes.size(); ++i)
        {
            const ConfigItem* item = findItem(scopes[i].c_str());
            const auto str = toString(*item, item->name(), wantExpandedUidNames, indentLevel);
            buf << str;
        }
    }
}
