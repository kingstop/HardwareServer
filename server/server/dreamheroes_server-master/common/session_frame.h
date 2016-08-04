#ifndef __session_frame_h__
#define __session_frame_h__

template<class Class, typename TP>
class TSessionManager : public DataMgr<TP, Class>
{
public:
	TSessionManager(){};
	virtual ~TSessionManager(){ShutDown();}

	virtual bool Init()
	{
		Class::InitSessionFuns();
		return true;
	}
	virtual void Update(u32 diff){;}
	virtual void ShutDown()
	{
		typename std::map<TP, Class*>::iterator it = DataMgr<TP, Class>::m_InstanceMap.begin();
		for (it; it != DataMgr<TP, Class>::m_InstanceMap.end(); ++it)
		{
			Memory::Release<Class>( it->second);
			//delete it->second;
		}
		DataMgr<TP, Class>::m_InstanceMap.clear();
	}

	bool AddNewSession(TP p)
	{
		Class* pkNew = Memory::Create<Class, TP>(p);
		if (pkNew)
		{
			return AddData(p, pkNew);
		}
		return false;
	}
	void RemoveSession(TP p)
	{
		Class* pkNew =  EraseData(p);
		if (pkNew)
		{
			Memory::Release<Class>(pkNew);
		}
	}
	void HandleMessage(TP p, const PacketUsn& packet)
	{
		Class* pkNew =  GetData(p);
		if (pkNew)
		{ pkNew->HandlePacket( const_cast<PacketUsn&>(packet) );}
	}
protected:
};
#endif