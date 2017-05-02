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
#include "danek/Configuration.h"
#include "danek/SchemaValidator.h"
#include <string.h>
#include <stdlib.h>

using danek::Configuration;
using danek::ConfigurationException;
using danek::SchemaValidator;

//----------------------------------------------------------------------
// class FooConfigurationException
//----------------------------------------------------------------------

FooConfigurationException::FooConfigurationException(const char* str)
{
    m_str = new char[strlen(str) + 1];
    strcpy(m_str, str);
}

FooConfigurationException::FooConfigurationException(const FooConfigurationException& other)
{
    m_str = new char[strlen(other.m_str) + 1];
    strcpy(m_str, other.m_str);
}

FooConfigurationException::~FooConfigurationException()
{
    delete[] m_str;
}

const char* FooConfigurationException::c_str() const
{
    return m_str;
}

//----------------------------------------------------------------------
// class FooConfiguration
//----------------------------------------------------------------------

FooConfiguration::FooConfiguration() : m_cfg(Configuration::create()), m_logLevels()
{
}

FooConfiguration::~FooConfiguration()
{
    ((Configuration*) m_cfg)->destroy();
}

void FooConfiguration::parse(const char* cfgInput, const char* cfgScope, const char* secInput,
    const char* secScope) throw(FooConfigurationException)
{
    SchemaValidator sv;
    Configuration* cfg = (Configuration*) m_cfg;

    try
    {
        const char* schema[] = {
            "@typedef logLevel = int[0, 4]",
            "log_levels = table[string,operation-name, logLevel,log-level]",
            0,
        };

        //--------
        // Set non-default security, if supplied. Then parse the
        // configuration input. Finally, perform schema validation.
        //--------
        if (strcmp(secInput, "") != 0)
        {
            cfg->setSecurityConfiguration(secInput, secScope);
        }
        cfg->parse(cfgInput);
        sv.parseSchema(schema);
        sv.validate(cfg, cfgScope, "");

        //--------
        // Cache configuration variables in instance variables for
        // faster access.
        //--------
        cfg->lookupList(cfgScope, "log_levels", m_logLevels);
    }
    catch (const ConfigurationException& ex)
    {
        throw FooConfigurationException(ex.c_str());
    }
}

Logger::LogLevel FooConfiguration::getLogLevel(const char* opName) const
{
    int result;

    for ( std::size_t i = 0; i < m_logLevels.size(); i += 2)
    {
        const char* pattern = m_logLevels[i + 0].c_str();
        const char* logLevelStr = m_logLevels[i + 1].c_str();
        if (Configuration::patternMatch(opName, pattern))
        {
            result = atoi(logLevelStr);
            if (result > (int) Logger::LogLevel::Debug)
            {
                result = (int) Logger::LogLevel::Debug;
            }
            else if (result < 0)
            {
                result = 0;
            }
            return (Logger::LogLevel) result;
        }
    }
    return Logger::LogLevel::Error; // default log level
}
