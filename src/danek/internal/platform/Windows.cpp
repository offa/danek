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

#include "danek/internal/platform/Platform.h"
#include <array>
#include <windows.h>

namespace danek
{
    namespace platform
    {

        bool isCmdInDir(const std::string& cmd, const std::string& dir)
        {
            std::array<std::string, 3> extensions{{"", ".exe", ".bat"}};

            for (const auto& ext : extensions)
            {
                const std::string fileName = dir + directorySeparator() + cmd + ext;
                DWORD fileAttr = GetFileAttributes(fileName.c_str());

                if (fileAttr != 0xFFFFFFFF)
                {
                    return true;
                }
            }
            return false;
        }
    }
}
