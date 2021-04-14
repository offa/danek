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

#pragma once

//--------
// #include's
//--------
#include "ConfigScope.h"
#include "UidIdentifierProcessor.h"
#include "danek/Configuration.h"

namespace danek
{
    //--------
    // Forward class declarations.
    //--------
    class ConfigParser;

    struct SpellingAndValue
    {
        const char* spelling;
        int val;
    };

    //--------
    // Class ConfigurationImpl
    //--------

    class ConfigurationImpl : public Configuration
    {
    public:
        //--------
        // Constructor and destructor
        //--------
        ConfigurationImpl();
        virtual ~ConfigurationImpl();

        //--------
        // Public operations
        //--------
        virtual void setFallbackConfiguration(Configuration* cfg);
        virtual void setFallbackConfiguration(Configuration::SourceType sourceType, const char* source,
                                              const char* sourceDescription = "");
        virtual const Configuration* getFallbackConfiguration();

        virtual void setSecurityConfiguration(Configuration* cfg, bool takeOwnership, const char* scope = "");
        virtual void setSecurityConfiguration(const char* cfgInput, const char* scope = "");
        virtual void getSecurityConfiguration(const Configuration*& cfg, const char*& scope);

        virtual void parse(Configuration::SourceType sourceType, const char* source, const char* sourceDescription = "");
        virtual const char* fileName() const;
        virtual ConfType type(const char* scope, const char* localName) const;

        virtual void listFullyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                          StringVector& names) const;
        virtual void listFullyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                          const char* filterPattern, StringVector& names) const;
        virtual void listFullyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                          const StringVector& filterPatterns, StringVector& names) const;

        virtual void listLocallyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                            StringVector& names) const;
        virtual void listLocallyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                            const char* filterPattern, StringVector& names) const;
        virtual void listLocallyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                            const StringVector& filterPatterns, StringVector& names) const;

        virtual bool uidEquals(const char* s1, const char* s2) const;
        virtual void expandUid(StringBuffer& spelling);

        virtual std::string unexpandUid(const char* spelling, StringBuffer& buf) const;

        virtual void dump(StringBuffer& buf, bool wantExpandedUidNames) const;
        virtual void dump(StringBuffer& buf, bool wantExpandedUidNames, const char* scope, const char* localName) const;

        virtual bool isBoolean(const char* str) const;
        virtual bool isInt(const char* str) const;
        virtual bool isFloat(const char* str) const;
        virtual bool isDurationMicroseconds(const char* str) const;
        virtual bool isDurationMilliseconds(const char* str) const;
        virtual bool isDurationSeconds(const char* str) const;
        virtual bool isMemorySizeBytes(const char* str) const;
        virtual bool isMemorySizeKB(const char* str) const;
        virtual bool isMemorySizeMB(const char* str) const;
        virtual bool isEnum(const char* str, const EnumNameAndValue* enumInfo, int numEnums) const;
        virtual bool isFloatWithUnits(const char* str, const char** allowedUnits, int allowedUnitsSize) const;
        virtual bool isIntWithUnits(const char* str, const char** allowedUnits, int allowedUnitsSize) const;

        virtual bool isUnitsWithFloat(const char* str, const char** allowedUnits, int allowedUnitsSize) const;
        virtual bool isUnitsWithInt(const char* str, const char** allowedUnits, int allowedUnitsSize) const;

        virtual bool stringToBoolean(const char* scope, const char* localName, const char* str) const;
        virtual int stringToInt(const char* scope, const char* localName, const char* str) const;
        virtual float stringToFloat(const char* scope, const char* localName, const char* str) const;
        virtual int stringToDurationSeconds(const char* scope, const char* localName, const char* str) const;
        virtual int stringToDurationMicroseconds(const char* scope, const char* localName, const char* str) const;
        virtual int stringToDurationMilliseconds(const char* scope, const char* localName, const char* str) const;
        virtual int stringToMemorySizeBytes(const char* scope, const char* localName, const char* str) const;
        virtual int stringToMemorySizeKB(const char* scope, const char* localName, const char* str) const;
        virtual int stringToMemorySizeMB(const char* scope, const char* localName, const char* str) const;
        virtual int stringToEnum(const char* scope, const char* localName, const char* typeName, const char* str,
                                 const EnumNameAndValue* enumInfo, int numEnums) const;
        virtual void stringToFloatWithUnits(const char* scope, const char* localName, const char* typeName, const char* str,
                                            const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                            const char*& unitsResult) const;
        virtual void stringToUnitsWithFloat(const char* scope, const char* localName, const char* typeName, const char* str,
                                            const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                            const char*& unitsResult) const;
        virtual void stringToIntWithUnits(const char* scope, const char* localName, const char* typeName, const char* str,
                                          const char** allowedUnits, int allowedUnitsSize, int& intResult,
                                          const char*& unitsResult) const;
        virtual void stringToUnitsWithInt(const char* scope, const char* localName, const char* typeName, const char* str,
                                          const char** allowedUnits, int allowedUnitsSize, int& intResult,
                                          const char*& unitsResult) const;

        virtual const char* lookupString(const char* scope, const char* localName, const char* defaultVal) const;
        virtual const char* lookupString(const char* scope, const char* localName) const;

        virtual void lookupList(const char* scope, const char* localName, std::vector<std::string>& data,
                                const char** defaultArray, int defaultArraySize) const;
        virtual void lookupList(const char* scope, const char* localName, std::vector<std::string>& data) const;

        virtual void lookupList(const char* scope, const char* localName, StringVector& list,
                                const StringVector& defaultList) const;
        virtual void lookupList(const char* scope, const char* localName, StringVector& list) const;

        virtual int lookupInt(const char* scope, const char* localName, int defaultVal) const;
        virtual int lookupInt(const char* scope, const char* localName) const;

        virtual float lookupFloat(const char* scope, const char* localName, float defaultVal) const;
        virtual float lookupFloat(const char* scope, const char* localName) const;

        virtual int lookupEnum(const char* scope, const char* localName, const char* typeName, const EnumNameAndValue* enumInfo,
                               int numEnums, const char* defaultVal) const;
        virtual int lookupEnum(const char* scope, const char* localName, const char* typeName, const EnumNameAndValue* enumInfo,
                               int numEnums, int defaultVal) const;
        virtual int lookupEnum(const char* scope, const char* localName, const char* typeName, const EnumNameAndValue* enumInfo,
                               int numEnums) const;

        virtual bool lookupBoolean(const char* scope, const char* localName, bool defaultVal) const;
        virtual bool lookupBoolean(const char* scope, const char* localName) const;

        virtual void lookupFloatWithUnits(const char* scope, const char* localName, const char* typeName,
                                          const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                          const char*& unitsResult) const;
        virtual void lookupFloatWithUnits(const char* scope, const char* localName, const char* typeName,
                                          const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                          const char*& unitsResult, float defaultFloat, const char* defaultUnits) const;

        virtual void lookupUnitsWithFloat(const char* scope, const char* localName, const char* typeName,
                                          const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                          const char*& unitsResult) const;
        virtual void lookupUnitsWithFloat(const char* scope, const char* localName, const char* typeName,
                                          const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                          const char*& unitsResult, float defaultFloat, const char* defaultUnits) const;

        virtual void lookupIntWithUnits(const char* scope, const char* localName, const char* typeName, const char** allowedUnits,
                                        int allowedUnitsSize, int& intResult, const char*& unitsResult) const;
        virtual void lookupIntWithUnits(const char* scope, const char* localName, const char* typeName, const char** allowedUnits,
                                        int allowedUnitsSize, int& intResult, const char*& unitsResult, int defaultInt,
                                        const char* defaultUnits) const;

        virtual void lookupUnitsWithInt(const char* scope, const char* localName, const char* typeName, const char** allowedUnits,
                                        int allowedUnitsSize, int& intResult, const char*& unitsResult) const;
        virtual void lookupUnitsWithInt(const char* scope, const char* localName, const char* typeName, const char** allowedUnits,
                                        int allowedUnitsSize, int& intResult, const char*& unitsResult, int defaultInt,
                                        const char* defaultUnits) const;

        virtual int lookupDurationMicroseconds(const char* scope, const char* localName, int defaultVal) const;
        virtual int lookupDurationMicroseconds(const char* scope, const char* localName) const;
        virtual int lookupDurationMilliseconds(const char* scope, const char* localName, int defaultVal) const;
        virtual int lookupDurationMilliseconds(const char* scope, const char* localName) const;
        virtual int lookupDurationSeconds(const char* scope, const char* localName, int defaultVal) const;
        virtual int lookupDurationSeconds(const char* scope, const char* localName) const;

        virtual int lookupMemorySizeBytes(const char* scope, const char* localName, int defaultVal) const;
        virtual int lookupMemorySizeBytes(const char* scope, const char* localName) const;
        virtual int lookupMemorySizeKB(const char* scope, const char* localName, int defaultVal) const;
        virtual int lookupMemorySizeKB(const char* scope, const char* localName) const;
        virtual int lookupMemorySizeMB(const char* scope, const char* localName, int defaultVal) const;
        virtual int lookupMemorySizeMB(const char* scope, const char* localName) const;

        virtual void lookupScope(const char* scope, const char* localName) const;

        //--------
        // Update operations.
        //--------
        virtual void insertString(const char* scope, const char* localName, const char* strValue);
        virtual void insertList(const char* scope, const char* localName, std::vector<std::string> data);

        virtual void insertList(const char* scope, const char* localName, const StringVector& vec);

        virtual void ensureScopeExists(const char* scope, const char* localName);
        virtual void remove(const char* scope, const char* localName);
        virtual void empty();

    protected:
        friend class ConfigParser;

        //--------
        // Operations called by ConfigParser
        //--------
        virtual void insertList(const char* name, const StringVector& list);
        inline ConfigScope* rootScope();
        inline ConfigScope* getCurrScope();
        inline void setCurrScope(ConfigScope* scope);

        void ensureScopeExists(const char* name, ConfigScope*& scope);

        void ensureScopeExists(const StringVector& vec, int firstIndex, int lastIndex, ConfigScope*& scope);

        bool isExecAllowed(const char* cmdLine, StringBuffer& trustedCmdLine);

        //--------
        // Helper operations
        //--------
        const ConfigItem* lookup(const char* fullyScopedName, const char* localName, bool startInRoot = false) const;
        const ConfigItem* lookupHelper(const ConfigScope* scope, const StringVector& vec) const;
        void stringValue(const char* fullyScopedName, const char* localName, const char*& str, ConfType& type) const;
        void listValue(const char* fullyScopedName, const char* localName, StringVector& list, ConfType& type) const;
        void listValue(const char* fullyScopedName, const char* localName, std::vector<std::string>& list, ConfType& type) const;
        virtual bool enumVal(const char* description, const EnumNameAndValue* enumInfo, int numEnums, int& val) const;

        void pushIncludedFilename(const char* fileName);
        void popIncludedFilename(const char* fileName);
        void checkForCircularIncludes(const char* fileName, int includeLineNum);

        int stringToMemorySizeGeneric(const char* typeName, SpellingAndValue unitsInfo[], int unitsInfoSize,
                                      const char* allowedSizes[], const char* scope, const char* localName,
                                      const char* str) const;

    protected:
        //--------
        // Instance variables
        //--------
        UidIdentifierProcessor m_uidIdentifierProcessor;
        Configuration* m_securityCfg;
        StringBuffer m_securityCfgScope;
        StringBuffer m_fileName;
        std::unique_ptr<ConfigScope> m_rootScope;
        ConfigScope* m_currScope;
        StringVector m_fileNameStack;
        ConfigurationImpl* m_fallbackCfg;
        bool m_amOwnerOfSecurityCfg;
        bool m_amOwnerOfFallbackCfg;

    private:
        //--------
        // The following are not implemented
        //--------
        ConfigurationImpl(const Configuration& ex);
        ConfigurationImpl& operator=(const Configuration& ex);
    };

    //--------
    // Implementation of inline operations
    //--------

    inline const char* ConfigurationImpl::fileName() const
    {
        return m_fileName.str().c_str();
    }

    inline ConfigScope* ConfigurationImpl::rootScope()
    {
        return m_rootScope.get();
    }

    inline ConfigScope* ConfigurationImpl::getCurrScope()
    {
        return m_currScope;
    }

    inline void ConfigurationImpl::setCurrScope(ConfigScope* scope)
    {
        m_currScope = scope;
    }
}
