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

#include "danek/Configuration.h"
#include <string>
#include <assert.h>

namespace danek
{
    class ConfigScope;

    //--------------------------------------------------------------
    // Class:	ConfigItem
    //
    // Description:
    //		A config file contains "name = <value>" statements and
    //		"name <scope>" statements. This class is used to store
    //		name plus the the <value> part, (which can be a string
    //		or a sequence of string) or a <scope>.
    //--------------------------------------------------------------

    class ConfigItem
    {
    public:

        ConfigItem(const std::string& name, const std::string& str);
        [[deprecated]]
        ConfigItem(const std::string& name, const StringVector& list);
        ConfigItem(const std::string& name, const std::vector<std::string>& v);
        ConfigItem(const std::string& name, ConfigScope* scope);
        ConfigItem(const ConfigItem&) = delete;
        virtual ~ConfigItem();


        Configuration::Type type() const
        {
            return m_type;
        }

        const std::string& name() const
        {
            return m_name;
        }

        const std::string& stringVal() const
        {
            assert(m_type == Configuration::Type::String);
            return m_stringVal;
        }

        const std::vector<std::string>& listVal() const
        {
            assert(m_type == Configuration::Type::List);
            return m_listVal;
        }

        ConfigScope* scopeVal() const
        {
            assert(m_type == Configuration::Type::Scope);
            assert(m_scope != 0);
            return m_scope;
        }


        ConfigItem& operator=(const ConfigItem&) = delete;

        //--------
        // Debugging aid
        //--------
        void dump(StringBuffer& buf, const char* name, bool wantExpandedUidNames, int indentLevel = 0) const;


    private:

        Configuration::Type m_type;
        const std::string m_name;
        const std::string m_stringVal;
        std::vector<std::string> m_listVal;
        ConfigScope* m_scope;

    };
}
