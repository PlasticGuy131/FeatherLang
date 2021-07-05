#pragma once

#include<string>

#include "Parser.h"

namespace Interpreter
{
    union Data { int i; float f; char s[256]; };

    enum returnT {INT, FLOAT, STRING, VOID};

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

    ReturnType InterpretString(Parser::SyntaxNode string);
}