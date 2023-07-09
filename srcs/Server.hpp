/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andrferr <andrferr@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/14 14:25:54 by helneff           #+#    #+#             */
/*   Updated: 2023/07/09 15:31:31 by andrferr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <exception>
#include <poll.h>
//Just to work on my wsl at home
#include <stdexcept>
#include <string.h>

#include "Client.hpp"
#include "Time.hpp"
#include "Channel.hpp"

class Server
{
public:
	struct InitException : public std::runtime_error {
		InitException(const std::string &msg) : runtime_error(msg) {}
	};
	struct RecvException : public std::runtime_error {
		RecvException(const std::string &msg) : runtime_error(msg) {}
	};
	struct UserRegistrationException : public std::runtime_error {
		UserRegistrationException(const std::string &msg) : runtime_error(msg) {}
	};
	struct DuplicateNickException : public std::runtime_error {
		DuplicateNickException(const std::string &msg) : runtime_error(msg) {}
	};
	struct ChannelLenException : public std::runtime_error {
		ChannelLenException(const std::string &msg) : runtime_error(msg) {}
	};

	Server(const char *port, std::string pass);
	~Server();

	void pollClientEvents();
	void logMessage(int fd, std::string msg, std::string nickname) const;
	Channel *searchChannel(std::string name);
	void addChannel(Channel *channel, Client &client);
	Channel *createChannel(std::string name, std::string topic, std::string pass, Client &client);
	int totalChannels() const;
	std::vector<Channel*> &getChannels();
	std::string getCreationTimeAsString() const;
	std::string getCreationTimestampAsString() const;
	Client *findClientByNick(std::string nick);
	std::string toLowercase(std::string nick);
	bool isDuplicate(std::string nick);

private:
	static const int listenTimeout = 10;
	static const int bufferSize = 1024;
	char buffer[bufferSize];
	int sockfd;
	std::string pass;
	std::vector<pollfd> pollfds;
	std::vector<Client*> clients;
	std::vector<Channel*> channels;
	Time creationTime;

	void registerNewUser();
	void eraseDisconnectedUsers();
	void eraseEmptyChannels();
	void handleClientMessage(Client &client);
	std::vector<pollfd>::iterator findPollfdByFD(int fd);
	std::vector<Client*>::iterator findClientByFD(int fd);
	std::vector<Client*>::iterator eraseUserByFD(int fd);
	void checkDuplicateNick(std::string nick);
	std::string getISupportAsString() const;
};

#endif
