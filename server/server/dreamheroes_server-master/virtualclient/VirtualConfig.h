#pragma once
class VirtualConfig
{
public:
	VirtualConfig();
	virtual ~VirtualConfig();
	void set_account_begin(int n);
	void set_account_end(int n);
	int get_account_begin();
	int get_account_end();
	const char* get_virtual_account();
	const std::vector<std::string>& get_accounts();
public:
	void GenerateAccount();
	
protected:
	std::string _virtual_account;
	int _account_begin;
	int _account_end;
	std::vector<std::string> _accounts;

};

