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

#pragma once

//--------
// #include's
//--------
#include "danek/ConfigurationException.h"
#include "danek/StringVector.h"
#include "danek/StringBuffer.h"
#include "ConfigScopeEntry.h"
#include <vector>

namespace danek
{
    //----------------------------------------------------------------------
    // Class:	ConfigScope
    //
    // Description:	A hash table for storing (name, item) pairs.
    //----------------------------------------------------------------------

    class ConfigScope
    {
    public:

        ConfigScope(ConfigScope* parentScope, const std::string& name);
        ConfigScope(const ConfigScope&) = delete;

        const std::string& scopedName() const;

        bool addOrReplaceString(const std::string& name, const std::string& str);
        bool addOrReplaceList(const std::string& name, const StringVector& list);

        bool ensureScopeExists(const std::string& name, ConfigScope*& scope);

        bool removeItem(const std::string& name);

        ConfigItem* findItem(const std::string& name) const;

        bool contains(const std::string& name) const;

        void listFullyScopedNames(ConfType typeMask, bool recursive, StringVector& vec) const;
        void listFullyScopedNames(ConfType typeMask, bool recursive, const StringVector& filterPatterns, StringVector& vec) const;
        void listLocallyScopedNames(ConfType typeMask, bool recursive, const StringVector& filterPatterns, StringVector& vec) const;

        ConfigScope* parentScope() const;
        ConfigScope* rootScope() const;


        ConfigScope& operator=(const ConfigScope&) = delete;

        //--------
        // Debugging aids
        //--------
        void dump(StringBuffer& buf, bool wantExpandedUidNames, int indentLevel = 0) const;

    private:

        void listLocalNames(ConfType typeMask, StringVector& vec) const;
        void listScopedNamesHelper(const std::string& prefix, ConfType typeMask, bool recursive, const StringVector& filterPatterns, StringVector& vec) const;
        bool listFilter(const std::string& name, const StringVector& filterPatterns) const;


        ConfigScope* m_parentScope;
        std::string m_scopedName;
        std::vector<std::unique_ptr<ConfigScopeEntry>> m_table;
    };
}
