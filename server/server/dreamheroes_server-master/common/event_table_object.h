#ifndef _EVENTABLE_OBJECT_H
#define _EVENTABLE_OBJECT_H

#include "event_manager.h"
#include "common_header.h"
#include "memory.h"

typedef std::list<TimedEvent*> EventList;
typedef std::multimap<u32, TimedEvent*> EventMap;

#define EVENT_REMOVAL_FLAG_ALL 0xFFFFFFFF
#define WORLD_INSTANCE -1

class  EventableObject 
{
	friend class EventMgr;
	friend class EventableObjectHolder;

protected:
	void eventRemoveEvents();
	void eventRemoveEvents(u32 EventType);
	void eventModifyTimeLeft(u32 EventType, u32 TimeLeft,bool unconditioned=false);
	void eventModifyTime(u32 EventType, u32 Time);
	void eventModifyTimeAndTimeLeft(u32 EventType, u32 Time);
	bool eventHasEvent(u32 EventType);
	void eventRemoveByPointer(TimedEvent * ev);
	inline s32 eventGetCurrentInstanceId() { return m_event_Instanceid; }
	bool eventGetTimeLeft(u32 EventType, u32 * Time);

public:
    EventableObject();
    virtual ~EventableObject();

	u32 eventGetEventPeriod(u32 EventType);
	inline bool eventHasEvents() { return m_events.size() > 0 ? true : false; }
	void eventAddEvent(TimedEvent * ptr);
	void eventRelocate();
	virtual s32 eventGetInstanceID() { return -1; }

protected:
	s32 m_event_Instanceid;
	EventMap m_events;
	EventableObjectHolder * m_holder;
	
};



typedef std::set<EventableObject*> EventableObjectSet;

class EventableObjectHolder : PUBLIC_BASE_OBJECT(EventableObjectHolder)
{
	REGISTER_POOL_INFO(EventableObjectHolder, 10, 30)
public:
	EventableObjectHolder(s32 instance_id);
	~EventableObjectHolder();

	void update(u32 time_difference);

	void addEvent(TimedEvent * ev);
	void addObject(EventableObject * obj);

	inline u32 getInstanceID() { return mInstanceId; }

protected:
	s32 mInstanceId;
	EventList m_events;
};

#endif
