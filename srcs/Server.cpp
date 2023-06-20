#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "Server.hpp"
#include "Message.hpp"
#include "Parser.hpp"

Server::Server(const char *port, std::string pass)
{
	this->pass = pass;
	addrinfo hints;
	bzero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	addrinfo *servinfo;
	int status = getaddrinfo(NULL, port, &hints, &servinfo);
	if (status != 0)
		throw InitException(std::string("Error: getaddrinfo: ") + gai_strerror(status));
	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (sockfd == -1)
		throw InitException(std::string("Error: socket: ") + strerror(errno));

	if (fcntl(sockfd, F_SETFL, O_NONBLOCK) == -1)
		throw InitException(std::string("Error: fcntl: ") + strerror(errno));

	int yes = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
		throw InitException(std::string("Error: setsockopt: ") + strerror(errno));

	if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		throw InitException(std::string("Error: bind: ") + strerror(errno));

	freeaddrinfo(servinfo);

	if (listen(sockfd, listenTimeout) == -1)
		throw InitException(std::string("Error: listen: ") + strerror(errno));

	pollfd server_poll_fd;
	server_poll_fd.fd = sockfd;
	server_poll_fd.events = POLLIN;
	pollfds.push_back(server_poll_fd);
}


Server::~Server() { close(sockfd); }

void Server::pollClientEvents()
{
	int event_count = 0;
	while ((event_count = poll(pollfds.data(), pollfds.size(), -1)) != -1)
	{
		for (std::vector<pollfd>::iterator it = pollfds.begin();
			it != pollfds.end() && event_count > 0; it++)
		{
			if (it->revents == 0)
				continue;
			event_count--;
			if (it->fd == sockfd)
				registerNewUser();
			else
				handleClientMessage(*findClientByFD(it->fd));
		}
		eraseDisconnectedUsers();
	}
	std::cerr << "Error: poll: " << strerror(errno) << std::endl;
}

void Server::registerNewUser()
{
	std::cout << "New connection received!" << std::endl;
	sockaddr_storage client_addr;
	socklen_t client_addr_size = sizeof(client_addr);
	int client_fd = accept(sockfd, (sockaddr *)&client_addr, &client_addr_size);
	if (client_fd == -1)
		throw UserRegistrationException(std::string("Error: accept: ") + strerror(errno));
	pollfd client_pollfd;
	client_pollfd.fd = client_fd;
	client_pollfd.events = POLLIN;
	pollfds.push_back(client_pollfd);
	clients.push_back(Client(client_fd));
}

void Server::eraseDisconnectedUsers()
{
	std::vector<Client>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if (it->isConnected() == false)
			it = eraseUserByFD(it->getClientFD());
		else
			it++;
	}
}

void Server::handleClientMessage(Client &client)
{
	int bytes_read = recv(client.getClientFD(), buffer, sizeof(buffer) - 1, 0);
	if (bytes_read == -1)
		throw RecvException(std::string("Error: recv: ") + strerror(errno));
	if (bytes_read == 0)
	{
		std::cout << "Client closed connection!" << std::endl;
		client.setConnected(false);
		return;
	}
	buffer[bytes_read] = '\0';
	Parser parser;
	parser.setInput(std::string(buffer));
	std::vector<std::string>	args = parser.parseInput();
	int i = 0;
	for(std::vector<std::string>::iterator it = args.begin();
		it != args.end(); ++it, i++)
	{
		std::cout << "i: " << i << " fd: " << client.getClientFD() << " it: " << *it << std::endl;
		if (it->find("PASS ") != std::string::npos && client.isConnected() && client.getActiveStatus() != PASSED)
		{
			try{
				parser.parsePass(*it, pass);
				client.setActiveStatus(PASSED);
			} catch (Parser::NoPassException &e){
				Message msg("Incorrect password\r\n");
				
				msg.sendData(client.getClientFD());
				client.setConnected(false);
				std::cerr << e.what() << std::endl;
				//SEND ERR_PASSWDMISMATCH (464) and then close the connection with ERROR.
			} catch (Parser::WrongInputException &e){
				Message msg("Invalid input\r\n");
				msg.sendData(client.getClientFD());
				client.setConnected(false);
				std::cerr << e.what() << std::endl;
				return ;
			}
		}
		else if (it->find("NICK ") != std::string::npos && client.isConnected() && client.getActiveStatus() == PASSED)
		{
			std::string nick;
			try {
				parser.parseNick(*it, nick);
				if (nick.empty())
				{
					Message msg;
					msg.reply(NULL, client, ERR_NONICKNAMEGIVEN_CODE, SERVER, ERR_NONICKNAMEGIVEN);
					break ;
				}
				checkDuplicateNick(nick);
				//TODO -> Check for all Nickname rules in here!!!!
				client.setNickname(nick);
			} catch (Parser::NoNickException &e){
				Message msg(e.what());
				msg.sendData(client.getClientFD());
			} catch (DuplicateNickException &e){
				Message msg;
				msg.reply(NULL, client, ERR_NICKNAMEINUSE_CODE, SERVER, ERR_NICKNAMEINUSE, nick.c_str());
				break;
			} catch (Parser::InvalidNickException &e){
				Message msg;
				for (char c : nick)
					std::cout << (int)c << std::endl;
				msg.reply(NULL, client, ERR_ERRONEUSNICKNAME_CODE, SERVER, ERR_ERRONEUSNICKNAME, "*", nick.c_str());
				break;
			}
		}
		else if (it->find("USER ") != std::string::npos && client.isConnected() && client.getActiveStatus() == PASSED)
		{
			Message msg;
			//msg.RPL_Welcome(client.getClientFD(), client.getNickname());
			msg.reply(NULL, client, RPL_WELCOME_CODE, SERVER, RPL_WELCOME, client.getNickname().c_str(), client.getNickname().c_str());
			std::cout << "nickname: " << client.getNickname() << std::endl;
			client.setActiveStatus(REGISTERED);
			//TODO -> handle the USER!!!
			//create user
			//check if the user is already registered
		}
		else
		{
			client.setBuffer( client.getBuffer() + *it);
		}

		if (client.isReadyToSend())
		{
			std::cout << "Client sent something!" << std::endl;
			std::cout << client.getBuffer() << std::endl;
			emit(client.getClientFD(), client.getBuffer());
			client.increaseTotalMessages();
			client.resetBuffer();
			std::cout << "User " << client.getClientFD() << " has sent " << client.getTotalMessages() << " messages" << std::endl;
		}
	}
}

void Server::emit(int client_fd, std::string msg)
{
	Message message(msg);
	for (std::vector<Client>::iterator it = clients.begin();
		it != clients.end(); ++it)
	{
		if (it->getClientFD() != sockfd && it->getClientFD() != client_fd)
			message.sendData(it->getClientFD());
	}
}

std::vector<pollfd>::iterator Server::findPollfdByFD(int fd)
{
	for (std::vector<pollfd>::iterator it = pollfds.begin();
		it != pollfds.end(); ++it)
		if (it->fd == fd) return it;
	return pollfds.end();
}

std::vector<Client>::iterator Server::findClientByFD(int fd)
{
	for (std::vector<Client>::iterator it = clients.begin();
		it != clients.end(); ++it)
		if (it->getClientFD() == fd) return it;
	return clients.end();
}

std::vector<Client>::iterator Server::eraseUserByFD(int fd)
{
	close(fd);

	std::vector<pollfd>::iterator poll_fd = findPollfdByFD(fd);
	if (poll_fd == pollfds.end()) return clients.end();
	pollfds.erase(poll_fd);

	std::vector<Client>::iterator client = findClientByFD(fd);
	if (client == clients.end()) return clients.end();
	return clients.erase(client);
}


void Server::checkDuplicateNick(std::string nick)
{
	for(std::vector<Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (it->getNickname() == nick)
			throw DuplicateNickException("Duplicate nick");
	}
}