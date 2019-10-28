// Copyright (c) 2017-2019 offa
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
#include <algorithm>
#include <array>
#include <iterator>
#include <sstream>

namespace danek
{

    namespace
    {
        const std::array<std::pair<std::string, std::string>, 4> escapeSequences{
            {{"%", "%%"}, {"\t", "%t"}, {"\n", "%n"}, {"\"", "%\""}}};

        std::string indent(std::size_t level)
        {
            constexpr std::size_t indentCount{4};
            return std::string(level * indentCount, ' ');
        }


        void replaceInplace(std::string& input, const std::string& str, const std::string& replacement)
        {
            if (str.empty() == false)
            {
                std::size_t pos = 0;

                while ((pos = input.find(str, pos)) != std::string::npos)
                {
                    input.replace(pos, str.size(), replacement);
                    pos += replacement.size();
                }
            }
        }


        std::string escape(const std::string& str)
        {
            auto output = str;

            std::for_each(escapeSequences.cbegin(), escapeSequences.cend(),
                          [&output](const auto& v) { replaceInplace(output, v.first, v.second); });

            return "\"" + output + "\"";
        }


        std::string expandUid(const std::string& name, bool expand)
        {
            if (expand == false)
            {
                UidIdentifierProcessor uidIdProc;
                return uidIdProc.unexpand(name);
            }
            return name;
        }

        void appendConfType(std::stringstream& stream, const ConfigScope& scope, ConfType type, bool expandUid,
                            std::size_t indentLevel)
        {
            auto names = scope.listLocallyScopedNames(type, false, {});
            std::sort(names.begin(), names.end());

            std::for_each(names.cbegin(), names.cend(), [&stream, &scope, expandUid, indentLevel](const auto s) {
                const auto item = scope.findItem(s);
                stream << toString(*item, item->name(), expandUid, indentLevel);
            });
        }
    }


    std::string toString(const ConfigItem& item, const std::string& name, bool expandUidNames, std::size_t indentLevel)
    {
        const std::string nameStr = expandUid(name, expandUidNames);
        std::stringstream os;

        os << indent(indentLevel);

        switch (item.type())
        {
            case ConfType::String:
                os << nameStr << " = " << escape(item.stringVal()) << ";\n";
                break;
            case ConfType::List:
            {
                os << nameStr << " = [";
                const auto values = item.listVal();

                if (values.empty() == false)
                {
                    using OItr = std::ostream_iterator<std::string>;
                    std::transform(values.cbegin(), std::prev(values.cend()), OItr{os, ", "}, escape);
                    std::transform(std::prev(values.cend()), values.cend(), OItr{os}, escape);
                }

                os << "];\n";
            }
            break;
            case ConfType::Scope:
            {
                os << nameStr << " {\n"
                   << toString(*(item.scopeVal()), expandUidNames, indentLevel + 1) << indent(indentLevel) << "}\n";
            }
            break;
            default:
                break;
        }

        return os.str();
    }


    std::string toString(const ConfigScope& scope, bool expandUidNames, std::size_t indentLevel)
    {
        std::stringstream ss;
        appendConfType(ss, scope, ConfType::Variables, expandUidNames, indentLevel);
        appendConfType(ss, scope, ConfType::Scope, expandUidNames, indentLevel);

        return ss.str();
    }
}
