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

#include "danek/internal/ConfigItem.h"
#include "danek/internal/ConfigScope.h"
#include "danek/internal/UidIdentifierProcessor.h"
#include <stdio.h>
#include <string.h>

namespace danek
{
    static char* escapeString(const char* str)
    {
        StringBuffer buf;
        char* result;

        std::size_t len = strlen(str);
        for (std::size_t i = 0; i < len; i++)
        {
            switch (str[i])
            {
                case '\t':
                    buf.append("%t");
                    break;
                case '\n':
                    buf.append("%n");
                    break;
                case '%':
                    buf.append("%%");
                    break;
                case '"':
                    buf.append("%\"");
                    break;
                default:
                    buf.append(str[i]);
                    break;
            }
        }
        result = new char[buf.length() + 1];
        strcpy(result, buf.c_str());
        return result;
    }

    static void printIndent(StringBuffer& buf, int indentLevel)
    {
        for (int i = 0; i < indentLevel; ++i)
        {
            buf.append("    ");
        }
    }

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


    //----------------------------------------------------------------------
    // Function:	dump()
    //
    // Description:	Dump out the ConfigItem's contents.
    //----------------------------------------------------------------------
    void ConfigItem::dump(StringBuffer& buf, const char* name, bool wantExpandedUidNames, int indentLevel) const
    {
        StringBuffer nameBuf;

        if (!wantExpandedUidNames)
        {
            UidIdentifierProcessor uidIdProc;
            name = uidIdProc.unexpand(name, nameBuf);
        }

        printIndent(buf, indentLevel);
        char* escStr;

        switch (m_type)
        {
            case ConfType::String:
                escStr = escapeString(m_stringVal.c_str());
                buf << name << " = \"" << escStr << "\";\n";
                delete[] escStr;
                break;
            case ConfType::List:
                {
                    buf << name << " = [";
                    std::size_t len = m_listVal.size();
                    for (std::size_t i = 0; i < len; i++)
                    {
                        escStr = escapeString(m_listVal[i].c_str());
                        buf << "\"" << escStr << "\"";
                        delete[] escStr;
                        if (i < len - 1)
                        {
                            buf << ", ";
                        }
                    }
                    buf << "];\n";
                }
                break;
            case ConfType::Scope:
                buf << name << " {\n";
                m_scope->dump(buf, wantExpandedUidNames, indentLevel + 1);
                printIndent(buf, indentLevel);
                buf << "}\n";
                break;
            default:
                assert(0); // Bug
                break;
        };
    }
}
