#pragma once

#include <string>
#include <vector>

struct SRecipInfo
{
	std::string strRecipName;
	std::string strRecipEMail;
};

struct SSendMailInfo
{
	std::string					strServer;
	std::string					strSender;
	std::string					strSenderName;
	std::vector< SRecipInfo >	vecRecip;
	std::string					strSubject;
	std::string					strText;
};


void	SendMail( const SSendMailInfo* pMailInfo );