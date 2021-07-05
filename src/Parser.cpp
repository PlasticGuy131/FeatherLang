#include <iostream>

#include "Parser.h"
#include "Error.h"
#include "Lexer.h"

std::string Parser::SyntaxNode::ToString()
{
	std::string out = "[" + data.ToString();
	if (hasChildren())
	{
		for (int i = 0; i < childrenCount(); i++)
		{
			out += "-" + children[i]->ToString();
		}
	}
	out += "]";
	return out;
}


bool Parser::SyntaxNode::operator==(const SyntaxNode &rhs)
{
	const SyntaxNode* ptr = dynamic_cast<const SyntaxNode*>(&rhs);
	if (ptr != nullptr)
	{
		bool sameChildren = true;

		if (children.size() == ptr->children.size())
		{
			for (size_t i = 0; i < children.size(); i++)
			{
				if (!(children[i] == ptr->children[i]))
				{
					sameChildren = false;
				}
			}
		}
		else
		{
			sameChildren = false;
		}
		return (data == ptr->data && sameChildren);
	}
	else
	{
		return false;
	}
}


Parser::SyntaxNode Parser::Parse(Lexer::TokenList tokens)
{
	std::vector<Parser::SyntaxNode*> nodes = TokensToNode(tokens);

	nodes = JoinAll(nodes, 0, 0);

	Parser::SyntaxNode root = CalculateRoot(nodes);

	return root;
}

std::vector<Parser::SyntaxNode*> Parser::TokensToNode(Lexer::TokenList tokens)
{
	std::vector<Parser::SyntaxNode*> nodes = std::vector<Parser::SyntaxNode*>();
	for (int i = 0; i < tokens.length(); i++)
	{
		nodes.push_back(new Parser::SyntaxNode(tokens.get(i)));
	}
	return nodes;
}

std::vector<Parser::SyntaxNode*> Parser::JoinNodesArithmetic(std::vector<Parser::SyntaxNode*> nodes, Lexer::TokenType op, int start, int end)
{
	for (size_t i = start; i < nodes.size() - end; i++)
	{
		if (nodes[i]->getData().getType() == op && !nodes[i]->hasChildren())
		{
			if (i == 0)
			{
				Error::ParsingError(Lexer::Token(), Lexer::NUMBER);
			}
			else if(!nodes[i-1]->isNumber())
			{
				Error::ParsingError(nodes[i - 1]->getData(), Lexer::NUMBER);
			}
			else
			{
				Parser::SyntaxNode* node = nodes[i - 1];
				while (node->hasParent())
				{
					node = node->getParent();
				}
				nodes[i]->addChild(node);

				node->setParent(nodes[i]);
			}

			if (i == nodes.size()-1)
			{
				Error::ParsingError(Lexer::Token(), Lexer::NUMBER);
			}
			else if (!nodes[i+1]->isNumber())
			{
				Error::ParsingError(nodes[i+1]->getData(), Lexer::NUMBER);
			}
			else
			{
				Parser::SyntaxNode* node = nodes[i+1];
				while (node->hasParent())
				{
					node = node->getParent();
				}
				nodes[i]->addChild(node);

				node->setParent(nodes[i]);
			}
		}
	}

	return nodes;
}

std::vector<Parser::SyntaxNode*> Parser::JoinBrackets(std::vector<Parser::SyntaxNode*> nodes, int start, int end)
{
	for (size_t i = start; i < nodes.size()-end; i++)
	{
		if (*nodes[i] == Parser::SyntaxNode(Lexer::OPENBRACKET))
		{
			if (i != 0)
			{
				if (nodes[i - 1]->getData().getType() == Lexer::KEYWORD) continue;
			}
			int close = -1;
			int brackets = 0;
			for (size_t j = i + 1;  nodes.size() - end; j++)
			{
				if (*nodes[j] == Parser::SyntaxNode(Lexer::OPENBRACKET)) brackets++;
				else if (*nodes[j] == Parser::SyntaxNode(Lexer::CLOSEBRACKET))
				{
					if (brackets != 0)
					{
						brackets--;
					}
					else
					{
						close = j;
						break;
					}
				}
			}

			if (close == -1) Error::EOFError(')');
			else
			{
				nodes = JoinAll(nodes, i+1, nodes.size() - close);

				nodes.erase(nodes.begin() + i, nodes.begin() + i + 1);

				for (size_t j = i + 1; nodes.size() - end; j++)
				{
					if (*nodes[j] == Parser::SyntaxNode(Lexer::CLOSEBRACKET))
					{
						close = j;
						break;
					}
				}

				nodes.erase(nodes.begin() + close, nodes.begin() + close + 1);
			}
		}
	}

	return nodes;
}

std::vector<Parser::SyntaxNode*> Parser::JoinKeywords(std::vector<Parser::SyntaxNode*> nodes, int start, int end)
{
    for (size_t i = start; i < nodes.size() - end; i++)
	{
		if (nodes[i]->getData().getType() == Lexer::KEYWORD && nodes[i]->getData().getDataStr() == "print")
		{
			if (i == nodes.size()-1)
			{
				Error::ParsingError(Lexer::Token(), Lexer::Token::Keywords.find("print")->second[0]);
			}
			else
			{
				if (i != nodes.size())
				{
					if (nodes[i + 1]->getData().getType() != Lexer::OPENBRACKET) Error::ParsingError(nodes[i + 1]->getData().getType(), Lexer::OPENBRACKET);
					else
					{
						auto it = Lexer::Token::Keywords.find("print");
						for (size_t n = 0; n < it->second.size(); n++)
						{
							if (nodes[i + 2 + n]->getData().getType() != it->second[n] && it->second[n] != Lexer::ANY)
							{
								Error::ParsingError(nodes[i + 2 + n]->getData().getType(), it->second[n]);
							}
							else
							{
								Parser::SyntaxNode* node = nodes[i + 2 + n];
								while (node->hasParent())
								{
									node = node->getParent();
								}
								node->setParent(nodes[i]);
								nodes[i]->addChild(node);

								nodes.erase(nodes.begin() + i + 1, nodes.begin() + i + 2);

								int close = 0;
								for (size_t j = i + 1; nodes.size() - end; j++)
								{
									if (*nodes[j] == Parser::SyntaxNode(Lexer::CLOSEBRACKET))
									{
										close = j;
										break;
									}
								}

								nodes.erase(nodes.begin() + close, nodes.begin() + close + 1);
							}
						}
					}
				}
				else
				{
					Error::ParsingError(Lexer::Token(), Lexer::OPENBRACKET);
				}
			}
		}
		else if (nodes[i]->getData().getType() == Lexer::KEYWORD && nodes[i]->getData().getDataStr() == "let")
		{
			if (i == nodes.size()-1)
			{
				Error::ParsingError(Lexer::Token(), Lexer::Token::Keywords.find("let")->second[0]);
			}
			else
			{
				if (i != nodes.size())
				{
					if (nodes[i + 1]->getData().getType() != Lexer::TYPE) Error::ParsingError(nodes[i + 1]->getData().getType(), Lexer::TYPE);
					else
					{
						auto it = Lexer::Token::Keywords.find("print");
						nodes[i]->addChild(nodes[i + 1]);
						nodes[i + 1]->setParent(nodes[i]);
					}
				}
				else
				{
					Error::ParsingError(Lexer::Token(), Lexer::OPENBRACKET);
				}
			}
		}
	}

	return nodes;
}

std::vector<Parser::SyntaxNode*> Parser::JoinAll(std::vector<Parser::SyntaxNode*> nodes, int start, int end)
{
	nodes = JoinBrackets(nodes, start, end);

	if (nodes.size() > 1)
	{
		nodes = JoinNodesArithmetic(nodes, Lexer::POW, start, end);

		nodes = JoinNodesArithmetic(nodes, Lexer::DIV, start, end);

		nodes = JoinNodesArithmetic(nodes, Lexer::MULT, start, end);

		nodes = JoinNodesArithmetic(nodes, Lexer::ADD, start, end);

		nodes = JoinNodesArithmetic(nodes, Lexer::SUB, start, end);
	}

	nodes = JoinKeywords(nodes, start, end);

	return nodes;
}

Parser::SyntaxNode Parser::CalculateRoot(std::vector<Parser::SyntaxNode*> nodes)
{
	Parser::SyntaxNode* node = nodes[0];

	while (node->hasParent())
	{
		node = node->getParent();
	}

	return *node;
}