/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TOPIC.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andrferr <andrferr@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 16:55:53 by andrferr          #+#    #+#             */
/*   Updated: 2023/07/07 16:57:34 by andrferr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TOPIC.hpp"
#include "../Message.hpp"
#include "../rpl_isupport.hpp"

Topic::Topic(Server &server, Client &client, std::string &input, std::vector<Client*> &clientsList) :
ACommand(server, client, input, clientsList){}

Topic::Topic(const Topic &other) :
ACommand(other.server, other.client, other.input, other.clientsList) { *this = other; }
Topic::~Topic(){}
Topic &Topic::operator=(const Topic &other) { return *this; }

void Topic::execTOPIC()
{
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
	if (!c->isOper(client.getNickname()) && c->getModesTopic())
	{
		Message msg;
		msg.reply(NULL, client, ERR_CHANOPRIVSNEEDED_CODE, SERVER, ERR_CHANOPRIVSNEEDED, client.getNickname().c_str(), c->getName().c_str());
		throw NoPrivilegesException("No privileges");
	}
	if (input.size() > TOPICLEN)
		input = input.substr(0, TOPICLEN);
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