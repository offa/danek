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

#ifdef WIN32
#include <process.h>
#define CONFIG4CPP_OS_TYPE "windows"
#define CONFIG4CPP_DIR_SEP "\\"
#define CONFIG4CPP_PATH_SEP ";"
#define CONFIG4CPP_POPEN(fileName, mode) _popen(fileName, mode)
#define CONFIG4CPP_PCLOSE(file) _pclose(file)
#define CONFIG4CPP_DISCARD_STDERR "2> nul"
#else
#include <unistd.h>
#ifndef CONFIG4CPP_OS_TYPE
#define CONFIG4CPP_OS_TYPE "unix"
#endif
#ifndef CONFIG4CPP_DIR_SEP
#define CONFIG4CPP_DIR_SEP "/"
#endif
#ifndef CONFIG4CPP_PATH_SEP
#define CONFIG4CPP_PATH_SEP ":"
#endif
#ifndef CONFIG4CPP_POPEN
#define CONFIG4CPP_POPEN(fileName, mode) popen(fileName, mode)
#endif
#ifndef CONFIG4CPP_PCLOSE
#define CONFIG4CPP_PCLOSE(file) pclose(file)
#endif
#ifndef CONFIG4CPP_DISCARD_STDERR
#define CONFIG4CPP_DISCARD_STDERR "2> /dev/null"
#endif
#endif
#include "danek/StringBuffer.h"
#include <stdio.h>

#include <fstream>

namespace danek
{
    extern bool execCmd(const char* cmd, StringBuffer& output);
    extern bool isCmdInDir(const char* cmd, const char* dir);

}
