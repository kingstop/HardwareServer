#include "stdafx.h"
#include "user_session.h"
enum
{
    _WAITE_CONNECT_TIME_ = 5 * _TIME_MINUTE_ * _TIME_SECOND_MSEL_,
    _WAITE_RECONNECT_TIME_ = 5 * _TIME_MINUTE_ * _TIME_SECOND_MSEL_,

    _TELL_LOGIN_GATE_INFO = 30 * _TIME_SECOND_MSEL_,
};
void UserManager::addToWait(tran_id_type t,  account_type a)
{
    UserSession* p = m_onlines.getData(t);
    if (p)
    {   kickUser(t, a);}

    m_wait_map.addData(t, a);
    gEventMgr.addEvent(this, &UserManager::eventCallRemoveWait, t, EVENT_REMOVE_WAIT_CONNECT, _WAITE_CONNECT_TIME_, 1, EVENT_FLAG_DELETES_OBJECT);
    
    message::MsgGT2LNPrepar msg;
    msg.set_account(t);
    gGTLoginClient.sendPBMessage(&msg);
    //Mylog::log_player(LOG_INFO,"add user account[%u], trans[%u] .", a, t);
}
void UserManager::eventCallRemoveWait(tran_id_type t)
{
    account_type v = INVALID_ACCOUNT ;
    m_wait_map.eraseData(t, v);
}
void UserManager::kickUser(tran_id_type t,  account_type a)
{
    UserSession* p = m_onlines.getData(t);
    if (p)
    {
		GateGameClient* pkNew = gGTGameMgr.getGameClient(p->getGSid());
		if (pkNew)
		{pkNew->removePlayer(t);}
        p->close();
        m_onlines.eraseData(t);
        m_onlineaccs.eraseData(a);
        return ;
    }else
    {
        
    }

    account_type v = INVALID_ACCOUNT;
    m_wait_map.eraseData(t, v);
    if (v != INVALID_ACCOUNT)
    {   m_onlineaccs.eraseData(v);}

    u16 gs = INVALID_GAME_ID; 
    if (m_recon_map.eraseData(t, gs))
    {
        GateGameClient* pkNew = gGTGameMgr.getGameClient(gs);
        if (pkNew)
        {pkNew->removePlayer(t);}
    }
}
void UserManager::removeUsers(u16 gs)
{
   std::map<tran_id_type, UserSession*> sonlinemap = m_onlines.getDataMap();
   std::map<tran_id_type, UserSession*>::iterator it = sonlinemap.begin();
   std::map<tran_id_type, UserSession*>::iterator itend = sonlinemap.end();

   for (it; it!= itend; )
   {
       UserSession* p = it->second;
#ifdef WIN32
	   if (p && p->getGSid() == gs)
	   {
		   p->setState(UserSession::_wait_close_);
		   it = sonlinemap.erase(it);
	   }
	   else
	   {
		   ++it;
	   }
#else
	   if (p && p->getGSid() == gs)
	   {
		   p->setState(UserSession::_wait_close_);
		   sonlinemap.erase(it);
	   }
	   ++it;

#endif // DEBUG



   }

   m_recon_map.eraseValue(gs);
}
account_type UserManager::getAccount(tran_id_type t)
{
	account_type a = INVALID_ACCOUNT ;
	m_onlineaccs.getKey(a,t);
	return a;
}
void UserManager::enableGateEvent()
{
    if (!gEventMgr.hasEvent(this, EVENT_TELL_LOGIN_GATE_INFO))
    {
        gEventMgr.addEvent(this, &UserManager::evevntCall, EVENT_TELL_LOGIN_GATE_INFO, _TELL_LOGIN_GATE_INFO, 0,0);
    }
}
UserSession* UserManager::getUserSession(tran_id_type t)
{
	return m_onlines.getData(t);
}

void UserManager::evevntCall()
{
    message::MsgGT2LNOnlines msg;
    msg.set_onlines( m_onlineaccs.getSize());
    gGTLoginClient.sendPBMessage(&msg);
}
void UserManager::checkClose(UserSession* p)
{
    u32 tranid = p->getTrans();
    u32 gsid = p->getGSid();
    m_onlines.eraseData(tranid);
    GateGameClient* gs = gGTGameMgr.getGameClient(gsid);
    if (NULL != gs && UserSession::_connect_ == p->getState() )
    {   
        m_recon_map.addData(tranid, gsid);
        gs->offlinePlayer(tranid);
    }
    else
    {   
        m_onlineaccs.eraseValue(tranid);
        if (gs)
        {   gs->removePlayer(tranid);}
    }
    gEventMgr.addEvent(this, &UserManager::eventCallRemoveReconn, tranid, EVENT_REMOVE_RECONNECT, _WAITE_RECONNECT_TIME_, 1, EVENT_FLAG_DELETES_OBJECT);
}
void UserManager::eventCallRemoveReconn(tran_id_type t)
{
    u16 gsid = INVALID_GAME_ID;
    m_recon_map.eraseData(t, gsid);
    m_onlineaccs.eraseValue(t);
    if (INVALID_GAME_ID != gsid)
    {
         GateGameClient* p = gGTGameMgr.getGameClient(gsid);
         if (p)
         {  p->removePlayer(t);}
    }
}
bool UserManager::checkConn(tran_id_type t, UserSession* p)
{
    account_type v = INVALID_ACCOUNT;
    if (m_wait_map.hasData(t))
    {
        m_wait_map.eraseData(t, v);
        GateGameClient* gscliet = gGTGameMgr.getIdleGameClient();
        if (NULL == gscliet )
        {
            p->setState(UserSession::_wait_close_);
            return false;
        }
        gscliet->addNewPlayer(t, v);
        p->setGSid(gscliet->getGameId());
        p->setState(UserSession::_connect_);
        m_onlines.addData(t, p);
        m_onlineaccs.addData(v, t);
       // Mylog::log_player(LOG_INFO, "user[%u] connect count [%d]", t, m_onlineaccs.getDataMap().size());
        return true;
    }

    u16 gs = INVALID_GAME_ID;
    if (m_recon_map.getData(t, gs))
    {
        m_recon_map.eraseData(t);
        p->setGSid(gs);
        p->setState(UserSession::_connect_);
        m_onlines.addData(t, p);
        Mylog::log_player(LOG_INFO, "user[%u] reconnect", t);
        return true;
    }

    p->setState(UserSession::_wait_close_);
    return false;
}
void UserManager::makeRegisterData(message::MsgGTRegisterLG& msg)
{
    const std::map<account_type, tran_id_type>& m = m_onlineaccs.getDataMap();
    std::map<account_type, tran_id_type>::const_iterator it = m.begin();
    std::map<account_type, tran_id_type>::const_iterator itend = m.end();

    for (it; it != itend; ++it)
    {
        message::GTLGData* p = msg.add_accounts();
        p->set_account(it->first);
        p->set_transid(it->second);
    }
}
