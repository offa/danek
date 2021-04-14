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

#include "FooConfiguration.h"
#include "FallbackConfiguration.h"
#include "FooConfigurationException.h"
#include "danek/Configuration.h"
#include "danek/SchemaValidator.h"
#include <string.h>

using danek::Configuration;
using danek::ConfigurationException;
using danek::SchemaValidator;

FooConfiguration::FooConfiguration(bool wantDiagnostics)
    : m_cfg(Configuration::create()),
      m_wantDiagnostics(wantDiagnostics),
      m_connectionTimeout(0),
      m_rpcTimeout(0),
      m_idleTimeout(0),
      m_logFile(""),
      m_logLevel(0),
      m_host(0),
      m_port(0)
{
}

FooConfiguration::~FooConfiguration()
{
    static_cast<Configuration*>(m_cfg)->destroy();
}

void FooConfiguration::parse(const char* cfgInput, const char* cfgScope, const char* secInput,
                             const char* secScope)
{
    Configuration* cfg = static_cast<Configuration*>(m_cfg);
    SchemaValidator sv;

    try
    {
        //--------
        // Set non-default security, if supplied
        // Parse config input, if supplied
        // Set fallback configuration
        //--------
        if (strcmp(secInput, "") != 0)
        {
            cfg->setSecurityConfiguration(secInput, secScope);
        }
        if (strcmp(cfgInput, "") != 0)
        {
            cfg->parse(cfgInput);
        }
        cfg->setFallbackConfiguration(Configuration::SourceType::String, FallbackConfiguration::getString());

        //--------
        // Perform schema validation.
        //--------
        sv.wantDiagnostics(m_wantDiagnostics);
        sv.parseSchema(FallbackConfiguration::getSchema());
        sv.validate(cfg->getFallbackConfiguration(), "", "", SchemaValidator::ForceMode::Required);
        sv.validate(cfg, cfgScope, "");

        //--------
        // Cache configuration variables in instance variables for
        // faster access.
        //--------
        m_connectionTimeout = cfg->lookupDurationMilliseconds(cfgScope, "connection_timeout");
        m_rpcTimeout = cfg->lookupDurationMilliseconds(cfgScope, "rpc_timeout");
        m_idleTimeout = cfg->lookupDurationMilliseconds(cfgScope, "idle_timeout");
        m_logFile = cfg->lookupString(cfgScope, "log.file");
        m_logLevel = cfg->lookupInt(cfgScope, "log.level");
        m_host = cfg->lookupString(cfgScope, "host");
        m_port = cfg->lookupInt(cfgScope, "port");
    }
    catch (const ConfigurationException& ex)
    {
        throw FooConfigurationException(ex.message());
    }
}
