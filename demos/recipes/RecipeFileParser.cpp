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

#include "RecipeFileParser.h"
#include "danek/SchemaValidator.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

using danek::Configuration;
using danek::ConfigurationException;
using danek::SchemaValidator;

RecipeFileParser::RecipeFileParser() : m_cfg(nullptr), m_parseCalled(false)
{
}

RecipeFileParser::~RecipeFileParser()
{
    m_cfg->destroy();
}

void RecipeFileParser::parse(const char* recipeFilename, const char* scope)
{
    SchemaValidator sv;
    StringBuffer filter;

    assert(!m_parseCalled);
    m_cfg = Configuration::create();
    m_scope = scope;
    Configuration::mergeNames(scope, "uid-recipe", filter);

    try
    {
        const char* schema[] = {"uid-recipe = scope",
            "uid-recipe.ingredients = list[string]",
            "uid-recipe.name = string",
            "uid-recipe.uid-step = string",
            0};

        m_cfg->parse(recipeFilename);
        sv.parseSchema(schema);
        sv.validate(m_cfg, m_scope.str().c_str(), "");
        m_cfg->listFullyScopedNames(m_scope.str().c_str(), "", ConfType::Scope, false, filter.str().c_str(), m_recipeScopeNames);
    }
    catch (const ConfigurationException& ex)
    {
        throw RecipeFileParserException(ex.c_str());
    }
    m_parseCalled = true;
}

void RecipeFileParser::listRecipeScopes(StringVector& vec)
{
    assert(m_parseCalled);
    vec = m_recipeScopeNames;
}

const char* RecipeFileParser::getRecipeName(const char* recipeScope)
{
    assert(m_parseCalled);
    try
    {
        return m_cfg->lookupString(recipeScope, "name");
    }
    catch (const ConfigurationException& ex)
    {
        throw RecipeFileParserException(ex.c_str());
    }
}

void RecipeFileParser::getRecipeIngredients(const char* recipeScope, StringVector& result)
{
    assert(m_parseCalled);
    try
    {
        m_cfg->lookupList(recipeScope, "ingredients", result);
    }
    catch (const ConfigurationException& ex)
    {
        throw RecipeFileParserException(ex.c_str());
    }
}

void RecipeFileParser::getRecipeSteps(const char* recipeScope, StringVector& result)
{
    int len;
    StringVector namesVec;

    assert(m_parseCalled);
    try
    {
        m_cfg->listLocallyScopedNames(
            recipeScope, "", ConfType::String, false, "uid-step", namesVec);
    }
    catch (const ConfigurationException& ex)
    {
        throw RecipeFileParserException(ex.c_str());
    }
    result.clear();
    len = namesVec.size();
    try
    {
        for (int i = 0; i < len; i++)
        {
            assert(m_cfg->uidEquals("uid-step", namesVec[i].c_str()));
            const char* str = m_cfg->lookupString(recipeScope, namesVec[i].c_str());
            result.push_back(str);
        }
    }
    catch (const ConfigurationException&)
    {
        abort(); // Bug!
    }
}
