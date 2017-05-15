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

//--------
// #include's
//--------
#include "danek/Configuration.h"
#include "danek/StringBuffer.h"
#include "danek/SchemaValidator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <locale.h>

using danek::Configuration;
using danek::ConfigurationException;
using danek::SchemaValidator;
using danek::StringBuffer;
using danek::StringVector;
using danek::ConfType;

//--------
// Forward declarations
//--------
static void usage(const char* optMsg);

static void parseCmdLineArgs(int argc, char** argv, const char*& cmd, bool& isRecursive,
    bool& wantExpandedUidNames, StringVector& filterPatterns, const char*& scope, const char*& name,
    const char*& cfgSource, const char*& secSource, const char*& secScope, const char*& schemaSource,
    const char*& schemaName, SchemaValidator::ForceMode& forceMode, bool& wantDiagnostics,
    ConfType& types, Configuration* cfg);

static ConfType stringToTypes(const char* str);

int main(int argc, char** argv)
{
    const char* cmd;
    bool isRecursive;
    bool wantExpandedUidNames;
    StringVector filterPatterns;
    const char* scope;
    const char* name;
    const char* cfgSource;
    const char* secSource;
    const char* secScope;
    const char* schemaSource;
    const char* schemaName;
    bool wantDiagnostics;
    const char* str;
    Configuration* cfg;
    Configuration* secCfg;
    Configuration* schemaCfg;
    const Configuration* secDumpCfg;
    const char* secDumpScope;
    std::size_t len;
    SchemaValidator::ForceMode forceMode;
    std::size_t i;
    StringBuffer buf;
    StringVector names;
    StringBuffer fullyScopedName;
    ConfType types;
    SchemaValidator sv;

    setlocale(LC_ALL, "");

    cfg = Configuration::create();
    secCfg = Configuration::create();
    schemaCfg = Configuration::create();

    parseCmdLineArgs(argc,
        argv,
        cmd,
        isRecursive,
        wantExpandedUidNames,
        filterPatterns,
        scope,
        name,
        cfgSource,
        secSource,
        secScope,
        schemaSource,
        schemaName,
        forceMode,
        wantDiagnostics,
        types,
        cfg);

    try
    {
        if (secSource != nullptr)
        {
            secCfg->parse(secSource);
            cfg->setSecurityConfiguration(secCfg, secScope);
        }
        cfg->parse(cfgSource);
    }
    catch (const ConfigurationException& ex)
    {
        printf("%s\n", ex.c_str());
        exit(1);
    }
    cfg->mergeNames(scope, name, fullyScopedName);

    if (strcmp(cmd, "parse") == 0)
    {
        //--------
        // Nothing else to do
        //--------
    }
    else if (strcmp(cmd, "validate") == 0)
    {
        try
        {

            schemaCfg->parse(schemaSource);
            std::vector<std::string> outputData;
            schemaCfg->lookupList(schemaName, "", outputData);
            sv.wantDiagnostics(wantDiagnostics);

            std::vector<const char*> schemaVec; // Deprecated conversion; kept for compatibility

            for( const auto& s : outputData )
            {
                schemaVec.push_back(&s.front());
            }

            sv.parseSchema(schemaVec.data(), schemaVec.size());
            sv.validate(cfg, scope, name, isRecursive, types, forceMode);
        }
        catch (ConfigurationException& ex)
        {
            fprintf(stderr, "%s\n", ex.c_str());
        }
    }
    else if (strcmp(cmd, "slist") == 0)
    {
        try
        {
            cfg->listFullyScopedNames(scope, name, types, isRecursive, filterPatterns, names);
            len = names.size();
            for (i = 0; i < len; i++)
            {
                printf("%s\n", names[i].c_str());
            }
        }
        catch (ConfigurationException& ex)
        {
            fprintf(stderr, "%s\n", ex.c_str());
        }
    }
    else if (strcmp(cmd, "llist") == 0)
    {
        try
        {
            cfg->listLocallyScopedNames(scope, name, types, isRecursive, filterPatterns, names);
            len = names.size();
            for (i = 0; i < len; i++)
            {
                printf("%s\n", names[i].c_str());
            }
        }
        catch (ConfigurationException& ex)
        {
            fprintf(stderr, "%s\n", ex.c_str());
        }
    }
    else if (strcmp(cmd, "type") == 0)
    {
        switch (cfg->type(scope, name))
        {
            case ConfType::String:
                printf("string\n");
                break;
            case ConfType::List:
                printf("list\n");
                break;
            case ConfType::Scope:
                printf("scope\n");
                break;
            case ConfType::NoValue:
                printf("no_value\n");
                break;
            default:
                assert(0); // Bug!
                break;
        }
    }
    else if (strcmp(cmd, "print") == 0)
    {
        try
        {
            switch (cfg->type(scope, name))
            {
                case ConfType::String:
                    str = cfg->lookupString(scope, name);
                    printf("%s\n", str);
                    break;
                case ConfType::List:
                    {
                        std::vector<std::string> vec;
                        cfg->lookupList(scope, name, vec);
                        for (i = 0; i < vec.size(); i++)
                        {
                            printf("%s\n", vec[i].c_str());
                        }
                    }
                    break;
                case ConfType::Scope:
                    fprintf(stderr, "'%s' is a scope\n", fullyScopedName.str().c_str());
                    break;
                case ConfType::NoValue:
                    fprintf(stderr, "'%s' does not exist\n", fullyScopedName.str().c_str());
                    break;
                default:
                    assert(0); // Bug!
                    break;
            }
        }
        catch (const ConfigurationException& ex)
        {
            fprintf(stderr, "%s\n", ex.c_str());
        }
    }
    else if (strcmp(cmd, "dumpSec") == 0)
    {
        try
        {
            cfg->getSecurityConfiguration(secDumpCfg, secDumpScope);
            secDumpCfg->dump(buf, wantExpandedUidNames, secDumpScope, "allow_patterns");
            printf("%s", buf.str().c_str());
            secDumpCfg->dump(buf, wantExpandedUidNames, secDumpScope, "deny_patterns");
            printf("%s", buf.str().c_str());
            secDumpCfg->dump(buf, wantExpandedUidNames, secDumpScope, "trusted_directories");
            printf("%s", buf.str().c_str());
        }
        catch (const ConfigurationException& ex)
        {
            fprintf(stderr, "%s\n", ex.c_str());
        }
    }
    else if (strcmp(cmd, "dump") == 0)
    {
        try
        {
            cfg->dump(buf, wantExpandedUidNames, scope, name);
            printf("%s", buf.str().c_str());
        }
        catch (const ConfigurationException& ex)
        {
            fprintf(stderr, "%s\n", ex.c_str());
        }
    }
    else
    {
        assert(0); // Bug!
    }

    //--------
    // Terminate gracefully
    //--------
    cfg->destroy();
    secCfg->destroy();
    schemaCfg->destroy();
    return 0;
}

static void parseCmdLineArgs(int argc, char** argv, const char*& cmd, bool& isRecursive,
    bool& wantExpandedUidNames, StringVector& filterPatterns, const char*& scope, const char*& name,
    const char*& cfgSource, const char*& secSource, const char*& secScope, const char*& schemaSource,
    const char*& schemaName, SchemaValidator::ForceMode& forceMode, bool& wantDiagnostics,
    ConfType& types, Configuration* cfg)
{
    int i;
    StringBuffer msg;

    cmd = nullptr;
    wantExpandedUidNames = true;
    scope = "";
    name = "";
    cfgSource = nullptr;
    secSource = nullptr;
    secScope = "";
    schemaSource = nullptr;
    schemaName = nullptr;
    wantDiagnostics = false;
    forceMode = SchemaValidator::ForceMode::None;
    isRecursive = true;
    types = ConfType::ScopesAndVars;
    filterPatterns.clear();

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            usage("");
        }
        else if (strcmp(argv[i], "-set") == 0)
        {
            if (i >= argc - 2)
            {
                usage("");
            }
            cfg->insertString("", argv[i + 1], argv[i + 2]);
            i += 2;
        }
        else if (strcmp(argv[i], "-cfg") == 0)
        {
            if (i == argc - 1)
            {
                usage("");
            }
            cfgSource = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-secCfg") == 0)
        {
            if (i == argc - 1)
            {
                usage("");
            }
            secSource = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-secScope") == 0)
        {
            if (i == argc - 1)
            {
                usage("");
            }
            secScope = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-schemaCfg") == 0)
        {
            if (i == argc - 1)
            {
                usage("");
            }
            schemaSource = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-schema") == 0)
        {
            if (i == argc - 1)
            {
                usage("");
            }
            schemaName = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-diagnostics") == 0)
        {
            wantDiagnostics = true;
        }
        else if (strcmp(argv[i], "-force_optional") == 0)
        {
            forceMode = SchemaValidator::ForceMode::Optional;
        }
        else if (strcmp(argv[i], "-force_required") == 0)
        {
            forceMode = SchemaValidator::ForceMode::Required;
        }
        else if (strcmp(argv[i], "-nodiagnostics") == 0)
        {
            wantDiagnostics = true;
        }
        else if (strcmp(argv[i], "-types") == 0)
        {
            if (i == argc - 1)
            {
                usage("");
            }
            types = stringToTypes(argv[i + 1]);
            i++;
            //--------
            // Commands
            //--------
        }
        else if (strcmp(argv[i], "parse") == 0)
        {
            cmd = argv[i];
        }
        else if (strcmp(argv[i], "slist") == 0)
        {
            cmd = argv[i];
        }
        else if (strcmp(argv[i], "llist") == 0)
        {
            cmd = argv[i];
        }
        else if (strcmp(argv[i], "dump") == 0)
        {
            cmd = argv[i];
        }
        else if (strcmp(argv[i], "dumpSec") == 0)
        {
            cmd = argv[i];
        }
        else if (strcmp(argv[i], "type") == 0)
        {
            cmd = argv[i];
        }
        else if (strcmp(argv[i], "print") == 0)
        {
            cmd = argv[i];
        }
        else if (strcmp(argv[i], "validate") == 0)
        {
            cmd = argv[i];
            //--------
            // Arguments to commands
            //--------
        }
        else if (strcmp(argv[i], "-scope") == 0)
        {
            if (i == argc - 1)
            {
                usage("");
            }
            scope = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-filter") == 0)
        {
            if (i == argc - 1)
            {
                usage("");
            }
            filterPatterns.push_back(argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "-name") == 0)
        {
            if (i == argc - 1)
            {
                usage("");
            }
            name = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-recursive") == 0)
        {
            isRecursive = true;
        }
        else if (strcmp(argv[i], "-norecursive") == 0)
        {
            isRecursive = false;
        }
        else if (strcmp(argv[i], "-expandUid") == 0)
        {
            wantExpandedUidNames = true;
        }
        else if (strcmp(argv[i], "-unexpandUid") == 0)
        {
            wantExpandedUidNames = false;
        }
        else
        {
            usage(argv[i]);
        }
    }
    if (cfgSource == nullptr)
    {
        fprintf(stderr, "\nYou must specify -cfg <source>\n\n");
        usage("");
    }
    if (cmd == nullptr)
    {
        fprintf(stderr, "\nYou must specify a command\n\n");
        usage("");
    }
    if (strcmp(cmd, "validate") == 0)
    {
        if (schemaSource == nullptr)
        {
            fprintf(stderr,
                "\nThe validate command requires "
                "-schemaCfg <source>\n\n");
            usage("");
        }
        if (schemaName == nullptr)
        {
            fprintf(stderr,
                "\nThe validate command requires "
                "-schema <full.name>\n\n");
            usage("");
        }
    }
}

static ConfType stringToTypes(const char* str)
{
    if (strcmp(str, "string") == 0)
    {
        return ConfType::String;
    }
    else if (strcmp(str, "list") == 0)
    {
        return ConfType::List;
    }
    else if (strcmp(str, "scope") == 0)
    {
        return ConfType::Scope;
    }
    else if (strcmp(str, "variables") == 0)
    {
        return ConfType::Variables;
    }
    else if (strcmp(str, "scope_and_vars") == 0)
    {
        return ConfType::ScopesAndVars;
    }
    usage("Invalid value for '-types <...>'");
    return ConfType::String; // Not reached; keep compiler happy
}

static void usage(const char* optMsg)
{
    StringBuffer msg;

    if (strcmp(optMsg, "") != 0)
    {
        msg << optMsg << "\n\n";
    }

    msg << "usage: config4cpp -cfg <source> <command> <options>\n"
        << "\n"
        << "<command> can be one of the following:\n"
        << "  parse               Parse and report errors, if any\n"
        << "  validate            Validate <scope>.<name>\n"
        << "  dump                Dump <scope>.<name>\n"
        << "  dumpSec             Dump the security policy\n"
        << "  print               Print value of the <scope>.<name> "
        << "variable\n"
        << "  type                Print type of the <scope>.<name> entry\n"
        << "  slist               List scoped names in <scope>.<name>\n"
        << "  llist               List local names in <scope>.<name>\n"
        << "\n"
        << "<options> can be:\n"
        << "  -h                  Print this usage statement\n"
        << "  -set <name> <value> Preset name=value in configuration object\n"
        << "  -scope <scope>      Specify <scope> argument for commands\n"
        << "  -name <name>        Specify <name> argument for commands\n"
        << "\n"
        << "  -secCfg <source>    Override default security policy\n"
        << "  -secScope <scope>   Scope for security policy\n"
        << "\n"
        << "  -schemaCfg <source> Source that contains a schema\n"
        << "  -schema <full.name> Name of schema in '-schemaCfg <source>'\n"
        << "  -force_optional     Validate with forceMode = "
        << "FORCE_OPTIONAL\n"
        << "  -force_required     Validate with forceMode = "
        << "FORCE_REQUIRED\n"
        << "  -diagnostics        Print diagnostics during schema "
        << "validation\n"
        << "  -nodiagnostics      Do not print diagnostics during schema "
        << "validation (default)\n"
        << "\n"
        << "  -recursive          For llist, slist and validate (default)\n"
        << "  -norecursive        For llist, slist and validate\n"
        << "  -filter <pattern>   A filter pattern for slist and llist\n"
        << "  -types <types>      For llist, slist and validate\n"
        << "\n"
        << "  -expandUid          For dump (default)\n"
        << "  -unexpandUid        For dump\n"
        << "\n"
        << "<types> can be one of the following:\n"
        << "  string, list, scope, variables, scope_and_vars (default)\n"
        << "\n"
        << "<source> can be one of the following:\n"
        << "  file.cfg       A configuration file\n"
        << "  file#file.cfg  A configuration file\n"
        << "  exec#<command> Output from executing the specified command\n";
    fprintf(stderr, "%s", msg.str().c_str());
    exit(1);
}
