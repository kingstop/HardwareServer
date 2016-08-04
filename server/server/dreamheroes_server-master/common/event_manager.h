#ifndef __EVENTMGR_H
#define __EVENTMGR_H

#include "common_type.h"
#include "common_template.h"
#include "event_call_back.h"

enum EventFlags
{
	EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT = 0x1,  // 在全局的管理中，不执行事件
	EVENT_FLAG_DELETES_OBJECT				   = 0x2,  // 执行后删除事件对象
};

struct  TimedEvent
{
	TimedEvent(void* object, CallbackBase* callback, u32 type, u32 time, s32 repeat, u32 flags) :
		obj(object), cb(callback), eventType(type), eventFlag(flags), msTime(time), currTime(time), repeats(repeat), deleted(false),ref(0) {}
		
	void *obj;
	CallbackBase *cb;
	u32 eventType;
	u16 eventFlag;
	u32 msTime;  //时间间隔
	u32 currTime; //剩下的回调时间
	s32 repeats;
	bool deleted;
	int instanceId;
    s32 ref;
	static TimedEvent * Allocate(void* object, CallbackBase* callback, u32 flags, u32 time, s32 repeat)
	{
		return new TimedEvent(object, callback, flags, time, repeat, 0);
	}

#ifdef WIN32
	void decRef()
	{
		//InterlockedDecrement(&ref);
        obj = NULL;
        deleted = true;

        -- ref;
		if(ref <= 0)
		{
			delete cb;
			delete this;
		}
	}

	void incRef() { /*InterlockedIncrement(&ref)*/ ++ ref; }
#else


	void incRef() { ++ref; }
    
	void decRef()
	{
        obj = NULL;
        deleted = true;

		--ref;
		if(ref <= 0)
		{
			 delete cb;
			 delete this;
		}
	}
#endif

};

class EventableObjectHolder;
typedef std::map<s32, EventableObjectHolder*> HolderMap;

class EventMgr : public singletion < EventMgr >
{
	friend class MiniEventMgr;
public:
	template <class Class>
		void addEvent(Class *obj, void (Class::*method)(), u32 type, u32 time, s32 repeats, u32 flags)
	{
		
		TimedEvent * event = new TimedEvent(obj, new CallbackP0<Class>(obj, method), type, time, repeats, flags);

		
		obj->eventAddEvent(event);
	}

	template <class Class, typename P1>
		void addEvent(Class *obj, void (Class::*method)(P1), P1 p1, u32 type, u32 time, s32 repeats, u32 flags)
	{
		
		TimedEvent * event = new TimedEvent(obj, new CallbackP1<Class, P1>(obj, method, p1), type, time, repeats, flags);

		
		obj->eventAddEvent(event);
	}

	template <class Class, typename P1, typename P2>
		void addEvent(Class *obj, void (Class::*method)(P1,P2), P1 p1, P2 p2, u32 type, u32 time, s32 repeats, u32 flags)
	{
		
		TimedEvent * event = new TimedEvent(obj, new CallbackP2<Class, P1, P2>(obj, method, p1, p2), type, time, repeats, flags);

		
		obj->eventAddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3>
		void addEvent(Class *obj,void (Class::*method)(P1,P2,P3), P1 p1, P2 p2, P3 p3, u32 type, u32 time, s32 repeats, u32 flags)
	{
		
		TimedEvent * event = new TimedEvent(obj, new CallbackP3<Class, P1, P2, P3>(obj, method, p1, p2, p3), type, time, repeats, flags);

		
		obj->eventAddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4>
		void addEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4), P1 p1, P2 p2, P3 p3, P4 p4, u32 type, u32 time, s32 repeats, u32 flags)
	{
		TimedEvent * event = new TimedEvent(obj, new CallbackP4<Class, P1, P2, P3, P4>(obj, method, p1, p2, p3, p4), type, time, repeats, flags);
		obj->eventAddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5>
	void addEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4,P5), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, u32 type, u32 time, s32 repeats, u32 flags)
	{
		TimedEvent * event = new TimedEvent(obj, new CallbackP5<Class, P1, P2, P3, P4, P5>(obj, method, p1, p2, p3, p4, p5), type, time, repeats, flags);
		obj->eventAddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
	void addEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4,P5,P6), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6,u32 type, u32 time, s32 repeats, u32 flags)
	{
		TimedEvent * event = new TimedEvent(obj, new CallbackP6<Class, P1, P2, P3, P4, P5, P6>(obj, method, p1, p2, p3, p4, p5, p6), type, time, repeats, flags);
		obj->eventAddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
	void addEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4,P5,P6,P7), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, u32 type, u32 time, s32 repeats, u32 flags)
	{
		TimedEvent * event = new TimedEvent(obj, new CallbackP7<Class, P1, P2, P3, P4, P5, P6, P7>(obj, method, p1, p2, p3, p4, p5, p6, p7), type, time, repeats, flags);
		obj->eventAddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
	void addEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4,P5,P6,P7, P8), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, u32 type, u32 time, s32 repeats, u32 flags)
	{
		TimedEvent * event = new TimedEvent(obj, new CallbackP8<Class, P1, P2, P3, P4, P5, P6, P7, P8>(obj, method, p1, p2, p3, p4, p5, p6, p7, p8), type, time, repeats, flags);
		obj->Event_AddEvent(event);
	}


	template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
	void addEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4,P5,P6,P7,P8, P9), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, u32 type, u32 time, s32 repeats, u32 flags)
	{
		TimedEvent * event = new TimedEvent(obj, new CallbackP9<Class, P1, P2, P3, P4, P5, P6, P7, P8, P9>(obj, method, p1, p2, p3, p4, p5, p6, p7, p8, p9), type, time, repeats, flags);
		obj->eventAddEvent(event);
	}

	template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
	void addEvent(Class *obj, void (Class::*method)(P1,P2,P3,P4,P5,P6,P7,P8, P9, P10), P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, u32 type, u32 time, s32 repeats, u32 flags)
	{
		TimedEvent * event = new TimedEvent(obj, new CallbackP10<Class, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10>(obj, method, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10), type, time, repeats, flags);
		obj->eventAddEvent(event);
	}
	template <class Class> void removeEvents(Class *obj) { obj->Event_RemoveEvents(-1); }
	template <class Class> void removeEvents(Class *obj, s32 type)
	{
		obj->eventRemoveEvents(type);
	}

	template <class Class> void modifyEventTimeLeft(Class *obj, u32 type, u32 time,bool unconditioned=true)
	{
		obj->eventModifyTimeLeft(type, time,unconditioned);
	}

	template <class Class> void modifyEventTimeAndTimeLeft(Class *obj, u32 type, u32 time)
	{
		obj->eventModifyTimeAndTimeLeft(type, time);
	}

	template <class Class> void modifyEventTime(Class *obj, u32 type, u32 time)
	{
		obj->eventModifyTime(type, time);
	}

	template <class Class> bool hasEvent(Class *obj, u32 type)
	{
		return obj->eventHasEvent(type);
	}

    template <class Class> u32 getEventTimeLeft(Class *obj, u32 type)
    {
        u32 nt = 0;
        obj->eventGetTimeLeft( type, &nt );
        return nt;
    }
	EventableObjectHolder * getEventHolder(s32 InstanceId)
	{
		HolderMap::iterator itr = mHolders.find(InstanceId);
		if(itr == mHolders.end()) return 0;
		return itr->second;
	}

	void addEventHolder(EventableObjectHolder * holder, s32 InstanceId)
	{
		mHolders.insert( HolderMap::value_type( InstanceId, holder) );
	}

	void removeEventHolder(s32 InstanceId)
	{
		mHolders.erase(InstanceId);
	}

	void removeEventHolder(EventableObjectHolder * holder)
	{
		HolderMap::iterator itr = mHolders.begin();
        HolderMap::iterator itend = mHolders.end();
		for(; itr != itend; ++itr)
		{
			if(itr->second == holder)
			{
				mHolders.erase(itr);
				return;
			}
		}
	}

protected:
	HolderMap mHolders;
};

#define gEventMgr EventMgr::instance()

#endif
