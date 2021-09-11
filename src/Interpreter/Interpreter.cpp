#include <math.h>
#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#include "../Error/Error.h"
#include "Interpreter.h"

void Interpreter::InterpretAll(std::vector<std::string> programLines)
{
    for (size_t i = 0; i < programLines.size(); i++)
	{
        if(programLines[i].length() < 1) continue;
		Lexer::TokenList tokens = Lexer::MakeTokens(programLines[i]);
		Parser::SyntaxNode node = Parser::Parse(tokens);
		Interpreter::Interpret(node);
	}
}

std::string Interpreter::Variable::ToString()
{
    std::string out = "";
    if(type == INT) out += "int";
    else if (type == FLOAT) out += "float";
    else if (type == STRING) out += "string";
    out += " ";
    out += name;
    out += " = ";
    if(type == INT) out += std::to_string(data.i);
    else if (type == FLOAT) out += std::to_string(data.f);
    else if (type == STRING) out += std::string(data.s);
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
                if(variables[i].getType() == INT) return ReturnType(variables[i].getType(), variables[i].getData().i);
                else if(variables[i].getType() == FLOAT) return ReturnType(variables[i].getType(), variables[i].getData().f);
                else if(variables[i].getType() == STRING) return ReturnType(variables[i].getType(), std::string(variables[i].getData().s));
                else if(variables[i].getType() == BOOL) return ReturnType(variables[i].getType(), variables[i].getData().b);
            }
        }
        if(!found) Error::UnexpectedIdentifierError(identifier.getData().getDataStr());
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
    else if(val.getType() == BOOL)
    {  
        std::string out = "false";
        if(val.getData().b) out = "true";
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
    else if(let.getChild(0)->getData().getDataStr() == "string")
    {
        type = STRING;
        for(int i = 0; i < 256; i++)
        {
            d.s[i] = '\0';
        }
    }
    else if(let.getChild(0)->getData().getDataStr() == "bool")
    {
        type = BOOL;
        d.b = false;
    }

   

    if(let.childrenCount() > 1)
    {
        ReturnType set = Interpret(*let.getChild(2));
        if(type == INT) 
        {
            if(set.getType() == INT) d.i = set.getData().i;
            else Error::TypeError(INT, set.getType());
        }
        else if(type == FLOAT)
        {
            if(set.getType() == FLOAT) d.f = set.getData().f;
            else if(set.getType() == INT) d.f = set.getData().i;
            else Error::TypeError(FLOAT, set.getType());
        }
        else if(type == STRING)
        {
            if(set.getType() == STRING)
            {
                for(int i = 0; i < 256; i++)
                {
                    d.s[i] = set.getData().s[i];
                } 
            }
            else Error::TypeError(STRING, set.getType());
        }
        else if(type == BOOL)
        {
            if(set.getType() == BOOL) d.b = set.getData().b;
            else Error::TypeError(BOOL, set.getType());
        }
    }

    variables.push_back(Variable(let.getChild(0)->getChild(0)->getData().getDataStr(), type, d));
    
	return ReturnType();
}

Interpreter::ReturnType Interpreter::InterpretCommandInput(Parser::SyntaxNode input)
{
    std::string promt = std::string(Interpret(*input.getChild(0)).getData().s);
    std::cout << promt << std::endl;

    std::string val;
    std::cin >> val;

	return ReturnType(STRING, val);
}

Interpreter::ReturnType::ReturnType(returnT t, std::string d)
{
    type = t;
    for(int i = 0; i < 256; i++)
    {
        data.s[i] = '\0';
    }
    for(size_t i = 0; i < d.length(); i++)
    {
        data.s[i] = d[i];
    }
}

Interpreter::ReturnType Interpreter::InterpretString(Parser::SyntaxNode string)
{   
    ReturnType out = ReturnType(STRING, std::string(string.getData().getDataStr()));
    return out;
}

Interpreter::ReturnType Interpreter::InterpretBool(Parser::SyntaxNode boolean)
{   
    ReturnType out = ReturnType(BOOL, boolean.getData().getDataBool());
    return out;
}

Interpreter::ReturnType Interpreter::InterpretAssign(Parser::SyntaxNode assign)
{
    bool found;
    for(size_t i = 0; i < variables.size(); i++)
    {
        if(variables[i].getName() == assign.getChild(0)->getData().getDataStr())
        {
            found = true;
            ReturnType set = Interpret(*assign.getChild(1));
            if(variables[i].getType() == INT) 
            {
                if(set.getType() == INT) variables[i].setData(set.getData().i);
                else Error::TypeError(INT, set.getType());
            }
            else if(variables[i].getType() == FLOAT)
            {
                if(set.getType() == FLOAT) variables[i].setData(set.getData().f);
                else if(set.getType() == INT) variables[i].setData((float) set.getData().i);
                else Error::TypeError(FLOAT, set.getType());
            }
            else if(variables[i].getType() == STRING)
            {
                if(set.getType() == STRING) variables[i].setData(std::string(set.getData().s));
                else Error::TypeError(STRING, set.getType());
            }
            else if(variables[i].getType() == BOOL)
            {
                if(set.getType() == BOOL) variables[i].setData(set.getData().b);
                else Error::TypeError(BOOL, set.getType());
            }
        }
    }
    if(!found) Error::UnexpectedIdentifierError(assign.getChild(0)->getData().getDataStr());
    return ReturnType();
}

Interpreter::ReturnType Interpreter::Interpret(Parser::SyntaxNode root)
{   
    if (root.isNumber()) return Interpreter::InterpretNumOperator(root);
    else if (root.getData().getType() == Lexer::KEYWORD && root.getData().getDataStr() == "print") return InterpretCommandPrint(root);
    else if (root.getData().getType() == Lexer::STRING) return InterpretString(root);
    else if (root.getData().getType() == Lexer::KEYWORD && root.getData().getDataStr() == "let") return InterpretCommandLet(root);
    else if (root.getData().getType() == Lexer::IDENTIFIER) return InterpretIdentifier(root);
    else if (root.getData().getType() == Lexer::ASSIGN) return InterpretAssign(root);
    else if (root.isComp()) return Interpreter::InterpretCompOperator(root);
    else if (root.getData().getType() == Lexer::BOOL) return InterpretBool(root);
    else if (root.getData().getType() == Lexer::KEYWORD && root.getData().getDataStr() == "input") return InterpretCommandInput(root);
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

        if(leftR.getType() == STRING || leftR.getType() == STRING)
        {
           if(leftR.getType() == STRING && op.getData().getType() == Lexer::ADD && rightR.getType() == STRING)
           {
               std::string l = "";
               std::string r = "";
               l = leftR.getData().s;
               r = rightR.getData().s;
               return ReturnType(STRING, l + r);
           }
           else if (leftR.getType() == STRING && op.getData().getType() == Lexer::MULT && rightR.getType() == INT)
           {
               std::string l = "";
               int r = rightR.getData().i;
               l = leftR.getData().s;
               std::string out = "";
               for(int i = 0; i < r; i++)
               {
                   out += l;
               }
               return ReturnType(STRING, out);
           }
           else Error::TypeError(STRING, INT);
        }
        else
        {       
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
}

Interpreter::ReturnType Interpreter::InterpretCompOperator(Parser::SyntaxNode op)
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

        if(leftR.getType() == STRING || leftR.getType() == STRING) Error::TypeError(STRING, INT);
        else
        {       
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

            bool val = false;
            if (op.getData().getType() == Lexer::EQUAL)
            {
                val = left == right;
            }
            else if (op.getData().getType() == Lexer::LESS)
            {
                val = left < right;
            }
            else if (op.getData().getType() == Lexer::LESSE)
            {
                val = left <= right;
            }
            else if (op.getData().getType() == Lexer::GREATER)
            {
                val = left > right;
            }
            else if (op.getData().getType() == Lexer::GREATERE)
            {
                val = left >= right;
            }

            return ReturnType(BOOL, val);
        } 
	}
}