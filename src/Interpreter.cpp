#include <math.h>
#include <iostream>
#include <string>

#include "Interpreter.h"

void Interpreter::InterpretAll(std::vector<std::string> programLines)
{
    for (size_t i = 0; i < programLines.size(); i++)
	{
		Lexer::TokenList tokens = Lexer::MakeTokens(programLines[i]);
		Parser::SyntaxNode node = Parser::Parse(tokens);
        std::cout << node.ToString() << std::endl;
		Interpreter::Interpret(node).ToString();
	}
}

Interpreter::ReturnType Interpreter::InterpretCommandPrint(Parser::SyntaxNode print)
{
    ReturnType val = Interpret(*print.getChild(0));
    if(val.getType() == INT)
    {
        std::cout << val.getData().i << std::endl;
    }
    else if(val.getType() == FLOAT)
    {
        std::cout << val.getData().f << std::endl;
    }
    else if(val.getType() == STRING)
    {
        std::cout << std::string(val.getData().s) << std::endl;
    }
	return ReturnType();
}

Interpreter::ReturnType::ReturnType(returnT t, std::string d)
{
    type = t;
    for(size_t i = 0; i < d.length(); i++)
    {
        data.s[i] = d[i];
    }
}

Interpreter::ReturnType Interpreter::InterpretString(Parser::SyntaxNode string)
{   
    return ReturnType(STRING, string.getData().getDataStr());
}

Interpreter::ReturnType Interpreter::Interpret(Parser::SyntaxNode root)
{   
    if(root.isNumber()) return Interpreter::InterpretNumOperator(root);
    else if (root.getData().getType() == Lexer::KEYWORD && root.getData().getDataStr() == "print") return InterpretCommandPrint(root);
    else if (root.getData().getType() == Lexer::STRING) return InterpretString(root);
    else return ReturnType();    
}

std::string Interpreter::ReturnType::ToString()
{
    if(type == STRING) return data.s;
    else if (type == INT) return std::to_string(data.i);
    else return std::to_string(data.f);
}

Interpreter::ReturnType Interpreter::InterpretNumOperator(Parser::SyntaxNode op)
{
	if (!op.hasChildren())
	{
		if (op.getData().getType() == Lexer::INT)
		{
			int val = op.getData().getDataNumerical<int>();
            return Interpreter::ReturnType(INT, val);
		}
		else
		{
			float val = op.getData().getDataNumerical<float>();
            return Interpreter::ReturnType(FLOAT, val);
		}
	}
	else
	{
        ReturnType leftR = Interpret(*op.getChild(0));
        ReturnType rightR = Interpret(*op.getChild(1));
		float left;
		float right;
        if(leftR.getType() == INT)
        {
            left = leftR.getData().i;
        }
        else if (leftR.getType() == FLOAT)
        {
            left = leftR.getData().f;
        }

        if(rightR.getType() == INT)
        {
            right = rightR.getData().i;
        }
        else if (rightR.getType() == FLOAT)
        {
            right = rightR.getData().f;
        }

        float val = 0;
		if (op.getData().getType() == Lexer::ADD)
		{
			val = left + right;
		}
		else if (op.getData().getType() == Lexer::SUB)
		{
			val = left - right;
		}
		else if (op.getData().getType() == Lexer::DIV)
		{
			val = left / right;
		}
		else if (op.getData().getType() == Lexer::MULT)
		{
			val = left * right;
		}
		else if (op.getData().getType() == Lexer::POW)
		{
			val = pow(left, right);
		}

        if(leftR.getType() == INT && rightR.getType() == INT)
        {
            return ReturnType(INT, static_cast<int>(val));
        }
        else
        {
            return ReturnType(FLOAT, val);
        }   
	}
}