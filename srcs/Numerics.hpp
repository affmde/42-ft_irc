/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Numerics.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andrferr <andrferr@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/20 10:41:53 by andrferr          #+#    #+#             */
/*   Updated: 2023/07/25 17:54:09 by andrferr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NUMERICS_HPP
# define NUMERICS_HPP

#define SERVER 0
#define CLIENT 1

#define RPL_WELCOME_CODE "001"
#define RPL_YOURHOST_CODE "002"
#define RPL_CREATED_CODE "003"
#define RPL_MYINFO_CODE "004"
#define RPL_ISUPPORT_CODE "005"
#define RPL_CHANNELMODEIS_CODE "324"
#define RPL_CREATIONTIME_CODE "329"
#define RPL_NOTOPIC_CODE "331"
#define RPL_TOPIC_CODE "332"
#define RPL_INVITELIST_CODE "336"
#define RPL_ENDOFINVITELIST_CODE "337"
#define RPL_INVITING_CODE "341"
#define RPL_NAMREPLY_CODE "353"
#define RPL_ENDOFNAMES_CODE "366"
#define RPL_MOTD_CODE "372"
#define RPL_MOTDSTART_CODE "375"
#define RPL_ENDOFMOTD_CODE "376"
#define ERR_NOSUCHNICK_CODE "401"
#define ERR_NOSUCHCHANNEL_CODE "403"
#define ERR_CANNOTSENDTOCHAN_CODE "404"
#define ERR_BADCAP_CODE "410"
#define ERR_NONICKNAMEGIVEN_CODE "431"
#define ERR_ERRONEUSNICKNAME_CODE "432"
#define ERR_NICKNAMEINUSE_CODE "433"
#define ERR_USERNOTINCHANNEL_CODE "441"
#define ERR_NOTONCHANNEL_CODE "442"
#define ERR_USERONCHANNEL_CODE "443"
#define ERR_NEEDMOREPARAMS_CODE "461"
#define ERR_ALREADYREGISTERED_CODE "462"
#define ERR_PASSWDMISMATCH_CODE "464"
#define ERR_CHANNELISFULL_CODE "471"
#define ERR_INVITEONLYCHAN_CODE "473"
#define ERR_BADCHANNELKEY_CODE "475"
#define ERR_CHANOPRIVSNEEDED_CODE "482"

#define RPL_WELCOME "%s :Welcome to the IRCSERV Network, %s"			//001
#define RPL_YOURHOST "%s :Your host is IRCSERV, running version 1.0.0"	//002
#define RPL_CREATED "%s :This server was created %s"					//003
#define RPL_MYINFO "%s %s %s"											//004
#define RPL_ISUPPORT "%s %s :are supported by this server"				//005
#define RPL_CHANNELMODEIS "%s %s %s"									//324
#define RPL_CREATIONTIME "%s %s %s"										//329
#define RPL_NOTOPIC "%s %s :No topic is set"							//331
#define RPL_TOPIC "%s %s :%s"											//332
#define RPL_INVITELIST "%s %s"											//336
#define RPL_ENDOFINVITELIST "%s :End of /INVITE list"					//337
#define RPL_INVITING "%s %s %s"											//341
#define RPL_NAMREPLY "%s %s %s :%s"										//353
#define RPL_ENDOFNAMES "%s %s :End of /NAMES list"						//366
#define RPL_MOTD "%s :%s"												//372
#define RPL_MOTDSTART "%s :- %s Message of the day - "					//375
#define RPL_ENDOFMOTD "%s :End of /MOTD command."						//376
#define ERR_NOSUCHNICK "%s %s :No such nick/channel"					//401
#define ERR_NOSUCHCHANNEL "%s %s :No such channel"						//403
#define ERR_CANNOTSENDTOCHAN "%s %s :Cannot send to channel"			//404
#define ERR_BADCAP "%s :Invalid CAP command"							//410
#define ERR_NONICKNAMEGIVEN "%s :No nickname given"						//431
#define ERR_ERRONEUSNICKNAME "%s %s :Erroneus nickname"					//432
#define ERR_NICKNAMEINUSE "%s %s :Nickname is already in use"			//433
#define ERR_USERNOTINCHANNEL "%s %s %s :They aren't on channel"			//441
#define ERR_NOTONCHANNEL "%s %s :You're not on that channel"			//442
#define ERR_USERONCHANNEL "%s %s %s :is already on channel"				//443
#define ERR_NEEDMOREPARAMS "%s %s :Not enough parameters"				//461
#define ERR_ALREADYREGISTERED "%s :You may not reregister"				//462
#define ERR_PASSWDMISMATCH "%s :Password incorrect"						//464
#define ERR_CHANNELISFULL "%s %s :Cannot join channel (+l)"				//471
#define ERR_INVITEONLYCHAN "%s %s :Cannot join channel (+i)"			//473
#define ERR_BADCHANNELKEY "%s %s :Cannot join channel (+k)"				//475
#define ERR_CHANOPRIVSNEEDED "%s %s :You're not channel operator" 		//482

#endif
