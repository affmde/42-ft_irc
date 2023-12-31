/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kick.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: helneff <helneff@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 17:01:57 by andrferr          #+#    #+#             */
/*   Updated: 2023/07/26 12:53:11 by helneff          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KICK_HPP
# define KICK_HPP

#include <map>

#include "ACommand.hpp"

class Kick : public ACommand
{
public:
	Kick(Server &server, Client &client, std::string &input, std::vector<Client*> &clientsList);
	Kick(const Kick &other);
	~Kick();
	Kick &operator=(const Kick &other);

	void exec();

private:
	std::string target;
	std::string reason;
	std::map<std::string, std::string> usersToKick;

	void parseInput();
};

#endif
