/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andrferr <andrferr@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:40:52 by andrferr          #+#    #+#             */
/*   Updated: 2023/06/30 11:33:28 by andrferr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "Command.hpp"
#include "Parser.hpp"
#include "Message.hpp"
#include "Channel.hpp"

Command::Command(std::string &input, Client &client, Server &server) :
input(input),
client(client),
server(server) {}
Command::Command(const Command &other) : input(other.input), client(other.client), server(other.server) { *this = other; }
Command::~Command() {}
Command &Command::operator=(const Command &other)
{
	if (this == &other) return *this;
	input = other.input;
	client = other.client;
	return (*this);
}

void Command::checkCommands(std::vector<Client*> *clients)
{
	size_t pos;
	pos = input.find(" ");
	std::string command = input.substr(0, pos);
	int commandId = getCommandId(command);
	input.erase(0, pos + 1);
	if (input[input.length() - 1] == '\n')
		input.erase(input.length() - 1, 1);
	if (input[input.length() - 1] == '\r')
		input.erase(input.length() - 1, 1);
	switch (commandId)
	{
		case PASS:
			throw AlreadyRegisteredException("Already registered");
			break;
		case NICK:
		{
			try{
				execNICK(input, *clients);
			} catch(InvalidNickException &e) {
				std::cerr << client.getNickname() << " cant update NICK." << std::endl;
			} catch(DuplicateNickException &e) {
				std::cerr << client.getNickname() << " cant update NICK because already exists." << std::endl;
			}
			break;
		}
		case USER:
			throw AlreadyRegisteredException("Already registered");
			break;
		case KICK:
			break;
		case INVITE:
			break;
		case TOPIC:
		{
			try {
				execTOPIC(input);
				server.logMessage(1, "Topic changed", client.getNickname());
			} catch (NeedMoreParamsException &e) {
				Message msg;
				msg.reply(NULL, client, ERR_NEEDMOREPARAMS_CODE, SERVER, ERR_NEEDMOREPARAMS, client.getNickname().c_str(), "TOPIC");
				server.logMessage(2, "TOPIC: need more params", client.getNickname());
			} catch (NoSuchChannelException &e) {
				server.logMessage(2, "TOPIC: no such channel", client.getNickname());
			} catch (NotOnChannelException &e) {
				server.logMessage(2, "TOPIC: not on channel", client.getNickname());
			} catch (NoPrivilegesException &e) {
				server.logMessage(2, "TOPIC: no priviledges", client.getNickname());
			}
			break;
		}
		case MODE:
			break;
		case OPER:
			break;
		case QUIT:
		{
			execQUIT(input);
			break;
		}
		case JOIN:
			try {
				execJOIN(input);
			} catch(NeedMoreParamsException &e) {
				server.logMessage(2, "JOIN: Need more params", client.getNickname());
				Message msg;
				msg.reply(NULL, client, ERR_NEEDMOREPARAMS_CODE, SERVER, ERR_NEEDMOREPARAMS, client.getNickname().c_str(), "JOIN");
			} catch(NoSuchChannelException &e) {
				server.logMessage(2, "JOIN: No such channel", client.getNickname());
			} 
			break;
		case PART:
		{
			try {
				execPART(input);
			} catch (NeedMoreParamsException &e) {
				server.logMessage(2, "PART: Need more params.", client.getNickname());
			}
			break;
		}
		case PRIVMSG:
		{
			execPRIVMSG(input);
			break;
		}
		case NOTICE:
			break;
		default:
			break;
	}
}

int Command::getCommandId(std::string &input) const
{
	if (input == "PASS")
		return PASS;
	else if (input == "NICK")
		return NICK;
	else if (input == "USER")
		return USER;
	else if (input == "KICK")
		return KICK;
	else if (input == "INVITE")
		return INVITE;
	else if (input == "TOPIC")
		return TOPIC;
	else if (input == "MODE")
		return MODE;
	else if (input == "OPER")
		return OPER;
	else if (input == "QUIT")
		return QUIT;
	else if (input == "JOIN")
		return JOIN;
	else if (input == "PART")
		return PART;
	else if (input == "PRIVMSG")
		return PRIVMSG;
	else if (input == "NOTICE")
		return NOTICE;
	return (-1);
}

void Command::execNICK(std::string &input, std::vector<Client*> &clients)
{
	std::string clientNick = input;
	for (int i = 0; i < clientNick.length(); i++)
		clientNick[i] = std::tolower(clientNick[i]);
	std::string nickToCompare;
	for(std::vector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		nickToCompare = (*(*it)).getNickname();
		for (int i = 0; i < nickToCompare.length(); i++)
			nickToCompare[i] = std::tolower(nickToCompare[i]);
		if (nickToCompare == clientNick)
		{
			Message msg;
			msg.reply(NULL, client, ERR_NICKNAMEINUSE_CODE, SERVER, ERR_NICKNAMEINUSE, client.getNickname().c_str(), input.c_str());
			throw DuplicateNickException("Duplicate nick");
		}
	}
	if (std::isdigit(input[0]) || input[0] == '#' || input[0] == ' ' || input[0] == ':')
	{
		Message msg;
		msg.reply(NULL, client, ERR_ERRONEUSNICKNAME_CODE, SERVER, ERR_ERRONEUSNICKNAME, client.getNickname().c_str(), input.c_str());
		throw InvalidNickException("Invalid Nick");
	}
	client.setNickname(input);
}

void Command::execJOIN(std::string &input)
{
	std::vector<std::string> channels, keys;
	size_t pos;
	std::string arg, list;
	
	pos = input.find(" ");
	list = input.substr(0, pos);
	input.erase(0, pos + 1);
	while ((pos = list.find(",")) != std::string::npos)
	{
		arg = list.substr(0, pos);
		channels.push_back(arg);
		list.erase(0, pos + 1);
	}
	if (!list.empty())
		channels.push_back(list);
	while ((pos = input.find(",")) != std::string::npos)
	{
		arg = input.substr(0, pos);
		keys.push_back(arg);
		input.erase(0, pos + 1);
	}
	if (!input.empty())
		keys.push_back(input);
	if (channels.size() < 1)
		throw NeedMoreParamsException("Need more params");
	for (int i = 0; i < channels.size(); i++)
	{
		Parser parser;
		if (parser.parseChannelName(channels[i]) == -1)
		{
			server.logMessage(2, "Bad channel name", client.getNickname());
			Message msg;
			msg.reply(NULL, client, ERR_NOSUCHCHANNEL_CODE, SERVER, ERR_NOSUCHCHANNEL, client.getNickname().c_str(), channels[i].c_str());
			throw NoSuchChannelException("No such channel");
		}
		Channel *channel = server.searchChannel(channels[i]);
		if (!channel)
		{
			channel = server.createChannel(channels[i], "", keys[i], client);
			channel->addOper(&client);
		}
		try {
			channel->addUser(&client);
		} catch (Channel::AlreadyUserException &e) {
			break ;
		}
		Message msg;
		channel->messageAll(&client, "%s %s", "JOIN", channel->getName().c_str());
		if (channel->getTopic().empty())
			msg.reply(NULL, client, RPL_NOTOPIC_CODE, SERVER, RPL_NOTOPIC, client.getNickname().c_str(), channel->getName().c_str());
		else
			msg.reply(NULL, client, RPL_TOPIC_CODE, SERVER, RPL_TOPIC, client.getNickname().c_str(), channel->getName().c_str(), channel->getTopic().c_str());
		msg.reply(NULL, client, RPL_NAMREPLY_CODE, SERVER, RPL_NAMREPLY, client.getNickname().c_str(), "=", channel->getName().c_str(), channel->getListClientsNicknames().c_str());
		msg.reply(NULL, client, RPL_ENDOFNAMES_CODE, SERVER, RPL_ENDOFNAMES, client.getNickname().c_str(), channel->getName().c_str());
		server.logMessage(1, "joined channel " + channel->getName(), client.getNickname());
	}
}

void Command::execPART(std::string &input)
{
	std::vector<std::string> channels;
	size_t pos;
	std::string tmp;
	while ((pos = input.find(",")) != std::string::npos)
	{
		tmp = input.substr(0, pos);
		channels.push_back(tmp);
		input.erase(0, pos + 1);
	}
	if (!input.empty())
		channels.push_back(input);
	if (channels.size() < 1)
		throw NeedMoreParamsException("Need more params");
	for(std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it)
	{
		//HANDLE PART OF EVERY CHANNEL IN THE LIST!!!
		Message msg;
		Channel *c = server.searchChannel(*it);
		if (!c)
		{
			Message msg;
			msg.reply(NULL, client, ERR_NOSUCHCHANNEL_CODE, SERVER, ERR_NOSUCHCHANNEL, client.getNickname().c_str(), (*it).c_str());
			continue;
		}
		if (!c->isClientInChannel(client.getNickname()))
		{
			Message msg;
			msg.reply(NULL, client, ERR_NOTONCHANNEL_CODE, SERVER, ERR_NOTONCHANNEL, client.getNickname().c_str(), (*it).c_str());
			continue;
		}
		c->eraseClient(client.getNickname());
	}
}

void Command::execPRIVMSG(std::string &input)
{
	std::vector<std::string> targets;
	size_t pos;
	std::string tmp;
	pos = input.find(" ");
	if (pos == std::string::npos)
		return ;
	std::string targets_list = input.substr(0, pos);
	input.erase(0, pos + 1);
	if (input[0] == ':')
		input.erase(0, 1);
	while ((pos = targets_list.find(",")) != std::string::npos)
	{
		tmp = targets_list.substr(0, pos);
		targets.push_back(tmp);
		targets_list.erase(0, pos + 1);
	}
	if (!targets_list.empty())
		targets.push_back(targets_list);
	for(std::vector<std::string>::iterator it = targets.begin(); it != targets.end(); ++it)
	{
		Channel *c = server.searchChannel(*it);
		if (!c) continue;
		c->sendPRIVMSG(&client, input);
		server.logMessage(1, "PRIVMSG " + c->getName() + ": " + input, client.getNickname());
	}
}

void Command::execTOPIC(std::string &input)
{
	std::cout << input << std::endl;
	size_t pos;
	pos = input.find(" ");
	std::string target = input.substr(0, pos);
	if (target.empty())
		throw NeedMoreParamsException("Need more params");
	input.erase(0, pos + 1);
	Channel *c = server.searchChannel(target);
	if (!c)
	{
		Message msg;
		msg.reply(NULL, client, ERR_NOSUCHCHANNEL_CODE, SERVER, ERR_NOSUCHCHANNEL, client.getNickname().c_str(), target.c_str());
		throw NoSuchChannelException("No such channel");
	}
	if (!c->isClientInChannel(client.getNickname()))
	{
		Message msg;
		msg.reply(NULL, client, ERR_NOTONCHANNEL, SERVER, ERR_NOTONCHANNEL, client.getNickname().c_str(), c->getName().c_str());
		throw NotOnChannelException("Not on channel");
	}
	if (!c->isOper(client.getNickname()))
	{
		Message msg;
		msg.reply(NULL, client, ERR_CHANOPRIVSNEEDED_CODE, SERVER, ERR_CHANOPRIVSNEEDED, client.getNickname().c_str(), c->getName().c_str());
		throw NoPrivilegesException("No privileges");
	}
	if (input[0] == ':' && input.length() > 1)
	{
		input.erase(0, 1);
		c->setTopic(input);
		c->messageAll(&client, "TOPIC %s :%s", c->getName().c_str(), c->getTopic().c_str());
	}
	else if (input[0] == ':' && input.length() <= 1)
	{
		c->setTopic("");
		c->messageAll(&client, "TOPIC %s :%s", c->getName().c_str(), c->getTopic().c_str());
	}
	else
	{
		Message msg;
		msg.reply(NULL, client, RPL_TOPIC_CODE, SERVER, RPL_TOPIC, client.getNickname().c_str(), c->getName().c_str(), c->getTopic().c_str());
	}
}

void Command::execQUIT(std::string &input)
{
	if (input.empty()) return;
	if (input[0] == ':')
		input.erase(0, 1);
	std::cout << "reason: " << input << std::endl;
	client.setConnected(false);
}