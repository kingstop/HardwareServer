#ifndef __game_tcp_server_h__
#define __game_tcp_server_h__
class GameTCPServer : public tcp_server
{
public:
	GameTCPServer(void);
	~GameTCPServer(void);
public:
	virtual tcp_session* create_session();
	virtual void run();
};
#endif