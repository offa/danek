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

// BNF of config file
// ------------------
// Note:	"|" denotes choice
//		"{ ... }*" denotes repetition 0+ times
//		"[ ... ]" denotes 0 or 1 times
//
//	configFile	= StmtList
//	StmtList	= { Stmt }*
//
//	Stmt		= ident_sym [ '=' | '?=' ] StringExpr ';'
//			| ident_sym [ '=' | '?=' ] ListExpr ';'
//			| ident_sym '{' StmtList '}' [ ';' ]
//			| '@include' StringExpr [ '@ifExists' ] ';'
//			| '@copyFrom' ident_sym [ '@ifExists' ] ';'
//			| '@remove' ident_sym ';'
//			| '@error' StringExpr ';'
//			| '@if' '(' Condition ')' '{' StmtList '}'
//			  { '@elseIf' '(' Condition ')' '{' StmtList '}' }*
//			  [ '@else' '{' StmtList '}' ]
//			  [ ';' ]
//
//	StringExpr	= String { '+' String }*
//
//	String		= string_sym
//			| ident_sym
//			| 'osType(' ')'
//			| 'osDirSeparator(' ')'
//			| 'osPathSeparator(' ')'
//			| 'getenv('  StringExpr [ ',' StringExpr ] ')'
//			| 'exec(' StringExpr [ ',' StringExpr ] ')'
// 			| 'join(' ListExpr ',' StringExpr ')'
// 			| 'siblingScope(' StringExpr ')'
//
//
//	ListExpr	= List { '+' List }*
//	List		= '[' StringExprList [ ',' ] ']'
//			| ident_sym
// 			| 'split(' StringExpr ',' StringExpr ')'
//
//	StringExprList = empty
//			| StringExpr { ',' StringExpr }*
//
//	Condition	= OrCondition
//	OrCondition	= AndCondition { '||' AndCondition }*
//	AndCondition	= TermCondition { '&&' TermCondition }*
//	TermCondition	= '(' Condition ')'
//			| '!' '(' Condition ')'
//			| 'isFileReadable(' StringExpr ')'
//			| StringExpr '==' StringExpr
//			| StringExpr '!=' StringExpr
//			| StringExpr '@in' ListExpr
//			| StringExpr '@matches' StringExpr
//----------------------------------------------------------------------

//--------
// #include's
//--------
#include "danek/internal/ConfigParser.h"
#include "danek/internal/platform.h"
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

namespace danek
{
    static bool startsWith(const char* str, const char* prefix)
    {
        return strncmp(str, prefix, strlen(prefix)) == 0;
    }

    //----------------------------------------------------------------------
    // Function:	Constructor
    //
    // Description:	Initialise instance variables and do actual parsing.
    //----------------------------------------------------------------------

    ConfigParser::ConfigParser(Configuration::SourceType sourceType, const char* source,
        const char* trustedCmdLine, const char* sourceDescription, ConfigurationImpl* config,
        bool ifExistsIsSpecified) throw(ConfigurationException)
    {
        StringBuffer msg;

        //--------
        // Initialise instance variables
        //--------
        m_config = config;
        m_errorInIncludedFile = false;
        switch (sourceType)
        {
            case Configuration::SourceType::File:
                m_fileName = source;
                break;
            case Configuration::SourceType::String:
                if (strcmp(sourceDescription, "") == 0)
                {
                    m_fileName = "<string-based configuration>";
                }
                else
                {
                    m_fileName = sourceDescription;
                }
                break;
            case Configuration::SourceType::Exec:
                if (strcmp(sourceDescription, "") == 0)
                {
                    m_fileName.clear();
                    m_fileName << "exec#" << source;
                }
                else
                {
                    m_fileName = sourceDescription;
                }
                source = trustedCmdLine;
                break;
            default:
                assert(0); // Bug!
                break;
        }

        //--------
        // Initialise the lexical analyser.
        // The constructor of the lexical analyser throws an exception
        // if it cannot open the specified file or execute the specified
        // command. If such an exception is thrown and if
        // "ifExistsIsSpecified" is true then we return without doing
        // any work.
        //--------
        try
        {
            m_lex = new ConfigLex(sourceType, source, &m_config->m_uidIdentifierProcessor);
        }
        catch (const ConfigurationException&)
        {
            m_lex = nullptr;
            if (ifExistsIsSpecified)
            {
                return;
            }
            else
            {
                throw;
            }
        }
        m_lex->nextToken(m_token);

        //--------
        // Push our file onto the the stack of (include'd) files.
        //--------
        m_config->pushIncludedFilename(m_fileName.str().c_str());

        //--------
        // Perform the actual work. Note that a config file
        // consists of a list of statements.
        //--------
        try
        {
            parseStmtList();
            accept(ConfigLex::LEX_EOF_SYM, "expecting identifier");
        }
        catch (const ConfigurationException& ex)
        {
            delete m_lex;
            m_lex = nullptr;
            m_config->popIncludedFilename(m_fileName.str().c_str());
            if (m_errorInIncludedFile)
            {
                throw;
            }
            else
            {
                msg << m_fileName << ", line " << m_token.lineNum() << ": " << ex.c_str();
                throw ConfigurationException(msg.str());
            }
        }

        //--------
        // Pop our file from the the stack of (include'd) files.
        //--------
        m_config->popIncludedFilename(m_fileName.str().c_str());
    }

    //----------------------------------------------------------------------
    // Function:	Destructor
    //
    // Description:	Reclaim memory.
    //----------------------------------------------------------------------

    ConfigParser::~ConfigParser()
    {
        delete m_lex;
    }

    //----------------------------------------------------------------------
    // Function:	parseStmtList()
    //
    // Description:	StmtList = { Stmt }*
    //----------------------------------------------------------------------

    void ConfigParser::parseStmtList()
    {
        while (m_token.type() == ConfigLex::LEX_IDENT_SYM || m_token.type() == ConfigLex::LEX_INCLUDE_SYM
               || m_token.type() == ConfigLex::LEX_IF_SYM
               || m_token.type() == ConfigLex::LEX_REMOVE_SYM
               || m_token.type() == ConfigLex::LEX_ERROR_SYM
               || m_token.type() == ConfigLex::LEX_COPY_FROM_SYM)
        {
            parseStmt();
        }
    }

    //----------------------------------------------------------------------
    // Function:	parseStmt()
    //
    // Description:	Stmt =	  ident [ '=' | '?=' ] RhsAssignStmt ';'
    //						| ident Scope [ ';' ]
    //						| '@include' StringExpr [ '@ifExists' ] ';'
    //						| '@copyFrom' ident_sym [ '@ifExists' ] ';'
    //----------------------------------------------------------------------

    void ConfigParser::parseStmt()
    {
        LexToken identName;
        short assignmentType;

        identName = m_token; // save it
        if (identName.type() == ConfigLex::LEX_INCLUDE_SYM)
        {
            parseIncludeStmt();
            return;
        }
        else if (identName.type() == ConfigLex::LEX_IF_SYM)
        {
            parseIfStmt();
            return;
        }
        else if (identName.type() == ConfigLex::LEX_REMOVE_SYM)
        {
            parseRemoveStmt();
            return;
        }
        else if (identName.type() == ConfigLex::LEX_ERROR_SYM)
        {
            parseErrorStmt();
            return;
        }
        else if (identName.type() == ConfigLex::LEX_COPY_FROM_SYM)
        {
            parseCopyStmt();
            return;
        }

        if (identName.type() == ConfigLex::LEX_IDENT_SYM && identName.spelling()[0] == '.')
        {
            error("cannot use '.' at start of the declaration of a "
                  "variable or scope,");
            return;
        }
        accept(ConfigLex::LEX_IDENT_SYM, "expecting identifier or 'include'");

        switch (m_token.type())
        {
            case ConfigLex::LEX_EQUALS_SYM:
            case ConfigLex::LEX_QUESTION_EQUALS_SYM:
                assignmentType = m_token.type();
                m_lex->nextToken(m_token);
                parseRhsAssignStmt(identName, assignmentType);
                accept(ConfigLex::LEX_SEMICOLON_SYM, "expecting ';' or '+'");
                break;
            case ConfigLex::LEX_OPEN_BRACE_SYM:
                parseScope(identName);
                //--------
                // Consume an optional ";"
                //--------
                if (m_token.type() == ConfigLex::LEX_SEMICOLON_SYM)
                {
                    m_lex->nextToken(m_token);
                }
                break;
            default:
                error("expecting '=', '?=' or '{'"); // matching '}'
                return;
        }
    }

    //----------------------------------------------------------------------
    // Function:	parseIncludeStmt()
    //
    // Description:	IncludeStmt = 'include' StringExpr [ 'if' 'exists' ] ';'
    //----------------------------------------------------------------------

    void ConfigParser::parseIncludeStmt()
    {
        StringBuffer source;
        StringBuffer msg;
        int includeLineNum;
        bool ifExistsIsSpecified;
        const char* execSource;
        StringBuffer trustedCmdLine;

        //--------
        // Consume the '@include' keyword
        //--------
        accept(ConfigLex::LEX_INCLUDE_SYM, "expecting 'include'");
        if (m_config->getCurrScope() != m_config->rootScope())
        {
            error("The '@include' command cannot be used inside a scope", false);
            return;
        }
        includeLineNum = m_token.lineNum();

        //--------
        // Consume the source
        //--------
        parseStringExpr(source);

        //--------
        // Check if this is a circular include.
        //--------
        m_config->checkForCircularIncludes(source.str().c_str(), includeLineNum);

        //--------
        // Consume "@ifExists" if specified
        //--------
        if (m_token.type() == ConfigLex::LEX_IF_EXISTS_SYM)
        {
            ifExistsIsSpecified = true;
            m_lex->nextToken(m_token);
        }
        else
        {
            ifExistsIsSpecified = false;
        }

        //--------
        // We get more intuitive error messages if we report a security
        // violation for include "exec#..." now instead of later from
        // inside a recursive call to the parser.
        //--------
        execSource = nullptr; // prevent warning about it possibly being uninitialized
        if (startsWith(source.str().c_str(), "exec#"))
        {
            execSource = source.str().c_str() + strlen("exec#");
            if (!m_config->isExecAllowed(execSource, trustedCmdLine))
            {
                msg << "cannot include \"" << source << "\" due to security restrictions";
                throw ConfigurationException(msg.str());
            }
        }

        //--------
        // The source is of one of the following forms:
        //	"exec#<command>"
        //	"file#<command>"
        //	"<filename>"
        //
        // Parse the source. If there is an error then propagate it with
        // some additional text to indicate that the error was in an
        // included file.
        //--------
        try
        {
            if (startsWith(source.str().c_str(), "exec#"))
            {
                ConfigParser tmp(Configuration::SourceType::Exec,
                    execSource,
                    trustedCmdLine.str().c_str(),
                    "",
                    m_config,
                    ifExistsIsSpecified);
            }
            else if (startsWith(source.str().c_str(), "file#"))
            {
                ConfigParser tmp(Configuration::SourceType::File,
                    source.str().c_str() + strlen("file#"),
                    trustedCmdLine.str().c_str(),
                    "",
                    m_config,
                    ifExistsIsSpecified);
            }
            else
            {
                ConfigParser tmp(Configuration::SourceType::File,
                    source.str().c_str(),
                    trustedCmdLine.str().c_str(),
                    "",
                    m_config,
                    ifExistsIsSpecified);
            }
        }
        catch (const ConfigurationException& ex)
        {
            m_errorInIncludedFile = true;
            msg << ex.c_str() << "\n(included from " << m_fileName << ", line " << includeLineNum << ")";
            throw ConfigurationException(msg.str());
        }

        //--------
        // Consume the terminating ';'
        //--------
        accept(ConfigLex::LEX_SEMICOLON_SYM, "expecting ';' or '@ifExists'");
    }

    //----------------------------------------------------------------------
    // Function:	parseIfStmt()
    //
    // Description:
    //----------------------------------------------------------------------

    void ConfigParser::parseIfStmt()
    {
        bool condition;

        //--------
        // Parse the "if ( Condition ) { StmtList }" clause
        //--------
        accept(ConfigLex::LEX_IF_SYM, "expecting 'if'");
        accept(ConfigLex::LEX_OPEN_PAREN_SYM, "expecting '('");
        condition = parseCondition();
        accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
        accept(ConfigLex::LEX_OPEN_BRACE_SYM, "expecting '{'");
        if (condition)
        {
            parseStmtList();
            accept(ConfigLex::LEX_CLOSE_BRACE_SYM, "expecting '}'");
        }
        else
        {
            skipToClosingBrace();
        }

        //--------
        // Parse 0+ "elseif ( Condition ) { StmtList }" clauses
        //--------
        while (m_token.type() == ConfigLex::LEX_ELSE_IF_SYM)
        {
            m_lex->nextToken(m_token);
            accept(ConfigLex::LEX_OPEN_PAREN_SYM, "expecting '('");
            bool condition2 = parseCondition();
            accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
            accept(ConfigLex::LEX_OPEN_BRACE_SYM, "expecting '{'");
            if (!condition && condition2)
            {
                parseStmtList();
                accept(ConfigLex::LEX_CLOSE_BRACE_SYM, "expecting '}'");
            }
            else
            {
                skipToClosingBrace();
            }
            condition = condition || condition2;
        }

        //--------
        // Parse the "else { StmtList }" clause, if any
        //--------
        if (m_token.type() == ConfigLex::LEX_ELSE_SYM)
        {
            m_lex->nextToken(m_token);
            accept(ConfigLex::LEX_OPEN_BRACE_SYM, "expecting '{'");
            if (!condition)
            {
                parseStmtList();
                accept(ConfigLex::LEX_CLOSE_BRACE_SYM, "expecting '}'");
            }
            else
            {
                skipToClosingBrace();
            }
        }

        //--------
        // Consume an optional ";"
        //--------
        if (m_token.type() == ConfigLex::LEX_SEMICOLON_SYM)
        {
            m_lex->nextToken(m_token);
        }
    }

    void ConfigParser::skipToClosingBrace()
    {
        int countOpenBraces;

        countOpenBraces = 1;
        while (countOpenBraces > 0)
        {
            switch (m_token.type())
            {
                case ConfigLex::LEX_OPEN_BRACE_SYM:
                    countOpenBraces++;
                    break;
                case ConfigLex::LEX_CLOSE_BRACE_SYM:
                    countOpenBraces--;
                    break;
                case ConfigLex::LEX_EOF_SYM:
                    error("expecting '}'");
                    break;
                default:
                    break;
            }
            m_lex->nextToken(m_token);
        }
    }

    //----------------------------------------------------------------------
    // Function:	parseCondition()
    //
    // Description:
    //----------------------------------------------------------------------

    bool ConfigParser::parseCondition()
    {
        return parseOrCondition();
    }

    //----------------------------------------------------------------------
    // Function:	parseOrCondition()
    //
    // Description:
    //----------------------------------------------------------------------

    bool ConfigParser::parseOrCondition()
    {
        bool result = parseAndCondition();
        while (m_token.type() == ConfigLex::LEX_OR_SYM)
        {
            m_lex->nextToken(m_token);
            bool result2 = parseAndCondition();
            result = result || result2;
        }
        return result;
    }

    //----------------------------------------------------------------------
    // Function:	parseAndCondition()
    //
    // Description:
    //----------------------------------------------------------------------

    bool ConfigParser::parseAndCondition()
    {
        bool result = parseTerminalCondition();
        while (m_token.type() == ConfigLex::LEX_AND_SYM)
        {
            m_lex->nextToken(m_token);
            bool result2 = parseTerminalCondition();
            result = result && result2;
        }
        return result;
    }

    //----------------------------------------------------------------------
    // Function:	parseTerminalCondition()
    //
    // TermCondition	= '(' Condition ')'
    //					| '!' '(' Condition ')'
    //					| 'isFileReadable(' StringExpr ')'
    //					| StringExpr '==' StringExpr
    //					| StringExpr '!=' StringExpr
    //					| StringExpr 'in' ListExpr
    //					| StringExpr 'matches' StringExpr
    //----------------------------------------------------------------------

    bool ConfigParser::parseTerminalCondition()
    {
        StringBuffer str1;
        StringBuffer str2;
        StringVector list;
        bool result;
        int len;
        int i;

        result = false;
        if (m_token.type() == ConfigLex::LEX_NOT_SYM)
        {
            m_lex->nextToken(m_token);
            accept(ConfigLex::LEX_OPEN_PAREN_SYM, "expecting '('");
            result = !parseCondition();
            accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
            return result;
        }
        if (m_token.type() == ConfigLex::LEX_OPEN_PAREN_SYM)
        {
            m_lex->nextToken(m_token);
            result = parseCondition();
            accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
            return result;
        }
        if (m_token.type() == ConfigLex::LEX_FUNC_IS_FILE_READABLE_SYM)
        {
            m_lex->nextToken(m_token);
            parseStringExpr(str1);
            accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
            FILE* file = fopen(str1.str().c_str(), "r");
            if (file == nullptr)
            {
                return false;
            }
            else
            {
                fclose(file);
                return true;
            }
        }
        parseStringExpr(str1);
        switch (m_token.type())
        {
            case ConfigLex::LEX_EQUALS_EQUALS_SYM:
                m_lex->nextToken(m_token);
                parseStringExpr(str2);
                result = (strcmp(str1.str().c_str(), str2.str().c_str()) == 0);
                break;
            case ConfigLex::LEX_NOT_EQUALS_SYM:
                m_lex->nextToken(m_token);
                parseStringExpr(str2);
                result = (strcmp(str1.str().c_str(), str2.str().c_str()) != 0);
                break;
            case ConfigLex::LEX_IN_SYM:
                m_lex->nextToken(m_token);
                parseListExpr(list);
                len = list.size();
                result = false;
                for (i = 0; i < len; i++)
                {
                    if (strcmp(str1.str().c_str(), list[i].c_str()) == 0)
                    {
                        result = true;
                        break;
                    }
                }
                break;
            case ConfigLex::LEX_MATCHES_SYM:
                m_lex->nextToken(m_token);
                parseStringExpr(str2);
                result = Configuration::patternMatch(str1.str().c_str(), str2.str().c_str());
                break;
            default:
                error("expecting '(', or a string expression");
                break;
        }
        return result;
    }

    //----------------------------------------------------------------------
    // Function:	parseCopyStmt()
    //
    // Description:	CopyStmt = '@copyFrom' stringExpr [ '@ifExists' ] ';'
    //----------------------------------------------------------------------

    void ConfigParser::parseCopyStmt()
    {
        StringBuffer fromScopeName;
        StringBuffer prefix;
        const char* toScopeName;
        StringBuffer msg;
        StringVector fromNamesVec;
        ConfigScope* fromScope;
        ConfigScope* dummyScope;
        int len;
        int fromScopeNameLen;
        bool ifExistsIsSpecified;

        accept(ConfigLex::LEX_COPY_FROM_SYM, "expecting '@copyFrom'");
        parseStringExpr(fromScopeName);
        fromScopeNameLen = fromScopeName.size();

        //--------
        // Consume "@ifExists" if specified
        //--------
        if (m_token.type() == ConfigLex::LEX_IF_EXISTS_SYM)
        {
            ifExistsIsSpecified = true;
            m_lex->nextToken(m_token);
        }
        else
        {
            ifExistsIsSpecified = false;
        }

        //--------
        // Sanity check: cannot copy from a parent scope
        //--------
        toScopeName = m_config->getCurrScope()->scopedName().c_str();
        if (strcmp(toScopeName, fromScopeName.str().c_str()) == 0)
        {
            throw ConfigurationException("copy statement: cannot copy from own scope");
        }
        prefix << fromScopeName << ".";
        if (strncmp(toScopeName, prefix.str().c_str(), fromScopeNameLen + 1) == 0)
        {
            throw ConfigurationException("copy statement: cannot copy from a parent scope");
        }

        //--------
        // If the scope does not exist and if "@ifExists" was specified
        // then we short-circuit the rest of this function.
        //--------
        const ConfigItem* item = m_config->lookup(fromScopeName.str().c_str(), fromScopeName.str().c_str(), true);
        if (item == nullptr && ifExistsIsSpecified)
        {
            accept(ConfigLex::LEX_SEMICOLON_SYM, "expecting ';'");
            return;
        }

        if (item == nullptr)
        {
            msg << "copy statement: scope '" << fromScopeName << "' does not exist";
            throw ConfigurationException(msg.str());
        }
        if (item->type() != ConfType::Scope)
        {
            msg << "copy statement: '" << fromScopeName << "' is not a scope";
            throw ConfigurationException(msg.str());
        }
        fromScope = item->scopeVal();
        assert(fromScope != nullptr);

        //--------
        // Get a recursive listing of all the items in fromScopeName
        //--------
        fromScope->listFullyScopedNames(ConfType::ScopesAndVars, true, fromNamesVec);

        //--------
        // Copy all the items into the current scope
        //--------
        len = fromNamesVec.size();
        for (int i = 0; i < len; i++)
        {
            const char* newName = &fromNamesVec[i][fromScopeNameLen + 1];
            item = m_config->lookup(fromNamesVec[i].c_str(), fromNamesVec[i].c_str(), true);
            assert(item != nullptr);
            switch (item->type())
            {
                case ConfType::String:
                    m_config->insertString("", newName, item->stringVal().c_str());
                    break;
                case ConfType::List:
                    m_config->insertList(newName, StringVector{item->listVal()});
                    break;
                case ConfType::Scope:
                    m_config->ensureScopeExists(newName, dummyScope);
                    break;
                default:
                    assert(0); // Bug!
                    break;
            }
        }

        //--------
        // Consume the terminating ';'
        //--------
        accept(ConfigLex::LEX_SEMICOLON_SYM, "expecting ';' or '@ifExists'");
    }

    //----------------------------------------------------------------------
    // Function:	parseRemoveStmt()
    //
    // Description:	removeStmt = 'remove' ident_sym ';'
    //----------------------------------------------------------------------

    void ConfigParser::parseRemoveStmt()
    {
        ConfigScope* currScope;
        StringBuffer identName;
        StringBuffer msg;

        accept(ConfigLex::LEX_REMOVE_SYM, "expecting 'remove'");
        identName = m_token.spelling();
        accept(ConfigLex::LEX_IDENT_SYM, "expecting an identifier");
        if (strchr(identName.str().c_str(), '.') != nullptr)
        {
            msg << m_fileName << ": can remove entries from only the "
                << "current scope";
            throw ConfigurationException(msg.str());
        }
        currScope = m_config->getCurrScope();
        if (!currScope->removeItem(identName.str().c_str()))
        {
            msg << m_fileName << ": '" << identName << "' does not exist in the current scope";
            throw ConfigurationException(msg.str());
        }
        accept(ConfigLex::LEX_SEMICOLON_SYM, "expecting ';'");
    }

    //----------------------------------------------------------------------
    // Function:	parseErrorStmt()
    //
    // Description:	ErrorStmt = 'error' stringExpr ';'
    //----------------------------------------------------------------------

    void ConfigParser::parseErrorStmt()
    {
        StringBuffer msg;

        accept(ConfigLex::LEX_ERROR_SYM, "expecting 'error'");
        parseStringExpr(msg);
        accept(ConfigLex::LEX_SEMICOLON_SYM, "expecting ';'");
        throw ConfigurationException(msg.str());
    }

    //----------------------------------------------------------------------
    // Function:	parseScope()
    //
    // Description:	Scope	= '{' StmtList '}'
    //----------------------------------------------------------------------

    void ConfigParser::parseScope(LexToken& scopeName)
    {
        ConfigScope* oldScope;
        ConfigScope* newScope;
        StringBuffer errMsg;

        //--------
        // Create the new scope and put it onto the stack
        //--------
        oldScope = m_config->getCurrScope();
        m_config->ensureScopeExists(scopeName.spelling().c_str(), newScope);
        m_config->setCurrScope(newScope);

        //--------
        // Do the actual parsing
        //--------
        accept(ConfigLex::LEX_OPEN_BRACE_SYM, "expecting '{'");
        parseStmtList();
        accept(ConfigLex::LEX_CLOSE_BRACE_SYM, "expecting an identifier or '}'");

        //--------
        // Finally, pop the scope from the stack
        //--------
        m_config->setCurrScope(oldScope);
    }

    //----------------------------------------------------------------------
    // Function:	parseRhsAssignStmt()
    //
    // Description:	RhsAssignStmt =	  StringExpr
    //				| ListExpr
    //----------------------------------------------------------------------

    void ConfigParser::parseRhsAssignStmt(LexToken& varName, short assignmentType)
    {
        StringBuffer stringExpr;
        StringVector listExpr;
        ConfType varType;
        StringBuffer msg;
        bool doAssign;

        switch (m_config->type(varName.spelling().c_str(), ""))
        {
            case ConfType::String:
            case ConfType::List:
                if (assignmentType == ConfigLex::LEX_QUESTION_EQUALS_SYM)
                {
                    doAssign = false;
                }
                else
                {
                    doAssign = true;
                }
                break;
            default:
                doAssign = true;
                break;
        }

        //--------
        // Examine the current token to determine whether the expression
        // to be parsed is a stringExpr or an listExpr.
        //--------
        switch (m_token.type())
        {
            case ConfigLex::LEX_OPEN_BRACKET_SYM:
            case ConfigLex::LEX_FUNC_SPLIT_SYM:
                varType = ConfType::List;
                break;
            case ConfigLex::LEX_FUNC_SIBLING_SCOPE_SYM:
            case ConfigLex::LEX_FUNC_GETENV_SYM:
            case ConfigLex::LEX_FUNC_EXEC_SYM:
            case ConfigLex::LEX_FUNC_JOIN_SYM:
            case ConfigLex::LEX_FUNC_READ_FILE_SYM:
            case ConfigLex::LEX_FUNC_REPLACE_SYM:
            case ConfigLex::LEX_FUNC_OS_TYPE_SYM:
            case ConfigLex::LEX_FUNC_OS_DIR_SEP_SYM:
            case ConfigLex::LEX_FUNC_OS_PATH_SEP_SYM:
            case ConfigLex::LEX_FUNC_FILE_TO_DIR_SYM:
            case ConfigLex::LEX_FUNC_CONFIG_FILE_SYM:
            case ConfigLex::LEX_FUNC_CONFIG_TYPE_SYM:
            case ConfigLex::LEX_STRING_SYM:
                varType = ConfType::String;
                break;
            case ConfigLex::LEX_IDENT_SYM:
                //--------
                // This identifier (hopefully) denotes an already
                // existing variable. We have to determine the type
                // of the variable (it is either a string or a list)
                // in order to proceed with the parsing.
                //--------
                switch (m_config->type(m_token.spelling().c_str(), ""))
                {
                    case ConfType::String:
                        varType = ConfType::String;
                        break;
                    case ConfType::List:
                        varType = ConfType::List;
                        break;
                    default:
                        msg << "identifier '" << m_token.spelling() << "' not previously declared";
                        error(msg.str().c_str(), false);
                        return;
                }
                break;
            default:
                error("expecting a string, identifier or '['"); // matching ']'
                return;
        }

        //--------
        // Now that we know the type of the input expression, we
        // can parse it correctly.
        //--------
        switch (varType)
        {
            case ConfType::String:
                parseStringExpr(stringExpr);
                if (doAssign)
                {
                    m_config->insertString("", varName.spelling().c_str(), stringExpr.str().c_str());
                }
                break;
            case ConfType::List:
                parseListExpr(listExpr);
                if (doAssign)
                {
                    m_config->insertList(varName.spelling().c_str(), listExpr);
                }
                break;
            default:
                assert(0); // Bug
                break;
        }
    }

    //----------------------------------------------------------------------
    // Function:	parseStringExpr()
    //
    // Description:	StringExpr = String { '+' String }*
    //----------------------------------------------------------------------

    void ConfigParser::parseStringExpr(StringBuffer& expr)
    {
        StringBuffer expr2;

        parseString(expr);
        while (m_token.type() == ConfigLex::LEX_PLUS_SYM)
        {
            m_lex->nextToken(m_token); // consume the '+'
            parseString(expr2);
            expr << expr2;
        }
    }

    //----------------------------------------------------------------------
    // Function:	parseString()
    //
    // Description:	string	= string_sym
    //						| ident_sym
    //						| 'os.type(' ')'
    //						| 'dir.sep(' ')'
    //						| 'path.sep(' ')'
    //						| Env
    //						| Exec
    //						| Join
    //						| Split
    //----------------------------------------------------------------------

    void ConfigParser::parseString(StringBuffer& str)
    {
        ConfType type;
        StringBuffer msg;
        StringBuffer name;
        const char* constStr;

        str.clear();
        switch (m_token.type())
        {
            case ConfigLex::LEX_FUNC_SIBLING_SCOPE_SYM:
                parseSiblingScope(str);
                break;
            case ConfigLex::LEX_FUNC_GETENV_SYM:
                parseEnv(str);
                break;
            case ConfigLex::LEX_FUNC_EXEC_SYM:
                parseExec(str);
                break;
            case ConfigLex::LEX_FUNC_JOIN_SYM:
                parseJoin(str);
                break;
            case ConfigLex::LEX_FUNC_READ_FILE_SYM:
                parseReadFile(str);
                break;
            case ConfigLex::LEX_FUNC_REPLACE_SYM:
                parseReplace(str);
                break;
            case ConfigLex::LEX_FUNC_OS_TYPE_SYM:
                str = CONFIG4CPP_OS_TYPE;
                m_lex->nextToken(m_token);
                accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
                break;
            case ConfigLex::LEX_FUNC_OS_DIR_SEP_SYM:
                str = CONFIG4CPP_DIR_SEP;
                m_lex->nextToken(m_token);
                accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
                break;
            case ConfigLex::LEX_FUNC_OS_PATH_SEP_SYM:
                str = CONFIG4CPP_PATH_SEP;
                m_lex->nextToken(m_token);
                accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
                break;
            case ConfigLex::LEX_FUNC_FILE_TO_DIR_SYM:
                m_lex->nextToken(m_token);
                parseStringExpr(name);
                accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
                getDirectoryOfFile(name.str().c_str(), str);
                break;
            case ConfigLex::LEX_FUNC_CONFIG_FILE_SYM:
                m_lex->nextToken(m_token);
                accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
                str = m_fileName;
                break;
            case ConfigLex::LEX_FUNC_CONFIG_TYPE_SYM:
                {
                    m_lex->nextToken(m_token);
                    parseStringExpr(name);
                    accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
                    const ConfigItem* item = m_config->lookup(name.str().c_str(), name.str().c_str());
                    if (item == nullptr)
                    {
                        type = ConfType::NoValue;
                    }
                    else
                    {
                        type = item->type();
                    }
                    switch (type)
                    {
                        case ConfType::String:
                            str = "string";
                            break;
                        case ConfType::List:
                            str = "list";
                            break;
                        case ConfType::Scope:
                            str = "scope";
                            break;
                        case ConfType::NoValue:
                            str = "no_value";
                            break;
                        default:
                            assert(0); // Bug!
                            break;
                    }
                }
                break;
            case ConfigLex::LEX_STRING_SYM:
                str = m_token.spelling();
                m_lex->nextToken(m_token);
                break;
            case ConfigLex::LEX_IDENT_SYM:
                m_config->stringValue(m_token.spelling().c_str(), m_token.spelling().c_str(), constStr, type);
                switch (type)
                {
                    case ConfType::String:
                        str = constStr;
                        break;
                    case ConfType::NoValue:
                        msg << "identifier '" << m_token.spelling() << "' not previously declared";
                        error(msg.str().c_str(), false);
                        return;
                    case ConfType::Scope:
                        msg << "identifier '" << m_token.spelling() << "' is a scope instead of a string";
                        error(msg.str().c_str(), false);
                        return;
                    case ConfType::List:
                        msg << "identifier '" << m_token.spelling() << "' is a list instead of a string";
                        error(msg.str().c_str(), false);
                        return;
                    default:
                        assert(0); // Bug
                        return;
                }
                m_lex->nextToken(m_token);
                break;
            default:
                error("expecting a string or identifier");
                return;
        }
    }

    //----------------------------------------------------------------------
    // Function:	parseEnv()
    //
    // Description:	Env	= 'getenv(' StringExpr ')'
    //			| 'getenv(' StringExpr ',' StringExpr ')'
    //----------------------------------------------------------------------

    void ConfigParser::parseEnv(StringBuffer& str)
    {
        StringBuffer msg;
        StringBuffer envVarName;
        bool hasDefaultStr;
        StringBuffer defaultStr;
        const char* val;

        accept(ConfigLex::LEX_FUNC_GETENV_SYM, "expecting 'getenv('");
        parseStringExpr(envVarName);
        if (m_token.type() == ConfigLex::LEX_COMMA_SYM)
        {
            accept(ConfigLex::LEX_COMMA_SYM, "expecting ','");
            parseStringExpr(defaultStr);
            hasDefaultStr = true;
        }
        else
        {
            hasDefaultStr = false;
        }
        accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
        val = getenv(envVarName.str().c_str());
        if (val == nullptr && hasDefaultStr)
        {
            val = defaultStr.str().c_str();
        }
        if (val == nullptr)
        {
            msg << "cannot access the '" << envVarName << "' environment variable";
            throw ConfigurationException(msg.str());
        }
        str = val;
    }

    void ConfigParser::parseSiblingScope(StringBuffer& str)
    {
        StringBuffer msg;
        ConfigScope* currScope;
        StringBuffer siblingName;
        const char* parentScopeName;

        accept(ConfigLex::LEX_FUNC_SIBLING_SCOPE_SYM, "expecting 'siblingScope('");
        currScope = m_config->getCurrScope();
        if (currScope == m_config->rootScope())
        {
            error("The siblingScope() function cannot be used in the "
                  "root scope",
                false);
            return;
        }
        parentScopeName = currScope->parentScope()->scopedName().c_str();
        parseStringExpr(siblingName);
        accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
        Configuration::mergeNames(parentScopeName, siblingName.str().c_str(), str);
    }

    //----------------------------------------------------------------------
    // Function:	parseReadFile()
    //
    // Description:
    //----------------------------------------------------------------------

    void ConfigParser::parseReadFile(StringBuffer& str)
    {
        StringBuffer msg;
        StringBuffer fileName;
        int ch;
        BufferedFileReader file;

        accept(ConfigLex::LEX_FUNC_READ_FILE_SYM, "expecting 'read.file('");
        parseStringExpr(fileName);
        accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
        str.clear();
        if (!file.open(fileName.str().c_str()))
        {
            msg << "error reading " << fileName << ": " << strerror(errno);
            throw ConfigurationException(msg.str());
        }
        while ((ch = file.getChar()) != EOF)
        {
            if (ch != '\r')
            {
                str.append((char) ch);
            }
        }
    }

    //----------------------------------------------------------------------
    // Function:	parseJoin()
    //
    // Description: Join	= 'join(' ListExpr ',' StringExpr ')'
    //----------------------------------------------------------------------

    void ConfigParser::parseJoin(StringBuffer& str)
    {
        StringVector list;
        StringBuffer separator;
        int len;
        int i;

        accept(ConfigLex::LEX_FUNC_JOIN_SYM, "expecting 'join('");
        parseListExpr(list);
        accept(ConfigLex::LEX_COMMA_SYM, "expecting ','");
        parseStringExpr(separator);
        accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");

        str.clear();
        len = list.size();
        for (i = 0; i < len; i++)
        {
            str.append(list[i]);
            if (i < len - 1)
            {
                str.append(separator);
            }
        }
    }

    //----------------------------------------------------------------------
    // Function:	parseReplace()
    //
    // Description: Replace	= 'replace(' StringExpr ',' StringExpr
    //						  ',' StringExpr ')'
    //----------------------------------------------------------------------

    void ConfigParser::parseReplace(StringBuffer& result)
    {
        StringBuffer origStr;
        StringBuffer searchStr;
        StringBuffer replacementStr;
        const char* p;
        int searchStrLen;

        accept(ConfigLex::LEX_FUNC_REPLACE_SYM, "expecting 'replace('");
        parseStringExpr(origStr);
        accept(ConfigLex::LEX_COMMA_SYM, "expecting ','");
        parseStringExpr(searchStr);
        accept(ConfigLex::LEX_COMMA_SYM, "expecting ','");
        parseStringExpr(replacementStr);
        accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");

        result = "";
        searchStrLen = searchStr.size();
        int currStart = 0;
        p = strstr(origStr.str().c_str(), searchStr.str().c_str());
        while (p != nullptr)
        {
            int currEnd = p - origStr.str().c_str();
            origStr[currEnd] = '\0';
            result << (origStr.str().c_str() + currStart);
            result << replacementStr;
            currStart = currEnd + searchStrLen;
            p = strstr(origStr.str().c_str() + currStart, searchStr.str().c_str());
        }
        result << (origStr.str().c_str() + currStart);
    }

    //----------------------------------------------------------------------
    // Function:	parseSplit()
    //
    // Description: Split	= 'split(' StringExpr ',' StringExpr ')'
    //----------------------------------------------------------------------

    void ConfigParser::parseSplit(StringVector& list)
    {
        StringBuffer str;
        StringBuffer delim;
        const char* p;
        int delimLen;

        accept(ConfigLex::LEX_FUNC_SPLIT_SYM, "expecting 'split('");
        parseStringExpr(str);
        accept(ConfigLex::LEX_COMMA_SYM, "expecting ','");
        parseStringExpr(delim);
        accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");

        list.clear();
        delimLen = delim.size();
        int currStart = 0;
        p = strstr(str.str().c_str(), delim.str().c_str());
        while (p != nullptr)
        {
            int currEnd = p - str.str().c_str();
            str[currEnd] = '\0';
            list.push_back(str.str().c_str() + currStart);
            currStart = currEnd + delimLen;
            p = strstr(str.str().c_str() + currStart, delim.str().c_str());
        }
        list.push_back(str.str().c_str() + currStart);
    }

    //----------------------------------------------------------------------
    // Function:	parseExec()
    //
    // Description:	Exec	= 'os.exec(' StringExpr ')'
    //						| 'os.exec(' StringExpr ',' StringExpr ')'
    //----------------------------------------------------------------------

    void ConfigParser::parseExec(StringBuffer& str)
    {
        StringBuffer msg;
        StringBuffer cmd;
        bool hasDefaultStr;
        StringBuffer defaultStr;
        bool execStatus;
        StringBuffer trustedCmdLine;

        //--------
        // Parse the command and default value, if any
        //--------
        accept(ConfigLex::LEX_FUNC_EXEC_SYM, "expecting 'os.exec('");
        parseStringExpr(cmd);
        if (m_token.type() == ConfigLex::LEX_COMMA_SYM)
        {
            accept(ConfigLex::LEX_COMMA_SYM, "expecting ','");
            parseStringExpr(defaultStr);
            hasDefaultStr = true;
        }
        else
        {
            hasDefaultStr = false;
        }

        if (!m_config->isExecAllowed(cmd.str().c_str(), trustedCmdLine))
        {
            msg << "cannot execute \"" << cmd.str() << "\" due to security restrictions";
            throw ConfigurationException(msg.str());
        }

        //--------
        // Execute the command and decide if we throw an exception,
        // return the default value, if any, or return the output of
        // the successful execCmd().
        //--------
        execStatus = execCmd(trustedCmdLine.str().c_str(), str);
        if (!execStatus && !hasDefaultStr)
        {
            msg << "os.exec(\"" << cmd << "\") failed: " << str;
            throw ConfigurationException(msg.str());
        }
        else if (!execStatus && hasDefaultStr)
        {
            str = defaultStr;
        }
        else
        {
            assert(execStatus == true);
        }

        accept(ConfigLex::LEX_CLOSE_PAREN_SYM, "expecting ')'");
    }

    //----------------------------------------------------------------------
    // Function:	parseListExpr()
    //
    // Description:	ListExpr = List { '+' List }*
    //----------------------------------------------------------------------

    void ConfigParser::parseListExpr(StringVector& expr)
    {
        StringVector expr2;

        expr.clear();
        parseList(expr);
        while (m_token.type() == ConfigLex::LEX_PLUS_SYM)
        {
            m_lex->nextToken(m_token); // consume the '+'
            parseList(expr2);

            for( const auto& str : expr2 )
            {
                expr.push_back(str);
            }
        }
    }

    //----------------------------------------------------------------------
    // Function:	parseList()
    //
    // Description:	List	= 'split(' StringExpr ',' StringExpr ')'
    //						| '[' StringExprList ']'
    //						| ident_sym
    //----------------------------------------------------------------------

    void ConfigParser::parseList(StringVector& expr)
    {
        ConfType type;
        StringBuffer msg;

        switch (m_token.type())
        {
            case ConfigLex::LEX_FUNC_SPLIT_SYM:
                parseSplit(expr);
                break;
            case ConfigLex::LEX_OPEN_BRACKET_SYM:
                //--------
                // '[' StringExprList [ ',' ] ']'
                //--------
                m_lex->nextToken(m_token); // consume the open bracket
                parseStringExprList(expr);
                accept(ConfigLex::LEX_CLOSE_BRACKET_SYM, "expecting ']'");
                break;
            case ConfigLex::LEX_IDENT_SYM:
                //--------
                // ident_sym: make sure the identifier is a list
                //--------
                m_config->listValue(m_token.spelling().c_str(), m_token.spelling().c_str(), expr, type);
                if (type != ConfType::List)
                {
                    msg << "identifier '" << m_token.spelling() << "' is not a list";
                    error(msg.str().c_str(), false);
                }
                m_lex->nextToken(m_token); // consume the identifier
                break;
            default:
                error("expecting an identifier or '['"); // matching ']'
                break;
        }
    }

    //----------------------------------------------------------------------
    // Function:	getDirectoryOfFile()
    //
    // Description:	Returns the directory name of the specified file
    //----------------------------------------------------------------------

    void ConfigParser::getDirectoryOfFile(const char* file, StringBuffer& result)
    {
        int len;
        int i;
        int j;
        bool found;

        len = strlen(file);
        found = false;
        for (i = len - 1; i >= 0; i--)
        {
            if (file[i] == '/' || file[i] == CONFIG4CPP_DIR_SEP[0])
            {
                found = true;
                break;
            }
        }
        if (!found)
        {
            //--------
            // Case 1. "foo.cfg"       ->  "."     (UNIX and Windows)
            //--------
            result = ".";
        }
        else if (i == 0)
        {
            //--------
            // Case 2. "/foo.cfg"      ->  "/."    (UNIX and Windows)
            // Or:     "\foo.cfg"      ->  "\."    (Windows only)
            //--------
            result = "";
            result << file[0] << ".";
        }
        else
        {
            //--------
            // Case 3. "/tmp/foo.cfg"  ->  "/tmp"  (UNIX and Windows)
            // Or:     "C:\foo.cfg"    ->  "C:\."  (Windows only)
            //--------
            assert(i > 0);
            result = "";
            for (j = 0; j < i; j++)
            {
                result << file[j];
            }
            if (i == 2 && isalpha(file[0]) && file[1] == ':')
            {
                result << file[i] << ".";
            }
        }
    }

    //----------------------------------------------------------------------
    // Function:	parseStringExprList()
    //
    // Description:	StringExprList = empty
    //						       | StringExpr { ',' StringExpr }* [ ',' ]
    //----------------------------------------------------------------------

    void ConfigParser::parseStringExprList(StringVector& list)
    {
        StringBuffer str;
        short type;

        list.clear();
        type = m_token.type();
        if (type == ConfigLex::LEX_CLOSE_BRACKET_SYM)
        {
            return; // empty list
        }
        if (!m_token.isStringFunc() && type != ConfigLex::LEX_STRING_SYM && type != ConfigLex::LEX_IDENT_SYM)
        {
            error("expecting a string or ']'");
        }

        parseStringExpr(str);
        list.push_back(str.str());
        while (m_token.type() == ConfigLex::LEX_COMMA_SYM)
        {
            m_lex->nextToken(m_token);
            if (m_token.type() == ConfigLex::LEX_CLOSE_BRACKET_SYM)
            {
                return;
            }
            parseStringExpr(str);
            list.push_back(str.str());
        }
    }

    //----------------------------------------------------------------------
    // Function:	accept()
    //
    // Description:	Consume the next token if it is the expected one.
    //				Otherwise report an error.
    //----------------------------------------------------------------------

    void ConfigParser::accept(short sym, const char* errMsg)
    {
        if (m_token.type() == sym)
        {
            m_lex->nextToken(m_token);
        }
        else
        {
            error(errMsg);
        }
    }

    //----------------------------------------------------------------------
    // Function:	error()
    //
    // Description:	Report an error.
    //----------------------------------------------------------------------

    void ConfigParser::error(const char* errMsg, bool printNear)
    {
        StringBuffer msg;

        //--------
        // In order to provide good error messages, lexical errors
        // take precedence over parsing errors. For example, there is
        // no point in printing out "was expecting a string or identifier"
        // if the real problem is that the lexical analyser returned a
        // string_with_eol_sym symbol.
        //--------
        // msg << "line " << m_token.lineNum() << ": ";
        switch (m_token.type())
        {
            case ConfigLex::LEX_UNKNOWN_FUNC_SYM:
                msg << "'" << m_token.spelling() << "' "
                    << "is not a built-in function";
                throw ConfigurationException(msg.str());
            case ConfigLex::LEX_SOLE_DOT_IDENT_SYM:
                msg << "'.' is not a valid identifier";
                throw ConfigurationException(msg.str());
            case ConfigLex::LEX_TWO_DOTS_IDENT_SYM:
                msg << "'..' appears in identified '" << m_token.spelling() << "'";
                throw ConfigurationException(msg.str());
            case ConfigLex::LEX_STRING_WITH_EOL_SYM:
                msg << "end-of-line not allowed in string '" << m_token.spelling() << "'";
                throw ConfigurationException(msg.str());
            case ConfigLex::LEX_BLOCK_STRING_WITH_EOF_SYM:
                msg << "end-of-file encountered in block string starting at "
                    << "line " << m_token.lineNum();
                throw ConfigurationException(msg.str());
            case ConfigLex::LEX_ILLEGAL_IDENT_SYM:
                msg << "'" << m_token.spelling() << "' "
                    << "is not a legal identifier";
                throw ConfigurationException(msg.str());
            default:
                // No lexical error. Handle the parsing error below.
                break;
        }

        //--------
        // If we get this far then it means that we have to report
        // a parsing error (as opposed to a lexical error; they have
        // already been handled).
        //--------

        if (printNear && m_token.type() == ConfigLex::LEX_STRING_SYM)
        {
            msg << errMsg << " near \"" << m_token.spelling() << "\"";
            throw ConfigurationException(msg.str());
        }
        else if (printNear && m_token.type() != ConfigLex::LEX_STRING_SYM)
        {
            msg << errMsg << " near '" << m_token.spelling() << "'";
            throw ConfigurationException(msg.str());
        }
        else
        {
            msg << errMsg;
            throw ConfigurationException(msg.str());
        }
    }
}
