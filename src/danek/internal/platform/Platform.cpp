// Copyright (c) 2017 offa
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
#include <memory>
#include <sstream>
#include <system_error>
#include <cstdio>

namespace danek
{
    namespace platform
    {

        std::string execCmd(const std::string& cmd)
        {
            constexpr std::size_t bufferSize = 128;
            std::array<char, bufferSize> buffer;
            std::ostringstream output;
            const std::string cmdStr = cmd + " 2>&1";
            std::shared_ptr<FILE> pipe(popen(cmdStr.c_str(), "r"), pclose);

            if( !pipe )
            {
                const auto errorCode = errno;
                throw std::system_error{errorCode, std::system_category()};
            }

            while( !std::feof(pipe.get()) )
            {
                if( std::fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr )
                {
                    output << buffer.data();
                }
            }

            return output.str();
        }
    }
}
