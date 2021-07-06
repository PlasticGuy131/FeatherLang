#include <iostream>
#include <string>

#include "Lexer.h"
#include "Error.h"
#include "Interpreter.h"

void Error::UnexpectedCharError(char unexpectedChar, int pos)
{
	std::string out = "UnexpectedCharError: Unexpected Character: ";
	out += unexpectedChar;
	out += " at postion: ";
	out += std::to_string(pos);
	out += "!";
	std::cout << out << std::endl;
	exit(EXIT_FAILURE);
}

void Error::UnexpectedIdentifierError(std::string identifier)
{
	std::string out = "UnexpectedIdentifierError: Unexpected Identifer: ";
	out += identifier;
	std::cout << out << std::endl;
	exit(EXIT_FAILURE);
}

void Error::EmptyFileError()
{
	std::string out = "EmptyFileError: File is empty or file does not exist!";
	std::cout << out << std::endl;
	exit(EXIT_FAILURE);
}

void Error::EOFError(char expectedChar)
{
	std::string out = "EOFError: Expected Character: ";
	out += expectedChar;
	out += " before end of file!";
	std::cout << out << std::endl;
	exit(EXIT_FAILURE);
}

void Error::ParsingError(Lexer::Token token, Lexer::TokenType expectedType)
{
	std::string out = "ParsingError: Unexpected token: ";
	out += token.ToString();
	out += ", expected type: ";
	out += Lexer::TokenTypeToString(expectedType);
	out += "!";
	std::cout << out << std::endl;
	exit(EXIT_FAILURE);
}

void Error::TypeError(Interpreter::returnT shouldBe, Interpreter::returnT type)
{
	std::string out = "TypeError: Cannot change type: ";
	if(type == Interpreter::INT) out += "int";
	else if(type == Interpreter::FLOAT) out += "float";
	else if(type == Interpreter::STRING) out += "string";
	out += " to type: ";
	if(shouldBe == Interpreter::INT) out += "int";
	else if(shouldBe == Interpreter::FLOAT) out += "float";
	else if(shouldBe == Interpreter::STRING) out += "string";
	out += "!";
	std::cout << out << std::endl;
	exit(EXIT_FAILURE);
}