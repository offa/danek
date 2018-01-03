// Copyright (c) 2017-2018 offa
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

#include "danek/internal/ConfigItem.h"
#include "danek/internal/ConfigScope.h"
#include <sstream>
#include <regex>

namespace danek
{

    ConfigItem::ConfigItem(const std::string& name, const std::string& str) : m_type(ConfType::String),
                                                                            m_name(name),
                                                                            m_stringVal(str),
                                                                            m_listVal(),
                                                                            m_scope(nullptr)
    {
    }

    ConfigItem::ConfigItem(const std::string& name, const std::vector<std::string>& v) : m_type(ConfType::List),
                                                                            m_name(name),
                                                                            m_stringVal(""),
                                                                            m_listVal(v),
                                                                            m_scope(nullptr)
    {
    }

    ConfigItem::ConfigItem(const std::string& name, std::unique_ptr<ConfigScope> scope)
                                                                        : m_type(ConfType::Scope),
                                                                        m_name(name),
                                                                        m_stringVal(""),
                                                                        m_listVal(),
                                                                        m_scope(std::move(scope))
    {
    }


    ConfType ConfigItem::type() const
    {
        return m_type;
    }

    const std::string& ConfigItem::name() const
    {
        return m_name;
    }

    const std::string& ConfigItem::stringVal() const
    {
        checkVariantType(ConfType::String);
        return m_stringVal;
    }

    const std::vector<std::string>& ConfigItem::listVal() const
    {
        checkVariantType(ConfType::List);
        return m_listVal;
    }

    ConfigScope* ConfigItem::scopeVal() const
    {
        checkVariantType(ConfType::Scope);
        return m_scope.get();
    }

    void ConfigItem::checkVariantType(ConfType expected) const
    {
        if( m_type != expected )
        {
            throw std::domain_error{"Invalid variant type"};
        }
    }

}
