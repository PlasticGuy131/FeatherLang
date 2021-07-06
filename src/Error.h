#pragma once

#include "Lexer.h"
#include "Interpreter.h"

namespace Error
{
	void UnexpectedCharError(char unexpectedChar, int pos);

	void EmptyFileError();

	void EOFError(char expectedChar);

	void ParsingError(Lexer::Token token, Lexer::TokenType expectedType);

	void UnexpectedIdentifierError(std::string name);

	void TypeError(Interpreter::returnT shouldBe, Interpreter::returnT type);
}
