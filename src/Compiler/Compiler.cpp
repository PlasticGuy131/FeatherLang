#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include "Compiler.h"
#include "../Interpreter/Interpreter.h"

Compiler::Variable* Compiler::VariableHolder::Find(std::string name)
{
    for (size_t i = 0; i < variables.size(); i++)
    {
        if(variables[i].getName() == name) return &variables[i];
    }
    return nullptr;
}

int Compiler::VariableHolder::getDepth(std::string name)
{
    Variable* var = Find(name);
    return var->getDepth();
}

Compiler::DataType Compiler::VariableHolder::getType(std::string name)
{
    Variable* var = Find(name);
    return var->getType();
}

void Compiler::VariableHolder::Add(Variable var)
{
    for(size_t i = 0; i < variables.size(); i++)
    {
        variables[i].setDepth(variables[i].getDepth()+var.getSize());
    }
    variables.push_back(var);
}

void Compiler::CompileAll(std::vector<std::string> programLines, std::string name)
{
    name += ".asm";
    out = std::ofstream(name);
    externs = "global _main\n\textern\tExitProcess\n\textern\tAllocConsole\n";
    bss = "section .bss\n";
    main = "section .text\n_main:\n\tpush\t0\n\tcall\tAllocConsole\n";
    for (size_t i = 0; i < programLines.size(); i++)
	{
        if(programLines[i].length() < 1) continue;
		Lexer::TokenList tokens = Lexer::MakeTokens(programLines[i]);
		Parser::SyntaxNode node = Parser::Parse(tokens);
		Compile(node);
	}
    main += newLinePrint;
    main += "\tpush\t0\n\tcall\tExitProcess\n\thlt\n";
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
    if(node.getData().getType() == Lexer::KEYWORD && node.getData().getDataStr() == "print") CompilePrint(node, false);
    else if(node.getData().getType() == Lexer::KEYWORD && node.getData().getDataStr() == "println") CompilePrint(node, true);
    else if (node.getData().getType() == Lexer::KEYWORD && node.getData().getDataStr() == "let") CompileVariableInit(node);
    else if (node.getData().getType() == Lexer::ASSIGN) CompileAssign(node);
}

void Compiler::CompileAssign(Parser::SyntaxNode node)
{
    //if (IsLiteralTree(node.getChild(1), true))
    if (false)
    {
        Interpreter::ReturnType interpreted = Interpreter::InterpretNumOperator(*node.getChild(1));
        main += "\tmov\teax, ";
        if(interpreted.getType() == Interpreter::FLOAT)
        {
            float newVal = interpreted.getData().f;
            main += std::to_string(newVal);
        }
        else
        {
            int newVal = interpreted.getData().i;
            main += std::to_string(newVal);
        }
        main += "\tpush\teax\n";
    }
    else
    {
        CompileMath(*node.getChild(1), 0);
    }
    main += "\tpop\teax\n\tmov\t[esp+";
    int depth = programVariables.getDepth(node.getChild(0)->getData().getDataStr());
    depth *= 4;
    main += std::to_string(depth);
    main += "], eax\n";
}

int Compiler::CompileMath(Parser::SyntaxNode node, int unfinishedPushes)
{
    if (node.getData().getType() == Lexer::IDENTIFIER)
    {
        main += "\tpush\tdword [esp+";
        int depth = programVariables.getDepth(node.getData().getDataStr()) + unfinishedPushes;
        depth *= 4;
        main += std::to_string(depth);
        main += "]\n";
        unfinishedPushes++;
        return unfinishedPushes;
    }
    else
    {
        if (node.childrenCount() == 0)
        {
            int number = Interpreter::InterpretNumOperator(node).getData().i;
            main += "\tpush\t";
            main += std::to_string(number);
            main += "\n";
            unfinishedPushes++;
            return unfinishedPushes;
        }
        else
        {
            unfinishedPushes = CompileMath(*node.getChild(0), unfinishedPushes);
            unfinishedPushes = CompileMath(*node.getChild(1), unfinishedPushes);
            if(node.getData().getType() == Lexer::ADD)
            {
                main += "\tpop\tebx\n\tpop\teax\n\tadd\teax, ebx\npush\teax\n";
                unfinishedPushes -= 2;
                return unfinishedPushes;
            }
            if(node.getData().getType() == Lexer::SUB)
            {
                main += "\tpop\tebx\n\tpop\teax\n\tsub\teax, ebx\npush\teax\n";
                unfinishedPushes -= 2;
                return unfinishedPushes;
            }
            if(node.getData().getType() == Lexer::MULT)
            {
                main += "\tpop\tebx\n\tpop\teax\n\tmul\tebx\npush\teax\n";
                unfinishedPushes -= 2;
                return unfinishedPushes;
            }
            if(node.getData().getType() == Lexer::IDIV)
            {
                main += "\tmov\tedi, 0\n\tpop\tebx\n\tpop\teax\n\tdiv\tebx\npush\teax\n";
                unfinishedPushes -= 2;
                return unfinishedPushes;
            }
            if(node.getData().getType() == Lexer::MOD)
            {
                main += "\tmov\tedx, 0\n\tpop\tebx\n\tpop\teax\n\tdiv\tebx\npush\tedx\n";
                unfinishedPushes -= 2;
                return unfinishedPushes;
            }
        }
    }
}

void Compiler::CompileVariableInit(Parser::SyntaxNode node)
{
    DataType type = NULLTYPE;
    if(node.getChild(0)->getData().getDataStr() == "int") type = INT;
    if(node.getChild(0)->getData().getDataStr() == "float") type = FLOAT;
    //if(node.getChild(0)->getData().getDataStr() == "string") type = STRING;
    if(node.getChild(0)->getData().getDataStr() == "bool") type = BOOL;
    programVariables.Add(Variable(node.getChild(0)->getChild(0)->getData().getDataStr(), type));

    std::string defaultValue = "";
    if(node.childrenCount() > 1)
    {
        if(type == INT) defaultValue = std::to_string(node.getChild(2)->getData().getDataNumerical<int>());
        else if(type == FLOAT) defaultValue = std::to_string(node.getChild(2)->getData().getDataNumerical<float>());
        else if(type == BOOL) defaultValue = std::to_string(node.getChild(2)->getData().getDataBool());
    }
    else
    {
        if(type == INT || type == BOOL) defaultValue = "0";
        else if(type == FLOAT) defaultValue = "0.0";
    }
    
    main += "\tpush\t";
    main += defaultValue;
    main += "\n";
}

void Compiler::CompileInput(std::string prompt)
{
    Parser::SyntaxNode promptNode = Parser::SyntaxNode(Lexer::Token(Lexer::KEYWORD, "print"), std::vector<Parser::SyntaxNode*>{new Parser::SyntaxNode(Lexer::Token(Lexer::STRING, prompt))});
    CompilePrint(promptNode, true);
    std::string inName = "__i";
    if(!hasInput)
    {
        externs += "\textern\tReadConsoleA\n";
        bss += inName;
        bss += ":\tresb 32\n__iLen:\tresw 1\n";
        hasInput = true;
    }
    main += "\tpush\t-10\n\tcall\tGetStdHandle\n\tmov\tebx, eax\n\tpush\t0\n\tpush\t__iLen\n\tpush\t32\n\tpush\t";
    main += inName;
    main += "\n\tpush\tebx\n\tcall\tReadConsoleA\n";
}

bool Compiler::IsLiteral(Parser::SyntaxNode node)
{
    Lexer::TokenType type = node.getData().getType();
    return (type == Lexer::STRING ||  node.getData().isNumber() || type == Lexer::BOOL);
}
bool Compiler::IsLiteralTree(Parser::SyntaxNode* node, bool rest)
{
    if (rest)
    {
        if(IsLiteral(*node))
        {
            if(node->childrenCount() == 0) return true;
            else
            {
                for (int i = 0; i < node->childrenCount(); i++)
                {
                    Parser::SyntaxNode* child = node->getChild(i);
                    return IsLiteralTree(child, true);
                }
            }
            
        }
        else return false;
    }
    else return false;
}

std::string Compiler::ShortenFloat(std::string f)
{
    while (f[f.length()-1] == '0') f.pop_back();
    if(f[f.length()-1] == '.') f += '0';
    return f;
}

void Compiler::CompilePrint(Parser::SyntaxNode node, bool newLine)
{
    if(valId == 0)
    {
        externs += "\textern\tGetStdHandle\n\textern\tWriteFile\n";
        bss += "__o:\tresb 32\n";
    } 
    bool literal = IsLiteralTree(node.getChild(0), true);
    
    if(literal)
    {
        Lexer::TokenType printType = node.getChild(0)->getData().getType();
        std::string message = "";
        if(printType == Lexer::STRING) message = node.getChild(0)->getData().getDataStr();
        else if(printType == Lexer::INT) message = std::to_string(node.getChild(0)->getData().getDataNumerical<int>());
        else if(printType == Lexer::FLOAT)
        {
            message = std::to_string(node.getChild(0)->getData().getDataNumerical<float>());
            message = ShortenFloat(message);
        }
        else if(node.getChild(0)->isNumber())
        {
            Interpreter::ReturnType interpreted = Interpreter::InterpretNumOperator(*node.getChild(0));
            if(interpreted.getType() == Interpreter::FLOAT)
            {
                message = std::to_string(interpreted.getData().f);
                message = ShortenFloat(message);
            }
            else message = std::to_string(interpreted.getData().i);
        }

        main += "\tpush\t-11\n\tcall\tGetStdHandle\n\tmov\tebx, eax\n\tpush\t0\n\tlea\teax, [ebp-4]\n\tpush\teax\n\tpush\t";
        main += std::to_string(message.length() + newLine);
        main += "\n\tpush\t";
        main += "l" + std::to_string(valId);
        main += "\n\tpush\tebx\n\tcall\tWriteFile\n\tpush\t0\n";
        end +=  "l" + std::to_string(valId);
        end += ": db '";
        end += message;
        if (newLine) end += "', 10\n";
        else end += "'\n";
    }
    else
    {
        main += "\tpush\t-11\n\tcall\tGetStdHandle\n\tmov\tebx, eax\n";
        Lexer::TokenType printType = node.getChild(0)->getData().getType();
        if(printType == Lexer::KEYWORD && node.getChild(0)->getData().getDataStr() == "input") main += "\tmov\tedx, __i\n\tmov\tecx, [__iLen]\n\tsub\tecx, 1\n";
        else if(printType == Lexer::IDENTIFIER)
        {
            main += "\tmov\tecx, [esp+";
            int depth = programVariables.getDepth(node.getChild(0)->getData().getDataStr());
            depth *= 4;
            std::string countStr = std::to_string(intToStringCount);
            main += std::to_string(depth);
            main += "]\n";
            StringToInt();
        }
        main += "\tpush\t0\n\tlea\teax, [ebp-4]\n\tpush\teax\n\tpush\tecx\n\tpush\tedx\n\tpush\tebx\n\tcall\tWriteFile\n";
        if (newLine) main += newLinePrint;
    } 
    valId++;
}

std::string Compiler::StringToInt()
{
    std::string countStr = std::to_string(intToStringCount);
    main += "\tpush\tebx\n\tcmp\tecx, 0\n\tjge\titsp";
    main += countStr;
    main += "\n\tmov\t[__o], byte '-'\n\tmov\tesi, 1\n\tpush\t1\n\tneg\tecx\n\tmov\tedi, 1\n\tjmp\titsas";
    main += countStr;
    main += "\nitsp";
    main += countStr;
    main += ":\n\tmov\tedi, 0\n\tmov\tesi, 0\n\tpush\t0\n";
    main += "itsas";
    main += countStr;
    main += ":\n\tmov\teax, 1\n\tmov\tebx, 10\nitssl";
    main += countStr;
    main += ":\n\tmul\tebx\n\tinc\tesi\n\tcmp\teax, ecx\n\tjle\titssl";
    main += countStr;
    main += "\n\tmov\tedx, 0\n\tdiv\tebx\n\tpush\teax\n\tmov\teax, ecx\n\tpop\tecx\n\tpop\tebx\n\tpush\tesi\n\tcmp\tebx, 1\n\tjne\titsp2";
    main += countStr;
    main += "\n\tsub\tesi, 2\n\tjmp\titsls";
    main += countStr;
    main += "\nitsp2";
    main += countStr;
    main += ":\n\tdec\tesi\nitsls";
    main += countStr;
    main += ":\n\tmov\tedx, 0\n\tmov\tebx, 10\nitscl";
    main += countStr;
    main += ":\n\tdiv\tecx\n\tadd\teax, '0'\n\tmov\t[__o+edi], eax\n\tdec\tesi\n\tinc\tedi\n\tpush\tedx\n\tmov\teax, ecx\n\tmov\tedx, 0\n\tdiv\tebx\n\tmov\tecx, eax\n\tpop\teax\n\tcmp\tesi, 0\n\tjge\titscl";
    main += countStr;
    main += "\n\n\t\tpop\tecx\n\tpop\tebx\n\tmov\tedx, __o\n";
    intToStringCount++;
}