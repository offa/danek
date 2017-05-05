// Copyright (c) 2017 offa
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

#include "danek/internal/ToString.h"
#include "danek/internal/ConfigItem.h"
#include "danek/internal/ConfigScope.h"
#include "danek/internal/UidIdentifierProcessor.h"
#include <sstream>
#include <regex>

namespace danek
{

    namespace
    {
        std::string indent(std::size_t level)
        {
            constexpr std::size_t indentCount = 4;
            return std::string(level * indentCount, ' ');
        }

        std::string escape(const std::string& str)
        {
            std::string output = str;

            output = std::regex_replace(output, std::regex("%"), "%%");
            output = std::regex_replace(output, std::regex("\t"), "%t");
            output = std::regex_replace(output, std::regex("\n"), "%n");
            output = std::regex_replace(output, std::regex("\""), "%\"");

            return output;
        }


        std::string expandUid(const std::string& name, bool expand)
        {
            if( expand == false )
            {
                UidIdentifierProcessor uidIdProc;
                StringBuffer nameBuf;
                return uidIdProc.unexpand(name.c_str(), nameBuf);
            }
            return name;
        }

    }


    std::string toString(const ConfigItem& item, const std::string& name, bool expandUidNames, std::size_t indentLevel)
    {
        const std::string nameStr = expandUid(name, expandUidNames);
        std::stringstream os;

        os << indent(indentLevel);

        switch(item.type())
        {
            case ConfType::String:
                os << nameStr << " = \"" << escape(item.stringVal()) << "\";\n";
                break;
            case ConfType::List:
                {
                    os << nameStr << " = [";
                    const auto values = item.listVal();

                    if( values.empty() == false )
                    {
                        os << "\"" << *values.cbegin() << "\"";

                        std::for_each(std::next(values.cbegin()), values.cend(), [&os](const auto& v)
                        {
                            os << ", \"" << escape(v) << "\"";
                        });
                    }

                    os << "];\n";
                }
                break;
            case ConfType::Scope:
                {
                    os << nameStr << " {\n";
                    StringBuffer buffer;
                    item.scopeVal()->dump(buffer, expandUidNames, indentLevel + 1);
                    os << indent(indentLevel) << "}\n";
                }
                break;
            default:
                break;
        }

        return os.str();
    }

}
