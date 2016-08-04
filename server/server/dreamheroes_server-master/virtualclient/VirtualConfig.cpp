#include "stdafx.h"
#include "VirtualConfig.h"
#include "virtualclientDlg.h"


VirtualConfig::VirtualConfig()
{
	_virtual_account = "virtual_count";
	_account_begin = 0;
	_account_end = 0;
}


VirtualConfig::~VirtualConfig()
{
}


void VirtualConfig::set_account_begin(int n)
{
	_account_begin = n;
}

void VirtualConfig::set_account_end(int n)
{
	_account_end = n;
}

int VirtualConfig::get_account_begin()
{
	return _account_begin;
}

int VirtualConfig::get_account_end()
{
	return _account_end;
}

const char* VirtualConfig::get_virtual_account()
{
	return _virtual_account.c_str();
}


void VirtualConfig::GenerateAccount()
{
	char sz_temp[512];
	_accounts.clear();
	std::string str_temp;
	_account_begin = g_app->getAccountBegin();
	_account_end = g_app->getAccountEnd();
	Instance.client_manage_.init(_account_begin, _account_end);

	
	for (int i = _account_begin; i < _account_end; i++)
	{
		sprintf(sz_temp, "_%d", i);
		str_temp += sz_temp;
		_accounts.push_back(str_temp);
	}
}