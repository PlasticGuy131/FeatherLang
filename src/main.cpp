#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <streambuf>
#include <vector>
#include <algorithm>

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
		std::cout << "NO ARGUMENTS SPECIFIED, PLEASE USE:\nhelp - for help" << std::endl;
		exit(1);
	}

	if(std::strcmp(argv[1], "run") == 0)
	{
		std::cout << argv[1] << std::endl;
		if(argc == 2)
		{
			std::cout << "NO [file name] GIVEN, PLEASE USE:\nrun [file name]" << std::endl;
			exit(1);
		}
		else
		{				
			std::string programFile = ReadFile(argv[2]);
			programFile += "\n";

			std::vector<std::string> programLines = Split(programFile, "\n");
			
			Interpreter::InterpretAll(programLines);
			exit(1);
		}
	}
	else if (std::strcmp(argv[1], "git") == 0)
	{
		std::cout << "GITHUB ADDRESS: https://github.com/PlasticGuy131/FeatherLang" << std::endl;
		exit(1);
	}
	else if (std::strcmp(argv[1], "help") == 0)
	{
		std::cout << "ALL ARGUEMENTS:\nrun [file name] - to run a file\ngit - to get the link to the github\nhelp - for help" << std::endl;
		exit(1);
	}
	else
	{
		std::cout << "UNRECOGNISED ARGUMENT " << argv[1] << " PLEASE USE:\nrun [file name] - to run a file\ngit - to get the link to the github\nhelp - for help" << std::endl;
		exit(1);
	}
}