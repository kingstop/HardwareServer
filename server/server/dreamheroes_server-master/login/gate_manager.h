#ifndef __gate_manager_h__
#define __gate_manager_h__
class GateSession ;
class UserLoginSession;
struct Account;
class GateManager
{
public:
    GateManager();
    ~GateManager();
    void addGate(u16 nId, GateSession* pkGate);
    void removeGate(GateSession* pkGate);

    bool kickUserFromGate(Account* pkAccount);
    bool giveUserToGate(Account* pkAccount, UserLoginSession* pkSession);
    void receiveUserPrepar(account_type ntran, const std::string& ip, u16 port);
    GateSession* getIdleGate();
    UserLoginSession* getWaiteSession(account_type nAccountId);
    void removeWaitSession(account_type nAccountId);
protected:
private:
    GateSession* m_Gates[MAX_GATE_ID];
    obj_ptr_map<u32, UserLoginSession>m_WaitSessions;
};
#endif