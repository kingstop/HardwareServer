#include "stdafx.h"
#include "session.h"
#include "DreamHero.h"
//#include "player.h"
//#include "message/girls.pb.h"
//#include "FRWorld.h"

/************************************************************************/

/*                          注册消息实例                                */  

/*  message::MsgDB2GSQueryCharResult 注册回调    prasePBTest             */

/************************************************************************/
typedef void(Session::*pn_msg_cb)(google::protobuf::Message*);
static std::map<std::string, pn_msg_cb > static_session_cb_funs;
static void registerCBFun(std::string str, pn_msg_cb fun)
{
    static_session_cb_funs.insert(std::make_pair(str, fun));
}

void Session::prasePBDefault(google::protobuf::Message* p)
{
     Mylog::log_server(LOG_INFO, "Parse message name [%s]", p->GetTypeName().c_str());
}

//这里负责注册消息
void Session::registerPBCall()
{
	registerCBFun(PROTOCO_NAME(message::C2SModifySuitNameReq), &Session::parseModifySuitName);
	registerCBFun(PROTOCO_NAME(message::C2SModifySuitReq), &Session::parseModifySuit);
	registerCBFun(PROTOCO_NAME(message::C2SDelSuitReq), &Session::parseDelSuit);
	registerCBFun(PROTOCO_NAME(message::C2SVerifyToyCDKeyReq), &Session::parseC2SVerifyToyCDKeyReq);
}

void Session::parseModifySuitName(google::protobuf::Message* p)
{
	message::C2SModifySuitNameReq* msg = (message::C2SModifySuitNameReq*) p;
	if (_dream_hero)
	{
		_dream_hero->modify_suit(msg->suit_id(), msg->suit_name().c_str());
	}
}

int Session::getState()
{
	return m_state;
}

void Session::parseModifySuit(google::protobuf::Message* p)
{
	message::C2SModifySuitReq* msg = (message::C2SModifySuitReq*)p;
	if (_dream_hero)
	{
		_dream_hero->ModifySuit(msg);
	}
}

void Session::parseDelSuit(google::protobuf::Message* p)
{
	message::C2SDelSuitReq* msg = (message::C2SDelSuitReq*)p;
	if (_dream_hero)
	{
		_dream_hero->DelSuit(msg);
	}
}

void Session::parseC2SVerifyToyCDKeyReq(google::protobuf::Message* p)
{
	message::C2SVerifyToyCDKeyReq* msg = (message::C2SVerifyToyCDKeyReq*)p;
	if (_dream_hero)
	{
		_dream_hero->VerifyToyCDKey(msg);
	}

}


void Session::parsePBMessage(google::protobuf::Message* p)
{
    std::map<std::string, pn_msg_cb >::iterator it = static_session_cb_funs.find(p->GetTypeName());
    if (it != static_session_cb_funs.end())
    {
        pn_msg_cb fun = boost::ref( it->second);
        if ( NULL != fun )
        {
            (this->*fun)(p);
            return ;
        }
    }
    prasePBDefault(p);
}

//////////////////////////////////////////////////////////////////////////

Session::Session(tran_id_type t, account_type a, u16 gate)
    :m_tranid(t), m_account(a), m_gate(gate),m_state(_session_online_), _dream_hero(NULL)
{
	DreamHero* heros = gDreamHeroManager.GetHero(m_account);
	if (heros != NULL)
	{
		_dream_hero = heros;
		_dream_hero->set_session(this);
		_dream_hero->SendClientInit();
		
	}
	else
	{
		message::MsgReqHeroDataGS2DB msg;
		msg.set_account(m_account);
		gGSDBClient.sendPBMessage(&msg, m_tranid);
	}

}

Session::~Session()
{

}

void Session::close()
{

}

void Session::setReconnet()
{
    m_state = _session_online_;
	if (_dream_hero)
	{
		_dream_hero->set_online(true);
	}
}

void Session::setWaitReconnet()
{
    m_state = _session_offline_;
	_dream_hero->set_online(false);
}

void Session::sendPBMessage(google::protobuf::Message* p)
{
    if (m_state == _session_online_)
    {
	    gGSGateManager.sendMessage(p, m_tranid, m_gate);
    }
	else
    {

    }
}



void Session::parseDBQueryNeedCreateHero()
{
	_dream_hero = gDreamHeroManager.CreateHero(m_account, this);
	if (_dream_hero)
	{
		_dream_hero->SendClientInit();
	}
}

void Session::praseDBQueryHeroInfo(message::MsgHeroDataDB2GS* HeroDataMsg)
{
	if (HeroDataMsg)
	{
		_dream_hero = gDreamHeroManager.CreateHero(HeroDataMsg, m_account, this);
		if (_dream_hero)
		{
			_dream_hero->SendClientInit();
		}		
	}
}

DreamHero* Session::get_dream_hero()
{
	return _dream_hero;
}

