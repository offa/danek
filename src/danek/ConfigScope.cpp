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
#include "danek/internal/UidIdentifierProcessor.h"
#include "danek/internal/ToString.h"
#include "danek/Configuration.h"
#include <algorithm>

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

    ConfigScope* ConfigScope::parentScope() const
    {
        return m_parentScope;
    }

    ConfigScope* ConfigScope::rootScope() const
    {
        const ConfigScope* scope = this;

        while (scope->m_parentScope != nullptr)
        {
            scope = scope->m_parentScope;
        }
        return (ConfigScope*) scope;
    }

    //----------------------------------------------------------------------
    // Function:	addOrReplaceString()
    //
    // Description:	Add an entry to the hash table.
    //
    // Notes:	Replaces the previous entry with the same name, if any.
    //----------------------------------------------------------------------

    bool ConfigScope::addOrReplaceString(const std::string& name, const std::string& str)
    {
        auto pos = std::find_if(m_table.begin(), m_table.end(), [&name](const auto& v) { return v->name() == name; });

        if( pos != m_table.cend() )
        {
            if( (*pos)->type() == ConfType::Scope )
            {
                return false;
            }

            delete (*pos)->m_item;
            (*pos)->m_item = new ConfigItem(name, str);
        }
        else
        {
            m_table.push_back(std::make_unique<ConfigScopeEntry>(new ConfigItem(name, str), nullptr));
        }

        return true;
    }

    //----------------------------------------------------------------------
    // Function:	addOrReplaceList()
    //
    // Description:	Add an entry to the hash table.
    //
    // Notes:	Replaces the previous entry with the same name, if any.
    //----------------------------------------------------------------------

    bool ConfigScope::addOrReplaceList(const std::string& name, const StringVector& list)
    {
        auto pos = std::find_if(m_table.begin(), m_table.end(), [&name](const auto& v) { return v->name() == name; });

        if( pos != m_table.cend() )
        {
            if( (*pos)->type() == ConfType::Scope )
            {
                return false;
            }

            delete (*pos)->m_item;
            (*pos)->m_item = new ConfigItem(name, list.get());
        }
        else
        {
            m_table.push_back(std::make_unique<ConfigScopeEntry>(new ConfigItem(name, list.get()), nullptr));
        }

        return true;
    }

    //----------------------------------------------------------------------
    // Function:	ensureScopeExists()
    //
    // Description:
    //----------------------------------------------------------------------

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

            scope = (*pos)->item()->scopeVal();
        }
        else
        {
            ConfigItem* item = new ConfigItem(name, std::make_unique<ConfigScope>(this, name));
            m_table.push_back(std::make_unique<ConfigScopeEntry>(item, nullptr));
            scope = item->scopeVal();
        }

        return true;
    }

    //----------------------------------------------------------------------
    // Function:	findItem()
    //
    // Description:	Returns the named item if it exists.
    //
    // Notes:	Returns a nil pointer on failure.
    //----------------------------------------------------------------------

    ConfigItem* ConfigScope::findItem(const std::string& name) const
    {
        auto pos = std::find_if(m_table.begin(), m_table.end(), [&name](const auto& v) { return v->name() == name; });

        if( pos != m_table.end() )
        {
            return (*pos)->m_item;
        }

        return nullptr;
    }

    //----------------------------------------------------------------------
    // Function:	findEntry()
    //
    // Description:	Returns the named entry if it exists.
    //
    // Notes:	Returns a nil pointer on failure.
    //----------------------------------------------------------------------

    ConfigScopeEntry* ConfigScope::findEntry(const std::string& name) const
    {
        auto pos = std::find_if(m_table.begin(), m_table.end(), [&name](const auto& v) { return v->name() == name; });

        if( pos != m_table.cend() )
        {
            return pos->get();
        }

        return nullptr;
    }

    bool ConfigScope::removeItem(const std::string& name)
    {
        auto pos = std::find_if(m_table.begin(), m_table.end(), [&name](const auto& v) { return v->name() == name; });

        if( pos != m_table.end() )
        {
            m_table.erase(pos);
            return true;
        }

        return false;
    }

    //----------------------------------------------------------------------
    // Function:	is_in_table()
    //
    // Description:	Returns true if the specified entry exists in the table.
    //		Returns false otherwise.
    //----------------------------------------------------------------------

    bool ConfigScope::is_in_table(const std::string& name) const
    {
        return (findEntry(name) != nullptr);
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

    void ConfigScope::listLocalNames(ConfType typeMask, StringVector& vec) const
    {
        vec.clear();
        vec.reserve(m_table.size());

        // TODO: Fix captures
        std::for_each(m_table.cbegin(), m_table.cend(), [&](const auto& v)
        {
            if( static_cast<int>(v->type()) & static_cast<int>(typeMask) )
            {
                vec.push_back(v->name());
            }
        });
    }

    //----------------------------------------------------------------------
    // Function:	listScopedNamesHelper()
    //
    // Description:
    //----------------------------------------------------------------------

    void ConfigScope::listScopedNamesHelper(const std::string& prefix, ConfType typeMask, bool recursive, const StringVector& filterPatterns, StringVector& vec) const
    {
        StringBuffer scopedName;

        vec.reserve(vec.size() + m_table.size());

        // TODO: Fix captures
        std::for_each(m_table.begin(), m_table.end(), [&](const auto& v)
        {
            scopedName = prefix;

            if( prefix.empty() == false )
            {
                scopedName.append(".");
            }
            scopedName.append(v->name());

            if ((static_cast<int>(v->type()) & static_cast<int>(typeMask)) && this->listFilter(scopedName.str().c_str(), filterPatterns))
            {
                vec.push_back(scopedName.str());
            }

            if (recursive && v->type() == ConfType::Scope)
            {
                v->item()->scopeVal()->listScopedNamesHelper(scopedName.str().c_str(), typeMask, true, filterPatterns, vec);
            }

        });
    }

    //----------------------------------------------------------------------
    // Function:	listFilter()
    //
    // Description:
    //----------------------------------------------------------------------

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
        StringVector nameVec;

        //--------
        // First pass. Dump the variables
        //--------
        listLocalNames(ConfType::Variables, nameVec);
        std::sort(nameVec.begin(), nameVec.end());

        for (std::size_t i = 0; i < nameVec.size(); ++i)
        {
            ConfigItem* item = findItem(nameVec[i].c_str());
            const auto str = toString(*item, item->name().c_str(), wantExpandedUidNames, indentLevel);
            buf << str.c_str();
        }

        //--------
        // Second pass. Dump the nested scopes
        //--------
        listLocalNames(ConfType::Scope, nameVec);
        std::sort(nameVec.begin(), nameVec.end());

        for (std::size_t i = 0; i < nameVec.size(); ++i)
        {
            ConfigItem* item = findItem(nameVec[i].c_str());
            const auto str = toString(*item, item->name().c_str(), wantExpandedUidNames, indentLevel);
            buf << str.c_str();
        }
    }
}
