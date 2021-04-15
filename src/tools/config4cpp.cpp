// Copyright (c) 2017-2021 offa
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
#include "danek/StringBuffer.h"
#include <iostream>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

using danek::Configuration;
using danek::ConfigurationException;
using danek::ConfType;
using danek::SchemaValidator;
using danek::StringBuffer;
using danek::StringVector;

namespace
{
    template <class Container>
    void printElements(const Container& c)
    {
        std::for_each(std::cbegin(c), std::cend(c), [](const auto& element) { std::cout << element << "\n"; });
    }
}

static void usage(const std::string& optMsg);

static void parseCmdLineArgs(int argc, char** argv, const char*& cmd, bool& isRecursive, bool& wantExpandedUidNames,
                             StringVector& filterPatterns, const char*& scope, const char*& name, const char*& cfgSource,
                             const char*& secSource, const char*& secScope, const char*& schemaSource, const char*& schemaName,
                             SchemaValidator::ForceMode& forceMode, bool& wantDiagnostics, ConfType& types, Configuration* cfg);

static ConfType stringToTypes(const char* str);

int main(int argc, char** argv)
{
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
    const Configuration* secDumpCfg;
    const char* secDumpScope;
    SchemaValidator::ForceMode forceMode;
    StringBuffer buf;
    StringVector names;
    StringBuffer fullyScopedName;
    ConfType types;
    SchemaValidator sv;

    setlocale(LC_ALL, "");

    Configuration* cfg = Configuration::create();
    Configuration* secCfg = Configuration::create();
    Configuration* schemaCfg = Configuration::create();

    const std::string cmd = [&] {
        const char* cmdStr;
        parseCmdLineArgs(argc, argv, cmdStr, isRecursive, wantExpandedUidNames, filterPatterns, scope, name, cfgSource, secSource,
                         secScope, schemaSource, schemaName, forceMode, wantDiagnostics, types, cfg);
        return std::string{cmdStr};
    }();

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
        std::cerr << ex.what() << "\n";
        throw;
    }
    cfg->mergeNames(scope, name, fullyScopedName);

    if (cmd == "parse")
    {
        //--------
        // Nothing else to do
        //--------
    }
    else if (cmd == "validate")
    {
        try
        {

            schemaCfg->parse(schemaSource);
            std::vector<std::string> outputData;
            schemaCfg->lookupList(schemaName, "", outputData);
            sv.wantDiagnostics(wantDiagnostics);

            std::vector<const char*> schemaVec; // Deprecated conversion; kept for compatibility

            for (const auto& s : outputData)
            {
                schemaVec.push_back(&s.front());
            }

            sv.parseSchema(schemaVec.data(), schemaVec.size());
            sv.validate(cfg, scope, name, isRecursive, types, forceMode);
        }
        catch (const ConfigurationException& ex)
        {
            std::cerr << ex.what() << "\n";
        }
    }
    else if (cmd == "slist")
    {
        try
        {
            cfg->listFullyScopedNames(scope, name, types, isRecursive, filterPatterns, names);
            printElements(names);
        }
        catch (const ConfigurationException& ex)
        {
            std::cerr << ex.what() << "\n";
        }
    }
    else if (cmd == "llist")
    {
        try
        {
            cfg->listLocallyScopedNames(scope, name, types, isRecursive, filterPatterns, names);
            printElements(names);
        }
        catch (const ConfigurationException& ex)
        {
            std::cerr << ex.what() << "\n";
        }
    }
    else if (cmd == "type")
    {
        switch (cfg->type(scope, name))
        {
            case ConfType::String:
                std::cout << "string\n";
                break;
            case ConfType::List:
                std::cout << "list\n";
                break;
            case ConfType::Scope:
                std::cout << "scope\n";
                break;
            case ConfType::NoValue:
                std::cout << "no_value\n";
                break;
            default:
                throw std::exception{}; // Bug!
                break;
        }
    }
    else if (cmd == "print")
    {
        try
        {
            switch (cfg->type(scope, name))
            {
                case ConfType::String:
                    str = cfg->lookupString(scope, name);
                    std::cout << str << "\n";
                    break;
                case ConfType::List:
                {
                    std::vector<std::string> vec;
                    cfg->lookupList(scope, name, vec);
                    printElements(vec);
                }
                break;
                case ConfType::Scope:
                    std::cerr << "'" << fullyScopedName.str() << "' is a scope\n";
                    break;
                case ConfType::NoValue:
                    std::cerr << "'" << fullyScopedName.str() << "' does not exist\n";
                    break;
                default:
                    throw std::exception{}; // Bug!
                    break;
            }
        }
        catch (const ConfigurationException& ex)
        {
            std::cerr << ex.what() << "\n";
        }
    }
    else if (cmd == "dumpSec")
    {
        try
        {
            cfg->getSecurityConfiguration(secDumpCfg, secDumpScope);
            secDumpCfg->dump(buf, wantExpandedUidNames, secDumpScope, "allow_patterns");
            std::cout << buf.str();
            secDumpCfg->dump(buf, wantExpandedUidNames, secDumpScope, "deny_patterns");
            std::cout << buf.str();
            secDumpCfg->dump(buf, wantExpandedUidNames, secDumpScope, "trusted_directories");
            std::cout << buf.str();
        }
        catch (const ConfigurationException& ex)
        {
            std::cerr << ex.what() << "\n";
        }
    }
    else if (cmd == "dump")
    {
        try
        {
            cfg->dump(buf, wantExpandedUidNames, scope, name);
            std::cout << buf.str();
        }
        catch (const ConfigurationException& ex)
        {
            std::cerr << ex.what() << "\n";
        }
    }
    else
    {
        throw std::exception{}; // Bug!
    }

    cfg->destroy();
    secCfg->destroy();
    schemaCfg->destroy();
    return 0;
}

static void parseCmdLineArgs(int argc, char** argv, const char*& cmd, bool& isRecursive, bool& wantExpandedUidNames,
                             StringVector& filterPatterns, const char*& scope, const char*& name, const char*& cfgSource,
                             const char*& secSource, const char*& secScope, const char*& schemaSource, const char*& schemaName,
                             SchemaValidator::ForceMode& forceMode, bool& wantDiagnostics, ConfType& types, Configuration* cfg)
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
        std::cerr << "\nYou must specify -cfg <source>\n\n";
        usage("");
    }
    if (cmd == nullptr)
    {
        std::cerr << "\nYou must specify a command\n\n";
        usage("");
    }
    if (strcmp(cmd, "validate") == 0)
    {
        if (schemaSource == nullptr)
        {
            std::cerr << "\nThe validate command requires -schemaCfg <source>\n\n";
            usage("");
        }
        if (schemaName == nullptr)
        {
            std::cerr << "\nThe validate command requires -schema <full.name>\n\n";
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

static void usage(const std::string& optMsg)
{
    if (!optMsg.empty())
    {
        std::cerr << optMsg << "\n\n";
    }

    std::cerr << "usage: config4cpp -cfg <source> <command> <options>\n"
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
    exit(1);
}
