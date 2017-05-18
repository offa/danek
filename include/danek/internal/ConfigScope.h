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

#include "danek/StringVector.h"
#include "danek/StringBuffer.h"
#include "danek/ConfType.h"
#include <vector>
#include <memory>

namespace danek
{
    class ConfigItem;

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
        bool addOrReplaceList(const std::string& name, const std::vector<std::string>& list);

        bool ensureScopeExists(const std::string& name, ConfigScope*& scope);

        bool removeItem(const std::string& name);

        const ConfigItem* findItem(const std::string& name) const;

        bool contains(const std::string& name) const;

        std::vector<std::string> listFullyScopedNames(ConfType typeMask, bool recursive) const;
        std::vector<std::string> listFullyScopedNames(ConfType typeMask, bool recursive, const std::vector<std::string>& filterPatterns) const;
        std::vector<std::string> listLocallyScopedNames(ConfType typeMask, bool recursive, const std::vector<std::string>& filterPatterns) const;

        const ConfigScope* parentScope() const;
        const ConfigScope* rootScope() const;


        ConfigScope& operator=(const ConfigScope&) = delete;

        //--------
        // Debugging aids
        //--------
        void dump(StringBuffer& buf, bool wantExpandedUidNames, int indentLevel = 0) const;

    private:

        std::vector<std::string> listLocalNames(ConfType typeMask) const;
        std::vector<std::string> listScopedNamesHelper(const std::string& prefix, ConfType typeMask, bool recursive, const std::vector<std::string>& filterPatterns) const;
        bool listFilter(const std::string& name, const std::vector<std::string>& filterPatterns) const;


        const ConfigScope* m_parentScope;
        std::string m_scopedName;
        std::vector<std::unique_ptr<ConfigItem>> m_table;
    };
}
