#include "stdafx.h"
#include "client.h"
#include "json_help.h"
#include "client_manager.h"
#include "virtualclientDlg.h"

ClientTest::ClientTest()
    :m_EventHold(WORLD_INSTANCE)
{
    m_maxCount = m_onceCount = m_curCount = m_accountBegin = 0;
    m_Online = 0;
    m_TransCount = 0;
    m_accountBegin = 1;
    m_onceTime = 1000;
    m_WaitReconnet.clear();
    m_Reconns.clear();
	_client_init = false;
};


bool ClientTest::init(int accountBegin, int accountEnd)
{
	if (_client_init == false)
	{
		ServerFrame::init();
		Client::initPBModule();
		m_accountBegin = accountBegin;
		int count = accountEnd - accountBegin;
		if (count < 0)
		{
			count = 0;
		}
		m_maxCount = count;
		JsonHelper help("conf/robot.json");
		if (!help.loadJson())
		{
			return false;
		}
		const Json::Value& Root = help.root();
		LAZY_JSON_GET_UINT(m_onceCount, "once_count", Root);
		LAZY_JSON_GET_UINT(m_info._port, "port", Root);
		LAZY_JSON_GET_UINT(m_onceTime, "once_time", Root);
		LAZY_JSON_GET_STRING(m_info._ip, "ip", Root);
		if (m_onceTime < 10)
		{
			m_onceTime = 100;
		}
		addRobot();
		gEventMgr.addEvent(this, &ClientTest::reConnetClient, EVENT_ADD_TEST_ACCOUNT, m_onceTime, 0, 0);
	}
	_client_init = true;
    return true;
}

void ClientTest::addToReconn(Client* p)
{
    m_WaitReconnet.push_back(p);
}

void ClientTest::removeFromReconn(Client* p)
{
    m_Reconns.erase(p);
}

void ClientTest::reConnetClient()
{
    Client* p = NULL;
    m_curCount ++;    
    while( !m_WaitReconnet.empty() &&  m_Reconns.size() < m_onceCount)
    {
        p = m_WaitReconnet.front();
        if (p)
        {
            p->reConnect();
            m_Reconns.insert(p);
            m_WaitReconnet.pop_front();
        }
    }    
}

void ClientTest::addRobot()
{
	for (u32 i = 0; i < m_maxCount; ++i)
	{
		Client* p = new Client(i + m_accountBegin, m_info);
		m_Clients.push_back(p);
		addToReconn(p);
	}
}
bool ClientTest::onKey()
{
    return true;
}

bool ClientTest::isStop()
{
	return _stop;
}

void ClientTest::runOnce(u32 nDiff)
{
    m_Online = 0;
    m_TransCount = 0;
    net_global::update_net_service();
	if (_client_init == true)
	{
		std::vector<Client*>::iterator it = m_Clients.begin();
		std::vector<Client*>::iterator itEnd = m_Clients.end();
		u32  reconn_count = m_onceCount;
		for (it; it != itEnd; ++it)
		{
			Client* p = (*it);
			if (p)
			{
				p->update(nDiff);
				if (p->IsOK())
				{
					m_Online++;
				}
				if (p->IsGetTrans())
				{
					m_TransCount++;
				}
			}
		}
	}
    m_EventHold.update(nDiff);
    static u32 curtime = 0;
    curtime += nDiff;
    if (curtime > 10000 && m_Online > 0)
    {
        Mylog::log_server(LOG_INFO, "Onlines:[%u], trans:[%u], wait connet Count:[%u]", m_Online, m_TransCount, m_WaitReconnet.size());
        curtime = 0;
    }
}


void ClientTest::addOnlineClient(Client* p)
{
	std::string acc_name = p->getAcc();
	MAP_CLIENTS::iterator it = _offline_clients.find(p->getAcc());
	if (it != _offline_clients.end())
	{		
		_offline_clients.erase(it);
		ClientListModify(acc_name.c_str(), offline_client, false);
	}

	it = _online_clients.find(p->getAcc());
	if (it == _online_clients.end())
	{
		_online_clients[p->getAcc()] = p;
		ClientListModify(acc_name.c_str(), online_client, true);
	}
}

void ClientTest::addOfflineClient(Client* p)
{
	std::string acc_name = p->getAcc();
	MAP_CLIENTS::iterator it = _online_clients.find(p->getAcc());
	if (it != _online_clients.end())
	{
		_online_clients.erase(it);
		ClientListModify(acc_name.c_str(), online_client, false);
	}

	it = _offline_clients.find(p->getAcc());
	if (it == _offline_clients.end())
	{
		_offline_clients[p->getAcc()] = p;
		ClientListModify(acc_name.c_str(), offline_client, true);
	}
}

void ClientTest::ClientListModify(const char* sztemp, enClientType en, bool add)
{
	g_app->ClientListModify(sztemp, en, add);
}


void ClientTest::shutDown()
{
    
}
void ClientTest::memoryLog() 
{

}
void ClientTest::setStop()
{

}
void ClientTest::checkStop()
{

}