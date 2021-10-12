#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "../Parser/Parser.h"

namespace Compiler
{
    enum Side {LEFT, RIGHT};

    void CompileAll(std::vector<std::string> programLines, std::string name);

    void Compile(Parser::SyntaxNode node);

    void CompilePrint(Parser::SyntaxNode node, bool newLine);

    std::pair<std::string, bool>  CheckForInput(Parser::SyntaxNode node);

    const static std::string newLinePrint = "\tpush\t-11\n\tcall\tGetStdHandle\n\tmov\tebx, eax\n\tmov\tecx, 1\n\tmov\tedx, 10\n\tmov\t[__o], edx\n\tmov\tedx, __o\n\tpush\t0\n\tlea\teax, [ebp-4]\n\tpush\teax\n\tpush\tecx\n\tpush\tedx\n\tpush\tebx\n\tcall\tWriteFile\n";

    int CompileMath(Parser::SyntaxNode node, int unfinishedPushes);

    void CompileInput(std::string prompt);

    void CompileVariableInit(Parser::SyntaxNode node);

    void CompileAssign(Parser::SyntaxNode node);

    bool IsLiteral(Parser::SyntaxNode node);

    bool IsLiteralTree(Parser::SyntaxNode* node, bool rest);

    std::string ShortenFloat(std::string f);

    static std::ofstream out;

    static std::string externs;
    static std::string bss;
    static std::string main;
    static std::string end;

    static int valId;
    static bool hasInput = false;

    enum DataType {INT, FLOAT, STRING, BOOL, NULLTYPE};
    
    struct Variable
    {
        private:
            std::string name;
            int size;
            DataType type;
            int depth;

        public:
            Variable(std::string n, DataType t) { name = n; type = t; depth = 0; size = 1; }

            Variable(std::string n, DataType t, int s) { name = n; type = t; depth = 0; size = s; }

            std::string getName() { return name; }

            int getSize() { return size; }

            DataType getType() { return type; }

            int getDepth() { return depth; }

            void setDepth(int d) { depth = d; }
    };

    class VariableHolder
    {
    private:
        std::vector<Variable> variables;

    public:
        VariableHolder() { variables = std::vector<Variable>(); }

        Variable* Find(std::string name);

        int getDepth(std::string name);

        DataType getType(std::string name);

        void Add(Variable var);
    };

    static VariableHolder programVariables = VariableHolder();
    static int intToStringCount = 0;

    std::string StringToInt();
}