#pragma once

#include<string>

#include "Parser.h"

namespace Interpreter
{
    union Data { int i; float f; char s[256]; };

    enum returnT {INT, FLOAT, STRING, VOID};

    class Variable
    {
    private:
        Data data;
        std::string name;
        returnT type;

    public:
        Variable(std::string n, returnT t, Data d)
        {
            name = n;
            data = d;
            type = t;
        }

        Data getData() { return data; }

        std::string getName() { return name; }

        returnT getType() { return type; }

        void setData(int d) { data.i = d; }

        void setData(float f) { data.f = f; }

        void setData(std::string s) 
        {
            for(size_t i = 0; i < s.length(); i++)
            {
                data.s[i] = s[i];
            }
        }

        std::string ToString();
    };

    static std::vector<Variable> variables = std::vector<Variable>();

    class ReturnType 
    {
    private:
        Data data;
        returnT type;
        
    public:
        ReturnType()
        {
            type = VOID;
        }

        ReturnType(returnT t, int d)
        {
            data.i = d;
            type = t;
        }

        ReturnType(returnT t, float d)
        {
            data.f = d;
            type = t;
        }

        ReturnType(returnT t, std::string d);

        Data getData() {return data; }

        returnT getType() {return type; }

        std::string ToString();
    };

	ReturnType InterpretNumOperator(Parser::SyntaxNode op);

	ReturnType Interpret(Parser::SyntaxNode root);

    ReturnType InterpretCommandPrint(Parser::SyntaxNode print);

    ReturnType InterpretCommandLet(Parser::SyntaxNode let);

    ReturnType InterpretString(Parser::SyntaxNode string);

    ReturnType InterpretIdentifier(Parser::SyntaxNode identifier);

    ReturnType InterpretAssign(Parser::SyntaxNode assign);

    void InterpretAll(std::vector<std::string> programLines);
}