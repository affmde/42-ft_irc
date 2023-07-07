/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andrferr <andrferr@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:40:52 by andrferr          #+#    #+#             */
/*   Updated: 2023/07/07 17:46:29 by andrferr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <map>

#include "Command.hpp"
#include "Parser.hpp"
#include "Message.hpp"
#include "Channel.hpp"
#include "rpl_isupport.hpp"
#include "commands/NICK.hpp"
#include "commands/JOIN.hpp"
#include "commands/PART.hpp"
#include "commands/TOPIC.hpp"
#include "commands/KICK.hpp"
#include "commands/QUIT.hpp"
#include "commands/INVITE.hpp"
#include "commands/NOTICE.hpp"
#include "commands/PRIVMSG.hpp"
#include "commands/MODE.hpp"

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
	input.erase(0, pos + 1);;
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
				Nick n(server, client, input, *clients);
				n.execNICK();
			} catch(ACommand::InvalidNickException &e) {
				std::cerr << client.getNickname() << " cant update NICK." << std::endl;
			} catch(ACommand::DuplicateNickException &e) {
				std::cerr << client.getNickname() << " cant update NICK because already exists." << std::endl;
			}
			break;
		}
		case USER:
			throw AlreadyRegisteredException("Already registered");
			break;
		case KICK:
		{
			try {
				Kick k(server, client, input, *clients);
				k.execKICK();
			} catch (ACommand::NoSuchChannelException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch (ACommand::NeedMoreParamsException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch (ACommand::NoPrivilegesException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch (ACommand::NotOnChannelException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			}
			break;
		}
		case INVITE:
		{
			try {
				Invite i(server, client, input, *clients);
				i.execINVITE();
			} catch (ACommand::NeedMoreParamsException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch (ACommand::NoSuchChannelException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch (ACommand::NotOnChannelException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch (ACommand::NoPrivilegesException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch (ACommand::UserOnChannelException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			}
		}
			break;
		case TOPIC:
		{
			try {
				Topic t(server, client, input, *clients);
				t.execTOPIC();
				server.logMessage(1, "Topic changed", client.getNickname());
			} catch (ACommand::NeedMoreParamsException &e) {
				Message msg;
				msg.reply(NULL, client, ERR_NEEDMOREPARAMS_CODE, SERVER, ERR_NEEDMOREPARAMS, client.getNickname().c_str(), "TOPIC");
				server.logMessage(2, "TOPIC: need more params", client.getNickname());
			} catch (ACommand::NoSuchChannelException &e) {
				server.logMessage(2, "TOPIC: no such channel", client.getNickname());
			} catch (ACommand::NotOnChannelException &e) {
				server.logMessage(2, "TOPIC: not on channel", client.getNickname());
			} catch (ACommand::NoPrivilegesException &e) {
				server.logMessage(2, "TOPIC: no priviledges", client.getNickname());
			}
			break;
		}
		case MODE:
		{
			try {
				Mode m(server, client, input, *clients);
				m.execMODE();
			} catch (ACommand::NoSuchChannelException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch (ACommand::NoPrivilegesException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			}
			break;
		}
		case QUIT:
		{
			Quit q(server, client, input, *clients);
			q.execQUIT();
			break;
		}
		case JOIN:
			try {
				Join j(server, client, input, *clients);
				j.execJOIN();
			} catch(ACommand::NeedMoreParamsException &e) {
				server.logMessage(2, "JOIN: Need more params", client.getNickname());
				Message msg;
				msg.reply(NULL, client, ERR_NEEDMOREPARAMS_CODE, SERVER, ERR_NEEDMOREPARAMS, client.getNickname().c_str(), "JOIN");
			} catch(ACommand::NoSuchChannelException &e) {
				server.logMessage(2, "JOIN: No such channel", client.getNickname());
			} catch(ACommand::BadChannelKeyException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch(ACommand::InviteOnlyException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch(ACommand::ChannelFullException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			}
			break;
		case PART:
		{
			try {
				Part p(server, client, input, *clients);
				p.execPART();
			} catch (ACommand::NeedMoreParamsException &e) {
				server.logMessage(2, "PART: Need more params.", client.getNickname());
			}
			break;
		}
		case PRIVMSG:
		{
			try {
				Privmsg p(server, client, input, *clients);
				p.execPRIVMSG();
			} catch (ACommand::NoSuchChannelException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch (ACommand::InvalidNickException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch (ACommand::NeedMoreParamsException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			}
			break;
		}
		case NOTICE:
		{
			try {
				Notice n(server, client, input, *clients);
				n.execNOTICE();
			} catch(ACommand::NeedMoreParamsException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch(ACommand::NoSuchChannelException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			} catch(ACommand::InvalidNickException &e) {
				server.logMessage(2, e.what(), client.getNickname());
			}
			break;
		}
		case PING:
		{
			execPING(input);
			break;
		}
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
	else if (input == "PING")
		return PING;
	return (-1);
}

void Command::execPING(std::string &input)
{
	if (input.empty())
		return ;
	Message msg;
	msg.reply(NULL, client, "0", SERVER, "PONG :%s %s", "IRCSERVER", input.c_str());
	server.logMessage(1, "PONG: " + input, "");
}
