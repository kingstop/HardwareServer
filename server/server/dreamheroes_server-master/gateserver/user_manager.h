#ifndef __user_manager_h__
#define __user_manager_h__

class UserSession;
class UserManager : public EventableObject
{
public:
    void addToWait(tran_id_type t,  account_type a);
    void kickUser(tran_id_type t,  account_type a);
    bool checkConn(tran_id_type t, UserSession* p);
    void checkClose(UserSession* p);

    void eventCallRemoveWait(tran_id_type t);
    void eventCallRemoveReconn(tran_id_type t);
    void makeRegisterData(message::MsgGTRegisterLG& msg);

    void removeUsers(u16 gs);
    void enableGateEvent();
    void evevntCall();

	UserSession* getUserSession(tran_id_type t);

	account_type getAccount(tran_id_type t);
protected:
private:
    obj_ptr_map<tran_id_type, UserSession> m_onlines;
    obj_map<account_type, tran_id_type> m_onlineaccs;
    obj_map<tran_id_type, account_type> m_wait_map;
    obj_map<tran_id_type, u16> m_recon_map;
};
#endif