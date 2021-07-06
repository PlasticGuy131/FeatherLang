#include <math.h>
#include <iostream>
#include <string>
#include <vector>

#include "Error.h"
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

std::string Interpreter::Variable::ToString()
{
    std::string out = "";
    if(type == INT) out += "int";
    else if (type == FLOAT) out += "float";
    out += " ";
    out += name;
    out += " = ";
    if(type == INT) out += std::to_string(data.i);
    else if (type == FLOAT) out += std::to_string(data.f);
    return out;
}

Interpreter::ReturnType Interpreter::InterpretIdentifier(Parser::SyntaxNode identifier)
{
    if(identifier.getParent() == nullptr)
    {
        return ReturnType();
    }
    else
    {
        bool found;
        for(size_t i = 0; i < variables.size(); i++)
        {
            if(variables[i].getName() == identifier.getData().getDataStr())
            {
                found = true;
                return ReturnType(variables[i].getType(), variables[i].getData().i);
            }
        }
        if(!found) Error::UnexpectedIdentifier(identifier.getData().getDataStr());
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
        std::string out = std::string(val.getData().s);
        std::cout << out << std::endl;
    }
	return ReturnType();
}

Interpreter::ReturnType Interpreter::InterpretCommandLet(Parser::SyntaxNode let)
{
    returnT type;
    Data d = Data();
    if(let.getChild(0)->getData().getDataStr() == "int") 
    {
        type = INT;
        d.i = 0;
    }
    else if(let.getChild(0)->getData().getDataStr() == "float")
    {
        type = FLOAT;
        d.f = 0;
    }
    
    variables.push_back(Variable(let.getChild(0)->getChild(0)->getData().getDataStr(), type, d));
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

Interpreter::ReturnType Interpreter::InterpretAssign(Parser::SyntaxNode assign)
{
    bool found;
    for(size_t i = 0; i < variables.size(); i++)
    {
        if(variables[i].getName() == assign.getChild(0)->getData().getDataStr())
        {
            found = true;
            variables[i].setData(Interpret(*assign.getChild(1)).getData().i);
        }
    }
    if(!found) Error::UnexpectedIdentifier(assign.getChild(0)->getData().getDataStr());
    return ReturnType();    
}

Interpreter::ReturnType Interpreter::Interpret(Parser::SyntaxNode root)
{   
    if(root.isNumber()) return Interpreter::InterpretNumOperator(root);
    else if (root.getData().getType() == Lexer::KEYWORD && root.getData().getDataStr() == "print") return InterpretCommandPrint(root);
    else if (root.getData().getType() == Lexer::STRING) return InterpretString(root);
    else if (root.getData().getType() == Lexer::KEYWORD && root.getData().getDataStr() == "let") return InterpretCommandLet(root);
    else if (root.getData().getType() == Lexer::IDENTIFIER) return InterpretIdentifier(root);
    else if (root.getData().getType() == Lexer::ASSIGN) return InterpretAssign(root);
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