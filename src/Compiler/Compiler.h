#pragma once

#include <string>
#include <fstream>
#include <map>

#include "../Parser/Parser.h"

namespace Compiler
{
    void CompileAll(std::vector<std::string> programLines);

    void Compile(Parser::SyntaxNode node);

    void CompilePrint(Parser::SyntaxNode node);

    float InterpretLiteralArithmatic(Parser::SyntaxNode op);

    std::pair<std::string, bool>  CheckForInput(Parser::SyntaxNode node);

    void CompileInput(std::string prompt);

    static std::ofstream out;

    static std::string externs;
    static std::string bss;
    static std::string main;
    static std::string end;


    static int valId;
    static bool hasInput = false;

    class VariableHolder
    {
    private:
        std::map<std::string, int> variables;

    public:
        VariableHolder()
        {
            variables = std::map<std::string, int>();
        }

        int getStackDepth(std::string name)
        {
            std::map<std::string, int>::iterator it;
            it = variables.find(name);
            if(it != variables.end()) return it->second;
            else return 0;
        }

        void add(std::string name)
        {
            std::map<std::string, int>::iterator it;
            for(std::map<std::string, int>::iterator it = variables.begin(); it != variables.end(); it++)
            {
                it->second++;
            }
            variables.insert(std::pair<std::string, int>(name, 1));
        }
    };

    static VariableHolder variables = VariableHolder();
}