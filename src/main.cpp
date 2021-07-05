#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <vector>

#include "Lexer.h"
#include "Error.h"
#include "Parser.h"
#include "Interpreter.h"

std::string ReadFile(std::string fileName)
{
	std::ifstream file(fileName);
	std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

	if (str != "")
	{
		return str;
	}
	else
	{
		Error::EmptyFileError();
	}
}

std::vector<std::string> Split(std::string s, std::string delimiter)
{
	std::vector<std::string> out;

	size_t pos = 0;
	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		std::string line = s.substr(0, pos);
		out.push_back(line);
		s.erase(0, pos + delimiter.length());
	}
		
	return out;
}

int main(int argc, char** argv)
{
	Lexer::Setup();

	if(argc == 1)
	{
		std::cout << "NO ARGUMENTS SPECIFIED, PLEASE USE:\nrun [file name] - to run a file\ngithub - to get the link to the github" << std::endl;
		exit(1);
	}

	std::string programFile = ReadFile("test.fth");
	programFile += "\n";

	std::vector<std::string> programLines = Split(programFile, "\n");

	for (size_t i = 0; i < programLines.size(); i++)
	{
		Lexer::TokenList tokens = Lexer::MakeTokens(programLines[i]);
		Parser::SyntaxNode node = Parser::Parse(tokens);
        std::cout << node.ToString() << std::endl;
		Interpreter::Interpret(node).ToString();
	}
	std::cout << std::endl;

	std::string end;
	std::cin >> end;
}