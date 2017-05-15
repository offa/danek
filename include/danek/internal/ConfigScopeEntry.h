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

#include "ConfigItem.h"
#include <assert.h>

namespace danek
{
    class ConfigScope;

    class ConfigScopeEntry
    {
    public:

        ConfigScopeEntry();
        ConfigScopeEntry(const char* name, ConfigItem* item, ConfigScopeEntry* next);
        ~ConfigScopeEntry();

        inline const char* name();
        inline const ConfigItem* item();
        inline ConfType type();
        void setItem(ConfigItem* item);

    protected:
        friend class ConfigScope;

        ConfigItem* m_item;
        ConfigScopeEntry* m_next;

    private:

        ConfigScopeEntry& operator=(const ConfigScopeEntry&);
    };

    inline const char* ConfigScopeEntry::name()
    {
        return m_item->name().c_str();
    }

    inline const ConfigItem* ConfigScopeEntry::item()
    {
        return m_item;
    }

    inline ConfType ConfigScopeEntry::type()
    {
        return m_item->type();
    }
}
