#pragma once

#include<string>

#include "../Parser/Parser.h"

namespace Interpreter
{
    union Data { int i; float f; char s[256]; bool b; };

    enum returnT {INT, FLOAT, STRING, VOID, BOOL};

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

        void setData(bool b) { data.b = b; }

        void setData(std::string s) 
        {
            for(size_t i = 0; i < 256; i++)
            {
                data.s[i] = '\0';
            }

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

        ReturnType(returnT t, bool b)
        {
            data.b = b;
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

    ReturnType InterpretCompOperator(Parser::SyntaxNode op);

	ReturnType Interpret(Parser::SyntaxNode root);

    ReturnType InterpretCommandPrint(Parser::SyntaxNode print, bool newLine);

    ReturnType InterpretCommandLet(Parser::SyntaxNode let);

    ReturnType InterpretCommandInput(Parser::SyntaxNode input);

    ReturnType InterpretString(Parser::SyntaxNode string);

    ReturnType InterpretBool(Parser::SyntaxNode boolean);

    ReturnType InterpretIdentifier(Parser::SyntaxNode identifier);

    ReturnType InterpretAssign(Parser::SyntaxNode assign);

    void InterpretAll(std::vector<std::string> programLines);
}