#pragma once

#include <string>
#include <vector>
#include <map>

namespace Lexer
{
	enum TokenType { INT, FLOAT, STRING, ADD, SUB, MULT, DIV, GREATER, LESS, GREATERE, LESSE, EQUAL, OPENBRACKET, CLOSEBRACKET, OPENBRACE, CLOSEBRACE, KEYWORD, IDENTIFIER, END, NUMBER, NULLTYPE, POW, ANY, TYPE };

	void Setup();

	std::string TokenTypeToString(TokenType t);

	union Data { int i; float f; char s[256]; };

	static std::string types[2] {"int", "float"};

	class Token
	{
	private:
		TokenType type;
		Data data = Data();

	public:
		Token() { type = NULLTYPE; }

		Token(TokenType tType) { type = tType; }

		Token(TokenType tType, int d) { type = tType; data.i = d; }

		Token(TokenType tType, float d) { type = tType; data.f = d; }

		Token(TokenType tType, std::string d);

		TokenType getType() { return type; }

		std::string ToString();

		template<typename T>
		T getDataNumerical()
		{
			if (type == INT)
			{
				return data.i;
			}
			else if (type == FLOAT)
			{
				return data.f;
			}
		}

		std::string getDataStr() { return data.s; }

		bool isNumber();

		bool operator==(const Token& rhs);

		static std::multimap<std::string, std::vector<Lexer::TokenType>> Keywords;
	};

	class TokenList
	{
	private:
		std::vector<Token> list = std::vector<Token>();

	public:
		Token get(int i) { return list[i]; }

		void add(Token t) { list.push_back(t); }

		int length() { return list.size(); }

		std::string ToString();
	};

	TokenList MakeTokens(std::string program);
}