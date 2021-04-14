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

#include "RecipeFileParser.h"
#include <iostream>
#include <locale.h>
#include <memory>
#include <tuple>

static std::tuple<std::string, std::string> parseCmdLineArgs(int argc, char** argv);
static void usage();

int main(int argc, char** argv)
{
    auto parser = std::make_unique<RecipeFileParser>();
    StringVector recipeScopes;
    StringVector steps;
    StringVector ingredients;

    setlocale(LC_ALL, "");
    std::string recipeFilename;
    std::string scope;
    std::tie(recipeFilename, scope) = parseCmdLineArgs(argc, argv);

    //--------
    // Parse and error-check a file containing recipes.
    //--------
    try
    {
        parser->parse(recipeFilename.c_str(), scope.c_str());
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
    const std::size_t len = recipeScopes.size();
    std::cout << "There are " << len << " recipes\n";

    for (std::size_t i = 0; i < len; ++i)
    {
        const char* name = parser->getRecipeName(recipeScopes[i].c_str());
        parser->getRecipeIngredients(recipeScopes[i].c_str(), ingredients);
        parser->getRecipeSteps(recipeScopes[i].c_str(), steps);
        std::cout << "\nRecipe \"" << name << "\":\n";
        const std::size_t ingredientsLen = ingredients.size();
        std::cout << "\tThis recipe has " << ingredientsLen << " ingredients:\n";

        for (std::size_t i2 = 0; i2 < ingredientsLen; ++i2)
        {
            std::cout << "\t\t\"" << ingredients[i2].c_str() << "\"\n";
        }

        const std::size_t stepsLen = steps.size();
        std::cout << "\tThis recipe has " << stepsLen << " steps:\n";

        for (std::size_t i2 = 0; i2 < stepsLen; ++i2)
        {
            std::cout << "\t\t\"" << steps[i2].c_str() << "\"\n";
        }
    }

    return 0;
}

static std::tuple<std::string, std::string> parseCmdLineArgs(int argc, char** argv)
{
    std::string recipeFilename = "";
    std::string scope = "";

    for (int i = 1; i < argc; ++i)
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
            ++i;
        }
        else if (strcmp(argv[i], "-scope") == 0)
        {
            if (i == argc - 1)
            {
                usage();
            }
            scope = argv[i + 1];
            ++i;
        }
        else
        {
            std::cerr << "Unrecognised option '" << argv[i] << "'\n\n";
            usage();
        }
    }

    if (recipeFilename.empty() == true)
    {
        usage();
    }

    return std::make_tuple(recipeFilename, scope);
}

static void usage()
{
    std::cerr << "\n"
              << "usage: demo <options> -recipes <source>\n"
              << "\n"
              << "The <options> can be:\n"
              << "  -h                Print this usage statement\n"
              << "  -scope            scope within recipes file\n"
              << "A <source> can be one of the following:\n"
              << "  file.cfg          A file\n"
              << "  file#file.cfg     A file\n"
              << "  exec#<command>    Output from executing the specified command\n\n";
    exit(1);
}
