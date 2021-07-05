#pragma once

#include "Lexer.h"

namespace Error
{
	void UnexpectedCharError(char unexpectedChar, int pos);

	void EmptyFileError();

	void EOFError(char expectedChar);

	void ParsingError(Lexer::Token token, Lexer::TokenType expectedType);
}
