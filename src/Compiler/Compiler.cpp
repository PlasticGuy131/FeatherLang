#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "Compiler.h"
#include "../Interpreter/Interpreter.h"

void Compiler::CompileAll(std::vector<std::string> programLines)
{
    out = std::ofstream("main.asm");
    externs = "global _main\n\textern\tExitProcess\n\textern\tAllocConsole\n";
    bss = "section .bss\n";
    main = "section .text\n_main:\n";
    for (size_t i = 0; i < programLines.size(); i++)
	{
        if(programLines[i].length() < 1) continue;
		Lexer::TokenList tokens = Lexer::MakeTokens(programLines[i]);
		Parser::SyntaxNode node = Parser::Parse(tokens);
		Compile(node);
	}
    main += "\tcall\tExitProcess\n\thlt\n";
    out << externs;
    out << bss;
    out << main;
    out << end;
}

std::pair<std::string, bool> Compiler::CheckForInput(Parser::SyntaxNode node)
{
    for (int i = 0; i < node.childrenCount(); i++)
    {
        if(node.getChild(i)->getData().getType() == Lexer::KEYWORD && node.getChild(i)->getData().getDataStr() == "input") return std::pair<std::string, bool>(node.getChild(i)->getChild(0)->getData().getDataStr(), true);
        else return CheckForInput(*node.getChild(i));
    }
    return std::pair<std::string, bool>("", false);
}

void Compiler::Compile(Parser::SyntaxNode node)
{
    std::pair<std::string, bool> inputCheck = CheckForInput(node);
    if(inputCheck.second)
    {
        CompileInput(inputCheck.first);
    }
    if(node.getData().getType() == Lexer::KEYWORD && node.getData().getDataStr() == "print") CompilePrint(node);
}

void Compiler::CompileInput(std::string prompt)
{
    Parser::SyntaxNode promptNode = Parser::SyntaxNode(Lexer::Token(Lexer::KEYWORD, "print"), std::vector<Parser::SyntaxNode*>{new Parser::SyntaxNode(Lexer::Token(Lexer::STRING, prompt))});
    CompilePrint(promptNode);
    std::string inName = "__input";
    if(!hasInput)
    {
        externs += "\textern\tReadConsoleA\n";
        //variables.add(inName); 
        bss += inName;
        bss += ":\tresb 32\nn:\tresw 1\n";
        hasInput = true;
    }
    main += "\tpush\t-10\n\tcall\tGetStdHandle\n\tmov\tebx, eax\n\tpush\t0\n\tpush\tn\n\tpush\t32\n\tpush\t";
    main += inName;
    main += "\n\tpush\tebx\n\tcall\tReadConsoleA\n";
}

float Compiler::InterpretLiteralArithmatic(Parser::SyntaxNode op)
{
    if(op.getChild(0)->isNumber() && op.getChild(1)->isNumber()) return Interpreter::InterpretNumOperator(op).getData().f; 
    else return 0;
}

void Compiler::CompilePrint(Parser::SyntaxNode node)
{
    if(valId == 0) externs += "\textern\tGetStdHandle\n\textern\tWriteFile\n";
    std::string message = "";
    Lexer::TokenType printType = node.getChild(0)->getData().getType();
    bool literal = true;
    if(printType == Lexer::STRING) message = node.getChild(0)->getData().getDataStr();
    else if(printType == Lexer::INT) message = std::to_string(node.getChild(0)->getData().getDataNumerical<int>());
    else if(printType == Lexer::FLOAT)
    {
        message = std::to_string(node.getChild(0)->getData().getDataNumerical<float>());
        while (message[message.length()-1] == '0') message.pop_back();
    }
    else if(node.getChild(0)->isNumber())
    {
        Parser::SyntaxNode* op = node.getChild(0);
        message = std::to_string(InterpretLiteralArithmatic(*op));
        while (message[message.length()-1] == '0' || message[message.length()-1] == '.') message.pop_back();
    }
    else if(printType == Lexer::BOOL)
    {
        if (node.getChild(0)->getData().getDataBool()) message = "true";
        else message = "false";
    }
    else if(printType == Lexer::KEYWORD && node.getChild(0)->getData().getDataStr() == "input")
    {
        literal = false;
        main += "\tpush\t-11\n\tcall\tGetStdHandle\n\tmov\tebx, eax\n\tpush\t0\n\tlea\teax, [ebp-4]\n\tpush\teax\n\tpush\t32\n\tpush\t__input\n\tpush\tebx\n\tcall\tWriteFile\n";
    }

    if(literal)
    {
        main += "\tpush\t-11\n\tcall\tGetStdHandle\n\tmov\tebx, eax\n\tpush\t0\n\tlea\teax, [ebp-4]\n\tpush\teax\n\tpush\t";
        main += std::to_string(message.length() + 1);
        main += "\n\tpush\t";
        main += "l" + std::to_string(valId);
        main += "\n\tpush\tebx\n\tcall\tWriteFile\n\tpush\t0\n";
        end +=  "l" + std::to_string(valId);
        end += ": db '";
        end += message;
        end += "', 10\n";  
        valId++;
    }
}