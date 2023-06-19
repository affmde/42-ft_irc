/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andrferr <andrferr@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/17 07:43:52 by andrferr          #+#    #+#             */
/*   Updated: 2023/06/19 17:14:42 by andrferr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Parser::Parser() {}

Parser::Parser(const Parser &other) { *this = other; }

Parser::~Parser() {}

Parser &Parser::operator=(const Parser &other)
{
	if (this != &other)
	{
		input = other.input;
		pass = other.pass;
		nick = other.nick;
	}
	return *this;
}

//Member Functions

std::string Parser::getInput() const { return input; }

void Parser::setInput(std::string str) { input = str; }

std::vector<std::string> Parser::parseInput()
{
	if (input.empty())
		throw WrongInputException("Wrong input");
	std::vector<std::string>	args;
	size_t pos;
	std::string arg;
	if (input.find("\n") == std::string::npos)
		args.push_back(input);
	else
	{
		while ((pos = input.find("\n")) != std::string::npos)
		{
			arg = input.substr(0, pos + 1);
			input.erase(0, pos + 1);
			args.push_back(arg);
		}
	}
	return args;
}

#include <iostream>
void Parser::parsePass(std::string input, std::string pass)
{
	if (input.empty())
		throw WrongInputException("Wrong input");
	if (input.length() - std::string("Pass ").length() < 1)
		throw NoPassException("No pass");
	std::string password = input.substr(5, input.length() - 5);
	if (password[0] == ':')
		password.erase(0, 1);
	if (password.compare(pass) != 0)
		throw NoPassException("Wrong pass");
}

std::string Parser::parseNick(std::string input)
{
	if (input.empty())
		throw WrongInputException("Wrong input");
	if (input.length() - std::string("NICK ").length() < 1)
		throw NoNickException("No nick");
	std::string nick = input.substr(5, input.length() - 5);
	return nick;
}

