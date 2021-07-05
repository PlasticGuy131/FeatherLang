#include <map>
#include <iostream>
#include <algorithm>

#include "Lexer.h"
#include "Error.h"


void Lexer::Setup()
{
	std::vector<TokenType> data = std::vector<TokenType>();
	data.push_back(ANY);
	Token::Keywords.emplace("print", data);
}

std::multimap<std::string, std::vector<Lexer::TokenType>> Lexer::Token::Keywords = std::multimap<std::string, std::vector<Lexer::TokenType>>();

std::string Lexer::TokenTypeToString(TokenType t)
{
	switch (t)
	{
	case INT:
		return "INT";
	case FLOAT:
		return "FLOAT";
	case STRING:
		return "STRING";
	case ADD:
		return "ADD";
	case SUB:
		return "SUB";
	case MULT:
		return "MULT";
	case DIV:
		return "DIV";
	case GREATER:
		return "GREATER";
	case LESS:
		return "LESS";
	case GREATERE:
		return "GREATERE";
	case LESSE:
		return "LESSE";
	case EQUAL:
		return "EQUAL";
	case OPENBRACKET:
		return "OPENBRACKET";
	case CLOSEBRACKET:
		return "CLOSEBRACKET";
	case OPENBRACE:
		return "OPENBRACE";
	case CLOSEBRACE:
		return "CLOSEBRACE";
	case KEYWORD:
		return "KEYWORD";
	case IDENTIFIER:
		return "IDENTIFIER";
	case END:
		return "END";
	case NUMBER:
		return "NUMBER";
	case NULLTYPE:
		return "NULLTYPE";
	case POW:
		return "POW";
	case TYPE:
		return "TYPE";
	default:
		return "";
	}
}

Lexer::Token::Token(TokenType tType, std::string d)
{
	type = tType;
	for (size_t i = 0; i < d.length(); i++)
	{
		data.s[i] = d[i];
	}
}

std::string Lexer::Token::ToString()
{
	std::string out = "[";
	out += TokenTypeToString(type);

	if (type == INT)
	{
		out += ":" + std::to_string(data.i);
	}
	else if (type == FLOAT)
	{
		out += ":" + std::to_string(data.f);
	}
	else if (type == STRING || type == IDENTIFIER || type == KEYWORD || type == TYPE)
	{
		out += ':';
		out += data.s;
	}
	out += "]";
	return out;
}

bool Lexer::Token::operator==(const Token &rhs)
{
	const Token* ptr = dynamic_cast<const Token*>(&rhs);
	if (ptr != nullptr)
	{
		if (type == ptr->type)
		{
            if(type == ANY) return true;
			if (type == INT || type == FLOAT || type == STRING || type == IDENTIFIER || type == KEYWORD)
			{
				return (type == ptr->type && (data.i == ptr->data.i || data.f == ptr->data.f || data.s == ptr->data.s));
			}
			else
			{
				return true;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

std::string Lexer::TokenList::ToString()
{
	std::string out = "[";
	for (size_t i = 0; i < list.size(); i++)
	{
		out += list[i].ToString();
		if (i < list.size() - 1)
		{
			out += ",";
		}
	}
	out += "]";
	return out;
}

bool Lexer::Token::isNumber()
{
	return (type == INT || type == FLOAT || type == ADD || type == MULT || type == SUB || type == DIV || type == POW);
}

Lexer::TokenList Lexer::MakeTokens(std::string program)
{
	TokenList tokens = TokenList();

	for (size_t pointer = 0; pointer < program.length(); pointer++)
	{
		if (isdigit(program[pointer]))
		{
			std::string num = "";
			bool point = false;

			while (pointer < program.length())
			{
				if (isdigit(program[pointer]) || program[pointer] == '.')
				{
					if (program[pointer] == '.')
					{
						if (!point)
						{
							point = true;
						}
						else
						{
							Error::UnexpectedCharError('.', pointer);
						}
					}

					num += program[pointer];
					pointer++;
				}
				else
				{
					break;
				}
			}

			if (!point)
			{
				tokens.add(Token(INT, std::stoi(num)));
			}
			else
			{
				tokens.add(Token(FLOAT, std::stof(num)));
			}
			pointer--;
		}
		else if (program[pointer] == '"')
		{
			std::string str = "";
			pointer++;

			while (pointer < program.length() && program[pointer] != '"')
			{
				if (pointer == program.length() - 1)
				{
					Error::EOFError('"');
				}
				else
				{
					str += program[pointer];
				}
				pointer++;
			}

			tokens.add(Token(STRING, str));
		}
		else if (program[pointer] == '+')
		{
			tokens.add(Token(ADD));
		}
		else if (program[pointer] == '-')
		{
			tokens.add(Token(SUB));
		}
		else if (program[pointer] == '*')
		{
			tokens.add(Token(MULT));
		}
		else if (program[pointer] == '/')
		{
			tokens.add(Token(DIV));
		}
		else if (program[pointer] == '=')
		{
			tokens.add(Token(EQUAL));
		}
		else if (program[pointer] == '(')
		{
			tokens.add(Token(OPENBRACKET));
		}
		else if (program[pointer] == ')')
		{
			tokens.add(Token(CLOSEBRACKET));
		}
		else if (program[pointer] == '{')
		{
			tokens.add(Token(OPENBRACE));
		}
		else if (program[pointer] == '}')
		{
			tokens.add(Token(CLOSEBRACE));
		}
		else if (program[pointer] == '^')
		{
			tokens.add(Token(POW));
		}
		else if (program[pointer] == '>')
		{
			if(program[pointer+1] == '=')
			{
				tokens.add(Token(GREATERE));
				pointer++;
			}
			else
			{
				tokens.add(Token(GREATER));
			}
		}
		else if (program[pointer] == '<')
		{
			if (program[pointer+1] == '=')
			{
				tokens.add(Token(LESSE));
				pointer++;
			}
			else
			{
				tokens.add(Token(LESS));
			}
		}
		else if (isalpha(program[pointer]) || program[pointer] == '_')
		{
			std::string str = "";

			while ((isalnum(program[pointer]) || program[pointer] == '_') && pointer < program.length() && program[pointer] != '(')
			{
				str += program[pointer];
				pointer++;
			}

			
			if (Lexer::Token::Keywords.find(str) != Lexer::Token::Keywords.end()) tokens.add(Token(KEYWORD, str));
			else if(std::count(std::begin(types), std::end(types), str)) tokens.add(Token(TYPE, str));
			else tokens.add(Token(IDENTIFIER, str));
			
			pointer--;
		}
	}

	return tokens;
}

