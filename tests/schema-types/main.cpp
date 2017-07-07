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

#include "danek/Configuration.h"
#include "danek/SchemaValidator.h"
#include "danek/PatternMatch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

using namespace danek;

static void parseCmdLineArgs(int argc, char** argv, const char*& cfgFile, bool& wantDiagnostics);
static void usage();

int main(int argc, char** argv)
{
    Configuration* cfg = Configuration::create();
    const char* cfgFile;
    const char* exPattern = nullptr;
    bool wantDiagnostics;
    StringBuffer buf;
    StringVector testSchema;
    StringVector goodScopes;
    StringVector badScopes;
    SchemaValidator fileSv;
    SchemaValidator testSv;
    std::size_t passedCount = 0;

    setlocale(LC_ALL, "");
    parseCmdLineArgs(argc, argv, cfgFile, wantDiagnostics);

    //--------
    // Parse and validate the configuration file. Then get the testSchema
    // and lists of the names of good and bad sub-scopes.
    //--------
    try
    {
        const char* fileSchema[] = {
            "testSchema = list[string]",
            "good = scope",
            "bad = scope",
            "@ignoreScopesIn good",
            "@ignoreScopesIn bad",
            0 // null-terminated array
        };
        cfg->parse(cfgFile);
        fileSv.parseSchema(fileSchema);
        fileSv.validate(cfg, "", "");

        cfg->lookupList("testSchema", "", testSchema);
        cfg->listFullyScopedNames("good", "", ConfType::Scope, false, goodScopes);
        cfg->listFullyScopedNames("bad", "", ConfType::Scope, false, badScopes);
        testSv.wantDiagnostics(wantDiagnostics);

        // Deprecated conversion
        std::vector<const char*> v;
        v.reserve(testSchema.size());

        for( const auto& s : testSchema.get() )
        {
            v.push_back(s.c_str());
        }

        testSv.parseSchema(v.data());
    }
    catch (const ConfigurationException& ex)
    {
        printf("%s\n", ex.message().c_str());
        return 1;
    }

    //--------
    // Schema validation should succeed for every sub-scope withinin "good".
    //--------
    for ( std::size_t i = 0; i < goodScopes.size(); ++i)
    {
        try
        {
            testSv.validate(cfg, goodScopes[i].c_str(), "");
            passedCount++;
        }
        catch (const ConfigurationException& ex)
        {
            try
            {
                cfg->dump(buf, true, goodScopes[i].c_str(), "");
            }
            catch (const ConfigurationException& ex2)
            {
                throw; // Bug!
            }
            printf("\n\n--------\n");
            printf("%s\n\n%s--------\n\n", ex.message().c_str(), buf.str().c_str());
        }
    }

    //--------
    // Schema validation should fail for every sub-scope within "bad".
    //--------
    for ( std::size_t i = 0; i < badScopes.size(); ++i)
    {
        try
        {
            exPattern = cfg->lookupString(badScopes[i].c_str(), "exception");
            testSv.validate(cfg, badScopes[i].c_str(), "");
            try
            {
                cfg->dump(buf, true, badScopes[i].c_str(), "");
            }
            catch (const ConfigurationException& ex2)
            {
                throw; // Bug!
            }
            printf("\n\n--------\n");
            printf("Validation succeeded for scope '%s'\n%s--------\n\n", badScopes[i].c_str(), buf.str().c_str());
        }
        catch (const ConfigurationException& ex)
        {
            if (patternMatch(ex.message().c_str(), exPattern))
            {
                ++passedCount;
            }
            else
            {
                printf("\n\n--------\n");
                printf("Unexpected exception for scope \"%s\"\n"
                       "Pattern \"%s\" does not match "
                       "exception\n\"%s\"\n--------\n\n",
                    badScopes[i].c_str(),
                    exPattern,
                    ex.message().c_str());
            }
        }
    }

    const auto totalCount = goodScopes.size() + badScopes.size();
    printf("\n%d tests out of %d passed\n\n", static_cast<int>(passedCount), static_cast<int>(totalCount));

    cfg->destroy();
    return static_cast<int>(passedCount != totalCount);
}

static void parseCmdLineArgs(int argc, char** argv, const char*& cfgFile, bool& wantDiagnostics)
{
    cfgFile = 0;
    wantDiagnostics = false;

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            usage();
        }
        else if (strcmp(argv[i], "-diagnostics") == 0)
        {
            wantDiagnostics = true;
        }
        else if (strcmp(argv[i], "-nodiagnostics") == 0)
        {
            wantDiagnostics = false;
        }
        else if (strcmp(argv[i], "-cfg") == 0)
        {
            if (i == argc - 1)
            {
                usage();
            }
            cfgFile = argv[i + 1];
            ++i;
        }
        else
        {
            fprintf(stderr, "\nUnrecognised option '%s'\n", argv[i]);
            usage();
        }
    }
    if (cfgFile == 0)
    {
        fprintf(stderr, "\nYou must specify '-cfg <file.cfg>'\n");
        usage();
    }
}

static void usage()
{
    fprintf(stderr,
        "\n"
        "usage: schema-testsuite <options>\n"
        "\n"
        "The <options> can be:\n"
        "  -h               Print this usage statement\n"
        "  -cfg <file.cfg>  Parse specified configuration file\n"
        "  -diagnostics     Prints diagnostics from schema validator\n"
        "  -nodiagnostics   No diagnostics (default)\n"
        "\n");
    exit(1);
}
