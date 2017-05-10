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
#include <string.h>
#include <assert.h>
#include <stdlib.h>

namespace danek
{

    ConfigScope::ConfigScope(ConfigScope* parentScope, const std::string& name)
    {
        m_parentScope = parentScope;
        m_tableSize = 16;
        m_table = new ConfigScopeEntry[m_tableSize];
        m_numEntries = 0;

        if (m_parentScope == 0)
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
            if (m_parentScope->m_parentScope != 0)
            {
                m_scopedName.append(".");
            }
            m_scopedName.append(name);
        }
    }

    ConfigScope::~ConfigScope()
    {
        delete[] m_table;
    }

    const char* ConfigScope::scopedName() const
    {
        return m_scopedName.str().c_str();
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

    bool ConfigScope::addOrReplaceString(const char* name, const char* str)
    {
        int index;
        ConfigScopeEntry* entry = findEntry(name, index);

        if (entry != nullptr && entry->type() == ConfType::Scope)
        {
            //--------
            // Fail because there is a scope with the same name.
            //--------
            return false;
        }
        else if (entry != nullptr)
        {
            //--------
            // It already exists.
            // Replace the existing item
            //--------
            delete entry->m_item;
            entry->m_item = new ConfigItem(name, str);
        }
        else
        {
            //--------
            // It doesn't already exist.
            // Add a new entry into the list.
            //--------
            m_numEntries++;
            growIfTooFull();
            index = hash(name);
            entry = &m_table[index];
            ConfigScopeEntry* nextEntry = entry->m_next;
            ConfigScopeEntry* newEntry = new ConfigScopeEntry(name, new ConfigItem(name, str), nextEntry);
            entry->m_next = newEntry;
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

    bool ConfigScope::addOrReplaceList(const char* name, const StringVector& list)
    {
        int index;
        ConfigScopeEntry* entry = findEntry(name, index);

        if (entry && entry->type() == ConfType::Scope)
        {
            //--------
            // Fail because there is a scope with the same name.
            //--------
            return false;
        }
        else if (entry)
        {
            //--------
            // It already exists. Replace the existing item
            //--------
            delete entry->m_item;
            entry->m_item = new ConfigItem(name, list.get());
        }
        else
        {
            //--------
            // It doesn't already exist. Add a new entry into the list.
            //--------
            m_numEntries++;
            growIfTooFull();
            index = hash(name);
            entry = &m_table[index];
            ConfigScopeEntry* nextEntry = entry->m_next;
            ConfigScopeEntry* newEntry = new ConfigScopeEntry(name, new ConfigItem(name, list.get()), nextEntry);
            entry->m_next = newEntry;
        }
        return true;
    }

    //----------------------------------------------------------------------
    // Function:	ensureScopeExists()
    //
    // Description:
    //----------------------------------------------------------------------

    bool ConfigScope::ensureScopeExists(const char* name, ConfigScope*& scope)
    {
        int index;
        ConfigScopeEntry* entry = findEntry(name, index);

        if (entry && entry->type() != ConfType::Scope)
        {
            //--------
            // Fail because it already exists, but not as a scope
            //--------
            scope = nullptr;
            return false;
        }
        else if (entry)
        {
            //--------
            // It already exists.
            //--------
            scope = entry->item()->scopeVal();
            return true;
        }
        else
        {
            //--------
            // It doesn't already exist. Add a new entry into the list.
            //--------
            m_numEntries++;
            growIfTooFull();
            index = hash(name);
            entry = &m_table[index];
            ConfigScopeEntry* nextEntry = entry->m_next;
            ConfigItem* item = new ConfigItem(name, std::make_unique<ConfigScope>(this, name));
            ConfigScopeEntry* newEntry = new ConfigScopeEntry(name, item, nextEntry);
            scope = item->scopeVal();
            entry->m_next = newEntry;
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

    ConfigItem* ConfigScope::findItem(const char* name) const
    {
        ConfigItem* result = nullptr;
        int index;

        ConfigScopeEntry* entry = findEntry(name, index);

        if (entry != nullptr)
        {
            result = entry->m_item;
        }
        return result;
    }

    //----------------------------------------------------------------------
    // Function:	findEntry()
    //
    // Description:	Returns the named entry if it exists.
    //
    // Notes:	Returns a nil pointer on failure.
    //		Always returns the index (both on success and failure).
    //----------------------------------------------------------------------

    ConfigScopeEntry* ConfigScope::findEntry(const char* name, int& index) const
    {
        index = hash(name);
        ConfigScopeEntry* entry = m_table[index].m_next;

        //--------
        // Iterate over singly linked list,
        // searching for the named entry.
        //--------
        while (entry)
        {
            if (!strcmp(name, entry->name()))
            {
                //--------
                // Found it!
                //--------
                return entry;
            }
            entry = entry->m_next;
        }
        //--------
        // Not found.
        //--------
        return nullptr;
    }

    bool ConfigScope::removeItem(const char* name)
    {
        int index = hash(name);
        ConfigScopeEntry* entry = &m_table[index];
        //--------
        // Iterate over singly linked list,
        // searching for the named entry.
        //--------
        while (entry->m_next != 0)
        {
            if (!strcmp(name, entry->m_next->name()))
            {
                //--------
                // Found it!
                //--------
                ConfigScopeEntry* victim = entry->m_next;
                entry->m_next = victim->m_next;
                victim->m_next = 0;
                delete victim;
                m_numEntries--;
                return true;
            }
            entry = entry->m_next;
        }
        //--------
        // Not found.
        //--------
        return false;
    }

    //----------------------------------------------------------------------
    // Function:	is_in_table()
    //
    // Description:	Returns true if the specified entry exists in the table.
    //		Returns false otherwise.
    //----------------------------------------------------------------------

    bool ConfigScope::is_in_table(const char* name) const
    {
        int index;
        return (findEntry(name, index) != nullptr);
    }

    //----------------------------------------------------------------------
    // Function:	listLocalNames()
    //
    // Description:
    //----------------------------------------------------------------------

    void ConfigScope::listLocalNames(ConfType typeMask, StringVector& vec) const
    {
        std::size_t countWanted = 0;
        std::size_t countUnwanted = 0;

        //--------
        // Iterate over all the entries in the hash table and copy
        // their names into the StringVector
        //--------
        vec.clear();
        vec.reserve(m_numEntries);

        for (std::size_t i = 0; i < m_tableSize; ++i)
        {
            ConfigScopeEntry* entry = m_table[i].m_next;
            while (entry)
            {
                if (static_cast<int>(entry->type()) & static_cast<int>(typeMask))
                {
                    vec.push_back(entry->name());
                    ++countWanted;
                }
                else
                {
                    countUnwanted++;
                }
                entry = entry->m_next;
            }
        }
        assert(countWanted + countUnwanted == m_numEntries);
    }

    //----------------------------------------------------------------------
    // Function:	listScopedNamesHelper()
    //
    // Description:
    //----------------------------------------------------------------------

    void ConfigScope::listScopedNamesHelper(const char* prefix, ConfType typeMask, bool recursive,
        const StringVector& filterPatterns, StringVector& vec) const
    {
        StringBuffer scopedName;

        //--------
        // Iterate over all the entries in the hash table and copy
        // their locally-scoped names into the StringVector
        //--------
        vec.reserve(vec.size() + m_numEntries);
        for (std::size_t i = 0; i < m_tableSize; i++)
        {
            ConfigScopeEntry* entry = m_table[i].m_next;
            while (entry)
            {
                scopedName = prefix;
                if (prefix[0] != '\0')
                {
                    scopedName.append(".");
                }
                scopedName.append(entry->name());
                if ((static_cast<int>(entry->type()) & static_cast<int>(typeMask)) && listFilter(scopedName.str().c_str(), filterPatterns))
                {
                    vec.push_back(scopedName.str());
                }
                if (recursive && entry->type() == ConfType::Scope)
                {
                    entry->item()->scopeVal()->listScopedNamesHelper(
                        scopedName.str().c_str(), typeMask, true, filterPatterns, vec);
                }
                entry = entry->m_next;
            }
        }
    }

    //----------------------------------------------------------------------
    // Function:	listFilter()
    //
    // Description:
    //----------------------------------------------------------------------

    bool ConfigScope::listFilter(const char* name, const StringVector& filterPatterns) const
    {
        UidIdentifierProcessor uidProc;
        std::size_t len = filterPatterns.size();

        if (len == 0)
        {
            return true;
        }

        StringBuffer buf;
        const char* unexpandedName = uidProc.unexpand(name, buf);

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

        for (std::size_t i = 0; i < nameVec.size(); i++)
        {
            ConfigItem* item = findItem(nameVec[i].c_str());
            assert(static_cast<int>(item->type()) & static_cast<int>(ConfType::Variables));

            const auto str = toString(*item, item->name().c_str(), wantExpandedUidNames, indentLevel);
            buf << str.c_str();
        }

        //--------
        // Second pass. Dump the nested scopes
        //--------
        listLocalNames(ConfType::Scope, nameVec);
        std::sort(nameVec.begin(), nameVec.end());

        for (std::size_t i = 0; i < nameVec.size(); i++)
        {
            ConfigItem* item = findItem(nameVec[i].c_str());
            assert(item->type() == ConfType::Scope);
            const auto str = toString(*item, item->name().c_str(), wantExpandedUidNames, indentLevel);
            buf << str.c_str();
        }
    }

    //----------------------------------------------------------------------
    // Function:	hash()
    //
    // Description:	Hashes the name to provide an integer value.
    //----------------------------------------------------------------------

    int ConfigScope::hash(const char* name) const
    {
        int result;
        const char* p;

        result = 0;
        for (p = name; *p != '\0'; p++)
        {
            result += (*p);
        }
        result = result % m_tableSize;

        return result;
    }

    //----------------------------------------------------------------------
    // Function:	growIfTooFull()
    //
    // Description:
    //----------------------------------------------------------------------

    void ConfigScope::growIfTooFull()
    {
        int origTableSize;
        int i;
        int index;
        ConfigScopeEntry* origTable;
        ConfigScopeEntry* entry;
        ConfigScopeEntry* nextEntry;

        if (m_numEntries * 2 < m_tableSize)
        {
            return;
        }

        origTable = m_table;
        origTableSize = m_tableSize;
        m_tableSize = m_tableSize * 2;
        m_table = new ConfigScopeEntry[m_tableSize];

        for (i = 0; i < origTableSize; i++)
        {
            entry = origTable[i].m_next;
            while (entry)
            {
                index = hash(entry->name());
                nextEntry = entry->m_next;
                entry->m_next = m_table[index].m_next;
                m_table[index].m_next = entry;
                entry = nextEntry;
            }
            origTable[i].m_next = 0;
        }
        delete[] origTable;
    }
}
