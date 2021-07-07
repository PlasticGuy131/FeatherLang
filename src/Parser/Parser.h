#pragma once

#include <vector>
#include <string>

#include "../Lexer/Lexer.h"

namespace Parser
{
	class SyntaxNode
	{
	public:
		SyntaxNode(Lexer::Token d, std::vector<SyntaxNode*> c, SyntaxNode* p) { data = d; children = c; parent = p; }

		SyntaxNode(Lexer::Token d, std::vector<SyntaxNode*> c) { data = d; children = c; }

		SyntaxNode(Lexer::Token d, SyntaxNode* p) { data = d;  parent = p; }

		SyntaxNode(Lexer::Token d) { data = d; }

		Lexer::Token getData() { return data; }

		SyntaxNode* getChild(int index) { return children[index]; }

		SyntaxNode* getParent() { return parent; }

		std::string ToString();

		int childrenCount() { return children.size(); }

		void addChild(SyntaxNode* c) { children.push_back(c); }

		void setParent(SyntaxNode* p) { parent = p; }

		bool hasParent() { return (parent != nullptr); }

		bool hasChildren() { return children.size() != 0; }

		bool isNumber() { return data.isNumber(); }

		bool isComp() { return data.isComp(); }

		std::vector<SyntaxNode*> getChildren() { return children; }

		bool operator==(const SyntaxNode& rhs);

	private:
		Lexer::Token data = Lexer::Token();
		std::vector<SyntaxNode*> children = std::vector<SyntaxNode*>();
		SyntaxNode* parent = nullptr;
	};

	SyntaxNode Parse(Lexer::TokenList tokens);

	std::vector<SyntaxNode*> TokensToNode(Lexer::TokenList tokens);

	std::vector<SyntaxNode*> JoinNodesArithmetic(std::vector<SyntaxNode*> nodes, Lexer::TokenType op, int start, int end);

	std::vector<SyntaxNode*> JoinNodesCompair(std::vector<SyntaxNode*> nodes, Lexer::TokenType op, int start, int end);

	std::vector<SyntaxNode*> JoinBrackets(std::vector<SyntaxNode*> nodes, int start, int end);

	std::vector<SyntaxNode*> JoinKeywords(std::vector<SyntaxNode*> nodes, int start, int end);

	std::vector<SyntaxNode*> JoinTypes(std::vector<SyntaxNode*> nodes, int start, int end);

	std::vector<SyntaxNode*> JoinAssign(std::vector<SyntaxNode*> nodes, int start, int end);

	std::vector<SyntaxNode*> JoinAll(std::vector<SyntaxNode*> nodes, int start, int end);

	SyntaxNode CalculateRoot(std::vector<SyntaxNode*> nodes);
}
