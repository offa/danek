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
#include <memory>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

static void parseCmdLineArgs(int argc, char** argv, const char*& recipeFilename, const char*& scope);
static void usage();

int main(int argc, char** argv)
{
    auto parser = std::make_unique<RecipeFileParser>();
    const char* recipeFilename;
    const char* scope;
    StringVector recipeScopes;
    StringVector steps;
    StringVector ingredients;
    int i;
    int len;
    int i2;

    setlocale(LC_ALL, "");
    parseCmdLineArgs(argc, argv, recipeFilename, scope);

    //--------
    // Parse and error-check a file containing recipes.
    //--------
    try
    {
        parser->parse(recipeFilename, scope);
    }
    catch (const RecipeFileParserException& ex)
    {
        std::cerr << ex.what() << std::endl;
        return 1;
    }

    //--------
    // Print information about the recipes.
    //--------
    parser->listRecipeScopes(recipeScopes);
    len = recipeScopes.size();
    printf("There are %d recipes\n", len);
    for (i = 0; i < len; i++)
    {
        const char* name = parser->getRecipeName(recipeScopes[i].c_str());
        parser->getRecipeIngredients(recipeScopes[i].c_str(), ingredients);
        parser->getRecipeSteps(recipeScopes[i].c_str(), steps);
        printf("\nRecipe \"%s\":\n", name);
        int len2 = ingredients.size();
        printf("\tThis recipe has %d ingredients:\n", len2);
        for (i2 = 0; i2 < len2; i2++)
        {
            printf("\t\t\"%s\"\n", ingredients[i2].c_str());
        }
        len2 = steps.size();
        printf("\tThis recipe has %d steps:\n", len2);
        for (i2 = 0; i2 < len2; i2++)
        {
            printf("\t\t\"%s\"\n", steps[i2].c_str());
        }
    }

    return 0;
}

static void parseCmdLineArgs(int argc, char** argv, const char*& recipeFilename, const char*& scope)
{
    int i;

    recipeFilename = "";
    scope = "";

    for (i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0)
        {
            usage();
        }
        else if (strcmp(argv[i], "-recipes") == 0)
        {
            if (i == argc - 1)
            {
                usage();
            }
            recipeFilename = argv[i + 1];
            i++;
        }
        else if (strcmp(argv[i], "-scope") == 0)
        {
            if (i == argc - 1)
            {
                usage();
            }
            scope = argv[i + 1];
            i++;
        }
        else
        {
            fprintf(stderr, "Unrecognised option '%s'\n\n", argv[i]);
            usage();
        }
    }
    if (strcmp(recipeFilename, "") == 0)
    {
        usage();
    }
}

static void usage()
{
    fprintf(stderr,
        "\n"
        "usage: demo <options> -recipes <source>\n"
        "\n"
        "The <options> can be:\n"
        "  -h                Print this usage statement\n"
        "  -scope            scope within recipes file\n"
        "A <source> can be one of the following:\n"
        "  file.cfg          A file\n"
        "  file#file.cfg     A file\n"
        "  exec#<command>    Output from executing the specified command\n\n");
    exit(1);
}
