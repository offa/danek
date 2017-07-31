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

#include "danek/internal/platform.h"
#include "danek/internal/Compat.h"
#include <stdlib.h>
#include "danek/StringBuffer.h"
#ifdef P_STDIO_HAS_LIMITED_FDS
    #ifdef WIN32
        //--------
        // Windows does NOT suffer from this problem. However, we can
        // pretend it does so we can compile and test both versions of
        // the BufferedFileReader class on Windows.
        //--------
        #include <io.h>
        #include <fcntl.h>
    #else
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <fcntl.h>
    #endif /* WIN32 */
#endif /* P_STDIO_HAS_LIMITED_FDS */

namespace danek
{
#ifdef WIN32
//--------
// Windows version.
//--------
#include <windows.h>
    bool isCmdInDir(const char* cmd, const char* dir)
    {
        StringBuffer fileName;
        static const char* extensions[] = {"", ".exe", ".bat", 0};
        int i;
        DWORD fileAttr;

        for (i = 0; extensions[i] != 0; i++)
        {
            fileName = "";
            fileName << dir << "\\" << cmd << extensions[i];
            fileAttr = GetFileAttributes(fileName.c_str());
            if (fileAttr != 0xFFFFFFFF)
            {
                return true;
            }
        }
        return false;
    }
#else
//--------
// UNIX version.
//--------
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
    bool isCmdInDir(const char* cmd, const char* dir)
    {
        StringBuffer fileName;
        struct stat sb;

        fileName << dir << "/" << cmd;
        if (stat(fileName.str().c_str(), &sb) == 0)
        {
            return true;
        }
        return false;
    }
#endif

}
