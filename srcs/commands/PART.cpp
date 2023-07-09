/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PART.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andrferr <andrferr@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 16:48:56 by andrferr          #+#    #+#             */
/*   Updated: 2023/07/08 22:30:24 by andrferr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <map>

#include "PART.hpp"
#include "../Message.hpp"


Part::Part(Server &server, Client &client, std::string &input, std::vector<Client*> &clientsList) :
ACommand(server, client, input, clientsList){}

Part::Part(const Part &other) :
ACommand(other.server, other.client, other.input, other.clientsList) { *this = other; }
Part::~Part(){}
Part &Part::operator=(const Part &other) { return *this; }

void Part::exec()
{
	std::vector<std::string> channels;
	size_t pos = input.find(" ");
	std::string channels_list = input.substr(0, pos);
	input.erase(0, pos +1);
	std::map<std::string, std::string> list;
	std::string tmp;
	while ((pos = channels_list.find(",")) != std::string::npos)
	{
		tmp = channels_list.substr(0, pos);
		channels.push_back(tmp);
		channels_list.erase(0, pos + 1);
	}
	if (!channels_list.empty())
		channels.push_back(channels_list);
	if (channels.size() < 1)
		throw NeedMoreParamsException("Need more params");
	std::vector<std::string> reasons;
	if (!input.empty() && input[0] == ':')
		input.erase(0, 1);
	while ((pos = input.find(",")) != std::string::npos)
	{
		tmp = input.substr(0, pos);
		reasons.push_back(tmp);
		input.erase(0, pos + 1);
	}
	if (!input.empty())
		reasons.push_back(input);
	for(int i = 0; i < channels.size(); i++)
	{
		if(i < reasons.size())
			list.insert(std::pair<std::string, std::string>(channels[i], reasons[i]));
		else
			list.insert(std::pair<std::string, std::string>(channels[i], ""));
	}
	for(std::map<std::string, std::string>::iterator it = list.begin(); it != list.end(); ++it)
	{
		//HANDLE PART OF EVERY CHANNEL IN THE LIST!!!
		Message msg;
		Channel *c = server.searchChannel(it->first);
		if (!c)
		{
			Message msg;
			msg.reply(NULL, client, ERR_NOSUCHCHANNEL_CODE, SERVER, ERR_NOSUCHCHANNEL, client.getNickname().c_str(), (*it).first.c_str());
			continue;
		}
		if (!c->isClientInChannel(client.getNickname()))
		{
			Message msg;
			msg.reply(NULL, client, ERR_NOTONCHANNEL_CODE, SERVER, ERR_NOTONCHANNEL, client.getNickname().c_str(), (*it).first.c_str());
			continue;
		}
		c->eraseClient(client.getNickname(), it->second, 0);
		client.removeChannel(c->getName());
	}
}
