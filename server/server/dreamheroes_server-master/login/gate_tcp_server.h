#ifndef __gate_tcp_server_h__
#define __gate_tcp_server_h__
class GateTcpServer : public tcp_server
{
public:
	GateTcpServer(void);
	~GateTcpServer(void);

public:
	virtual tcp_session* create_session();
	virtual void run();
};

#endif