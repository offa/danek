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

class FooConfiguration
{
public:
    explicit FooConfiguration(bool wantDiagnostics = false);
    FooConfiguration(const FooConfiguration&) = delete;
    ~FooConfiguration();

    void parse(const char* cfgInput, const char* cfgScope = "", const char* secInput = "",
               const char* secScope = "");

    int getConnectionTimeout()
    {
        return m_connectionTimeout;
    }

    int getRpcTimeout()
    {
        return m_rpcTimeout;
    }

    int getIdleTimeout()
    {
        return m_idleTimeout;
    }

    const char* getLogFile()
    {
        return m_logFile;
    }

    int getLogLevel()
    {
        return m_logLevel;
    }

    const char* getHost()
    {
        return m_host;
    }

    int getPort()
    {
        return m_port;
    }

    FooConfiguration& operator=(const FooConfiguration&) = delete;


private:
    void* m_cfg; // opaque pointer to Config4Cpp config object
    bool m_wantDiagnostics;

    //--------
    // Instance variables to cache configuration variables.
    //--------
    int m_connectionTimeout;
    int m_rpcTimeout;
    int m_idleTimeout;
    const char* m_logFile;
    int m_logLevel;
    const char* m_host;
    int m_port;
};
