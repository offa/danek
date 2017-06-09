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

#include "FooConfiguration.h"
#include <memory>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

static void parseCmdLineArgs(int argc, char** argv, const char*& cfgSource, const char*& scope);
static void usage();

int main(int argc, char** argv)
{
    auto cfg = std::make_unique<FooConfiguration>();
    const char* cfgSource;
    const char* scope;

    setlocale(LC_ALL, "");
    parseCmdLineArgs(argc, argv, cfgSource, scope);

    try
    {
        //--------
        // Parse the configuration file.
        //--------
        cfg->parse(cfgSource, scope);

        //--------
        // Query the configuration object.
        //--------
        std::cout << "host = \"" << cfg->lookupString("host") << "\"\n"
                    << "port = " << cfg->lookupInt("port") << "\n"
                    << "timeout = " << cfg->lookupDurationMilliseconds("timeout")
                    << " (\"" << cfg->lookupString("timeout") << "\")\n";
    }
    catch (const FooConfigurationException& ex)
    {
        std::cerr << ex.what() << "\n";
        return 1;
    }

    return 0;
}

static void parseCmdLineArgs(int argc, char** argv, const char*& cfgSource, const char*& scope)
{
    int i;

    cfgSource = "";
    scope = "";

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            usage();
        }
        else if (strcmp(argv[i], "-cfg") == 0)
        {
            if (i == argc - 1)
            {
                usage();
            }
            cfgSource = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-scope") == 0)
        {
            if (i == argc - 1)
            {
                usage();
            }
            scope = argv[i + 1];
            i++;
        }
        else
        {
            fprintf(stderr, "Unrecognised option '%s'\n\n", argv[i]);
            usage();
        }
    }
}

static void usage()
{
    fprintf(stderr,
        "\n"
        "usage: demo <options>\n"
        "\n"
        "The <options> can be:\n"
        "  -h             Print this usage statement\n"
        "  -cfg <source>  Parse the specified configuration file\n"
        "  -scope <name>  Application scope in the configuration source\n"
        "\n"
        "A configuration <source> can be one of the following:\n"
        "  file.cfg       A configuration file\n"
        "  file#file.cfg  A configuration file\n"
        "  exec#<command> Output from executing the specified command\n\n");
    exit(1);
}
