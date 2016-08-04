#ifndef __user_tcp_server_h__
#define __user_tcp_server_h__

class UserTcpServer : public tcp_server
{
public:
	UserTcpServer(void);
	~UserTcpServer(void);

public:
	virtual tcp_session* create_session();
	virtual void run();
};

#endif