#include "stdafx.h"
#include "user_session.h"
#include "gate_session.h"

GateManager::GateManager()
{
    for (u16 i = 0 ; i < MAX_GATE_ID; ++i)
    {
        m_Gates[i] = NULL;
    }
}
GateManager::~GateManager()
{

}
void GateManager::addGate(u16 nId, GateSession* pkGate)
{
    if (!pkGate)
    {
        Mylog::log_server( LOG_ERROR, "Add Gate[%d] error!  Gate Pointer is null", nId);
        return ;
    }
    if (nId > INVALID_GATE_ID && nId < MAX_GATE_ID && pkGate != NULL)
    {
        m_Gates[ nId ] = pkGate;
        Mylog::log_server( LOG_INFO, "Add Gate[%d] Active", nId);
        return ;
    }
    Mylog::log_server( LOG_ERROR, "Add Gate[%d] error!", nId);
}
void GateManager::removeGate(GateSession* pkGate)
{
    if (NULL == pkGate)
    {   return ;}
    u16 nId = pkGate->getGateId();
    if (nId > INVALID_GATE_ID && nId < MAX_GATE_ID )
    {
        if (m_Gates[ nId ] == pkGate )
        {
            m_Gates[ nId ] = NULL;
            Mylog::log_server( LOG_INFO, "Remove Active Gate[%d] ", nId);
        }
    }
}

bool GateManager::kickUserFromGate(Account* pkAccount)
{
    u16 id = pkAccount->nGateId;
    if (id >= INVALID_GATE_ID && id < MAX_GATE_ID)
    {
        if (m_Gates[id])
        {
           message::MsgLN2GTKickUser msg;
           ::message::GTLGData* data = msg.mutable_data();
           data->set_account(pkAccount->nId);
           data->set_transid(pkAccount->nTranId);
           m_Gates[id]->sendPBMessage(&msg);
           Mylog::log_player(LOG_INFO, "kick user account [%u]", pkAccount->nId);
        }
    }
    return true;
}
bool GateManager::giveUserToGate(Account* pkAccount, UserLoginSession* pkSession)
{
    if (pkSession && pkAccount)
    {
        GateSession* pkGate = getIdleGate();
        if (pkGate)
        {
            pkAccount->nGateId = pkGate->getGateId();
            message::MsgLN2GTUserLogin msg;
            ::message::GTLGData* data = msg.mutable_data();
            data->set_account(pkAccount->nId);
            data->set_transid(pkAccount->nTranId);

            pkSession->setAccount(pkAccount->nId);
            pkSession->setState(UserLoginSession::_wait_gate_);
			pkGate->addBalenceTime();
            m_WaitSessions.addData(pkAccount->nTranId, pkSession);
            pkGate->sendPBMessage(&msg);
			
            //Mylog::log_player( LOG_INFO, "set Account [%d]'s session to gate [%d]!",  pkAccount->nId, pkAccount->nGateId);
            return true;
        }
		else
        {
            Mylog::log_player( LOG_ERROR, "not find the gate server!");
        }
    }
    return false;
}
void GateManager::receiveUserPrepar(account_type ntran, const std::string& ip, u16 port)
{
    UserLoginSession* pkSession = getWaiteSession(ntran);
    if (pkSession)
    {
        message::LoginResponse msg;
        msg.set_gate_ip(ip);
        msg.set_gate_port(port);
        msg.set_user_account(ntran);
        msg.set_result(message::enumLoginResult_Success);
        pkSession->sendPBMessage(&msg);
        pkSession->setState(UserLoginSession::_wait_close_);
        removeWaitSession(ntran);
        //Mylog::log_player( LOG_INFO, "Handle gate prepair for user [%u]", ntran);
    }
}
GateSession* GateManager::getIdleGate()
{
    GateSession* pkGate = NULL;
    for (u16 i = 0; i < MAX_GATE_ID; i++)
    {
        if (m_Gates[ i ] != NULL)
        {
            if (pkGate == NULL)
            {
                pkGate = m_Gates[ i];
            }else
            {
                if (pkGate->getBalenceCount() > m_Gates[i]->getBalenceCount())
                {
                    pkGate = m_Gates[i];
                }
            }
        }

    }
    return pkGate;
}
UserLoginSession* GateManager::getWaiteSession(account_type nAccountId)
{
    return m_WaitSessions.getData(nAccountId);
}
void GateManager::removeWaitSession(account_type nAccountId)
{
    m_WaitSessions.eraseData(nAccountId);
}