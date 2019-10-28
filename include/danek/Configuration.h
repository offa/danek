// Copyright (c) 2017-2019 offa
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

#include "danek/ConfType.h"
#include "danek/ConfigurationException.h"
#include "danek/StringBuffer.h"
#include "danek/StringVector.h"
#include <stddef.h>
#include <string.h>

namespace danek
{
    struct EnumNameAndValue
    {
        const char* name;
        int value;
    };

    //--------
    // Class Configuration
    //--------

    class Configuration
    {
    public:
        enum class SourceType
        {
            File,
            String,
            Exec
        };

        static Configuration* create();
        virtual void destroy();

        static void mergeNames(const char* scope, const char* localName, StringBuffer& fullyScopedName);

        static int mbstrlen(const char* str);

        virtual void setFallbackConfiguration(Configuration* cfg) = 0;
        virtual void setFallbackConfiguration(Configuration::SourceType sourceType, const char* source,
                                              const char* sourceDescription = "") = 0;
        virtual const Configuration* getFallbackConfiguration() = 0;

        virtual void setSecurityConfiguration(Configuration* cfg, bool takeOwnership, const char* scope = "") = 0;

        virtual void setSecurityConfiguration(const char* cfgInput, const char* scope = "") = 0;
        virtual void getSecurityConfiguration(const Configuration*& cfg, const char*& scope) = 0;

        virtual void parse(Configuration::SourceType sourceType, const char* source, const char* sourceDescription = "") = 0;
        inline void parse(const char* sourceTypeAndSource);

        virtual const char* fileName() const = 0;

        virtual void listFullyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                          StringVector& names) const = 0;
        virtual void listFullyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                          const char* filterPattern, StringVector& names) const = 0;
        virtual void listFullyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                          const StringVector& filterPatterns, StringVector& names) const = 0;

        virtual void listLocallyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                            StringVector& names) const = 0;
        virtual void listLocallyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                            const char* filterPattern, StringVector& names) const = 0;
        virtual void listLocallyScopedNames(const char* scope, const char* localName, ConfType typeMask, bool recursive,
                                            const StringVector& filterPatterns, StringVector& names) const = 0;

        virtual ConfType type(const char* scope, const char* localName) const = 0;

        virtual bool uidEquals(const char* s1, const char* s2) const = 0;
        virtual void expandUid(StringBuffer& spelling) = 0;
        virtual std::string unexpandUid(const char* spelling, StringBuffer& buf) const = 0;

        //--------
        // Dump part or all of the configuration
        //--------
        virtual void dump(StringBuffer& buf, bool wantExpandedUidNames) const = 0;

        virtual void dump(StringBuffer& buf, bool wantExpandedUidNames, const char* scope, const char* localName) const = 0;

        virtual bool isBoolean(const char* str) const = 0;
        virtual bool isInt(const char* str) const = 0;
        virtual bool isFloat(const char* str) const = 0;
        virtual bool isDurationMicroseconds(const char* str) const = 0;
        virtual bool isDurationMilliseconds(const char* str) const = 0;
        virtual bool isDurationSeconds(const char* str) const = 0;
        virtual bool isMemorySizeBytes(const char* str) const = 0;
        virtual bool isMemorySizeKB(const char* str) const = 0;
        virtual bool isMemorySizeMB(const char* str) const = 0;
        virtual bool isEnum(const char* str, const EnumNameAndValue* enumInfo, int numEnums) const = 0;
        virtual bool isFloatWithUnits(const char* str, const char** allowedUnits, int allowedUnitsSize) const = 0;
        virtual bool isIntWithUnits(const char* str, const char** allowedUnits, int allowedUnitsSize) const = 0;

        virtual bool isUnitsWithFloat(const char* str, const char** allowedUnits, int allowedUnitsSize) const = 0;
        virtual bool isUnitsWithInt(const char* str, const char** allowedUnits, int allowedUnitsSize) const = 0;

        virtual bool stringToBoolean(const char* scope, const char* localName, const char* str) const = 0;
        virtual int stringToInt(const char* scope, const char* localName, const char* str) const = 0;
        virtual float stringToFloat(const char* scope, const char* localName, const char* str) const = 0;
        virtual int stringToDurationSeconds(const char* scope, const char* localName, const char* str) const = 0;
        virtual int stringToDurationMilliseconds(const char* scope, const char* localName, const char* str) const = 0;
        virtual int stringToDurationMicroseconds(const char* scope, const char* localName, const char* str) const = 0;
        virtual int stringToMemorySizeBytes(const char* scope, const char* localName, const char* str) const = 0;
        virtual int stringToMemorySizeKB(const char* scope, const char* localName, const char* str) const = 0;
        virtual int stringToMemorySizeMB(const char* scope, const char* localName, const char* str) const = 0;
        virtual int stringToEnum(const char* scope, const char* localName, const char* typeName, const char* str,
                                 const EnumNameAndValue* enumInfo, int numEnums) const = 0;
        virtual void stringToFloatWithUnits(const char* scope, const char* localName, const char* typeName, const char* str,
                                            const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                            const char*& unitsResult) const = 0;
        virtual void stringToUnitsWithFloat(const char* scope, const char* localName, const char* typeName, const char* str,
                                            const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                            const char*& unitsResult) const = 0;
        virtual void stringToIntWithUnits(const char* scope, const char* localName, const char* typeName, const char* str,
                                          const char** allowedUnits, int allowedUnitsSize, int& intResult,
                                          const char*& unitsResult) const = 0;
        virtual void stringToUnitsWithInt(const char* scope, const char* localName, const char* typeName, const char* str,
                                          const char** allowedUnits, int allowedUnitsSize, int& intResult,
                                          const char*& unitsResult) const = 0;

        //--------
        // lookup<Type>() operations, with and without default values.
        //--------
        virtual const char* lookupString(const char* scope, const char* localName, const char* defaultVal) const = 0;
        virtual const char* lookupString(const char* scope, const char* localName) const = 0;

        virtual void lookupList(const char* scope, const char* localName, std::vector<std::string>& data,
                                const char** defaultArray, int defaultArraySize) const = 0;
        virtual void lookupList(const char* scope, const char* localName, std::vector<std::string>& data) const = 0;

        virtual void lookupList(const char* scope, const char* localName, StringVector& list,
                                const StringVector& defaultList) const = 0;
        virtual void lookupList(const char* scope, const char* localName, StringVector& list) const = 0;

        virtual int lookupInt(const char* scope, const char* localName, int defaultVal) const = 0;
        virtual int lookupInt(const char* scope, const char* localName) const = 0;

        virtual float lookupFloat(const char* scope, const char* localName, float defaultVal) const = 0;
        virtual float lookupFloat(const char* scope, const char* localName) const = 0;

        virtual int lookupEnum(const char* scope, const char* localName, const char* typeName, const EnumNameAndValue* enumInfo,
                               int numEnums, const char* defaultVal) const = 0;
        virtual int lookupEnum(const char* scope, const char* localName, const char* typeName, const EnumNameAndValue* enumInfo,
                               int numEnums, int defaultVal) const = 0;
        virtual int lookupEnum(const char* scope, const char* localName, const char* typeName, const EnumNameAndValue* enumInfo,
                               int numEnums) const = 0;

        virtual bool lookupBoolean(const char* scope, const char* localName, bool defaultVal) const = 0;
        virtual bool lookupBoolean(const char* scope, const char* localName) const = 0;

        virtual void lookupFloatWithUnits(const char* scope, const char* localName, const char* typeName,
                                          const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                          const char*& unitsResult) const = 0;
        virtual void lookupFloatWithUnits(const char* scope, const char* localName, const char* typeName,
                                          const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                          const char*& unitsResult, float defaultFloat, const char* defaultUnits) const = 0;

        virtual void lookupUnitsWithFloat(const char* scope, const char* localName, const char* typeName,
                                          const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                          const char*& unitsResult) const = 0;
        virtual void lookupUnitsWithFloat(const char* scope, const char* localName, const char* typeName,
                                          const char** allowedUnits, int allowedUnitsSize, float& floatResult,
                                          const char*& unitsResult, float defaultFloat, const char* defaultUnits) const = 0;

        virtual void lookupIntWithUnits(const char* scope, const char* localName, const char* typeName, const char** allowedUnits,
                                        int allowedUnitsSize, int& intResult, const char*& unitsResult) const = 0;
        virtual void lookupIntWithUnits(const char* scope, const char* localName, const char* typeName, const char** allowedUnits,
                                        int allowedUnitsSize, int& intResult, const char*& unitsResult, int defaultInt,
                                        const char* defaultUnits) const = 0;

        virtual void lookupUnitsWithInt(const char* scope, const char* localName, const char* typeName, const char** allowedUnits,
                                        int allowedUnitsSize, int& intResult, const char*& unitsResult) const = 0;
        virtual void lookupUnitsWithInt(const char* scope, const char* localName, const char* typeName, const char** allowedUnits,
                                        int allowedUnitsSize, int& intResult, const char*& unitsResult, int defaultInt,
                                        const char* defaultUnits) const = 0;

        virtual int lookupDurationMicroseconds(const char* scope, const char* localName, int defaultVal) const = 0;
        virtual int lookupDurationMicroseconds(const char* scope, const char* localName) const = 0;

        virtual int lookupDurationMilliseconds(const char* scope, const char* localName, int defaultVal) const = 0;
        virtual int lookupDurationMilliseconds(const char* scope, const char* localName) const = 0;

        virtual int lookupDurationSeconds(const char* scope, const char* localName, int defaultVal) const = 0;
        virtual int lookupDurationSeconds(const char* scope, const char* localName) const = 0;

        virtual int lookupMemorySizeBytes(const char* scope, const char* localName, int defaultVal) const = 0;
        virtual int lookupMemorySizeBytes(const char* scope, const char* localName) const = 0;
        virtual int lookupMemorySizeKB(const char* scope, const char* localName, int defaultVal) const = 0;
        virtual int lookupMemorySizeKB(const char* scope, const char* localName) const = 0;
        virtual int lookupMemorySizeMB(const char* scope, const char* localName, int defaultVal) const = 0;
        virtual int lookupMemorySizeMB(const char* scope, const char* localName) const = 0;

        virtual void lookupScope(const char* scope, const char* localName) const = 0;

        //--------
        // Update operations
        //--------
        virtual void insertString(const char* scope, const char* localName, const char* strValue) = 0;

        virtual void insertList(const char* scope, const char* localName, std::vector<std::string> data) = 0;

        virtual void insertList(const char* scope, const char* localName, const StringVector& vec) = 0;

        virtual void ensureScopeExists(const char* scope, const char* localName) = 0;

        virtual void remove(const char* scope, const char* localName) = 0;

        virtual void empty() = 0;

    protected:
        //--------
        // Available only to the implementation subclass
        //--------
        Configuration();
        virtual ~Configuration();

    private:
        //--------
        // Not implemented
        //--------
        Configuration(const Configuration& ex);
        Configuration& operator=(const Configuration& ex);
    };

    inline void Configuration::parse(const char* str)
    {
        if (strncmp(str, "exec#", 5) == 0)
        {
            parse(SourceType::Exec, &(str[5]));
        }
        else if (strncmp(str, "file#", 5) == 0)
        {
            parse(SourceType::File, &(str[5]));
        }
        else
        {
            parse(SourceType::File, str);
        }
    }
}
