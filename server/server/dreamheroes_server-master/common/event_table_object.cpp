#include "event_table_object.h"

using namespace  std;

EventableObject::~EventableObject()
{
	EventMap::iterator itr = m_events.begin();
	for(; itr != m_events.end(); ++itr)
	{
		itr->second->decRef();
	}

	m_events.clear();
}

EventableObject::EventableObject()
{
	m_holder = 0;
	m_event_Instanceid = -1;
}

void EventableObject::eventAddEvent(TimedEvent * ptr)
{
	if(!m_holder)
	{
		m_event_Instanceid = eventGetInstanceID();
		m_holder = gEventMgr.getEventHolder(m_event_Instanceid);
	}

	ptr->incRef();
	ptr->instanceId = m_event_Instanceid;
	pair<u32,TimedEvent*> p(ptr->eventType, ptr);
	m_events.insert( p );

	if(!m_holder)
	{
		m_event_Instanceid = -1;
		m_holder = gEventMgr.getEventHolder(m_event_Instanceid);
		assert(m_holder);
	}

	m_holder->addEvent(ptr);
}

void EventableObject::eventRemoveByPointer(TimedEvent * ev)
{
	EventMap::iterator itr = m_events.find(ev->eventType);
	EventMap::iterator it2;
	if(itr != m_events.end())
	{
		do 
		{
			it2 = itr++;

			if(it2->second == ev)
			{
				it2->second->decRef();
				m_events.erase(it2);
				return;
			}

		} while(itr != m_events.upper_bound(ev->eventType));
	}
}

void EventableObject::eventRemoveEvents(u32 EventType)
{
	if(!m_events.size())
	{
		return;
	}

	if(EventType == EVENT_REMOVAL_FLAG_ALL)
	{
		EventMap::iterator itr = m_events.begin();
		for(; itr != m_events.end(); ++itr)
		{
			itr->second->decRef();
		}
		m_events.clear();
	}
	else
	{
		EventMap::iterator itr = m_events.find(EventType);
		EventMap::iterator it2;
		if(itr != m_events.end())
		{
			do 
			{
				it2 = itr++;
				it2->second->decRef();
				m_events.erase(it2);

			} while(itr != m_events.upper_bound(EventType));
		}
	}

}

void EventableObject::eventRemoveEvents()
{
	eventRemoveEvents(EVENT_REMOVAL_FLAG_ALL);
}

void EventableObject::eventModifyTimeLeft(u32 EventType, u32 TimeLeft,bool unconditioned)
{
	if(!m_events.size())
	{
		return;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			if(unconditioned)
				itr->second->currTime = TimeLeft;
			else itr->second->currTime = ((s32)TimeLeft > itr->second->msTime) ? itr->second->msTime : (s32)TimeLeft;
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}
}

bool EventableObject::eventGetTimeLeft(u32 EventType, u32 * Time)
{
	if(!m_events.size())
	{
		return false;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			if( itr->second->deleted )
			{
				++itr;
				continue;
			}

			*Time = (u32)itr->second->currTime;
			return true;

		} while(itr != m_events.upper_bound(EventType));
	}

	return false;
}

void EventableObject::eventModifyTime(u32 EventType, u32 Time)
{
	if(!m_events.size())
	{
		return;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			itr->second->msTime = Time;		//修改时间间隔
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}
}

void EventableObject::eventModifyTimeAndTimeLeft(u32 EventType, u32 Time)
{
	if(!m_events.size())
	{
		return;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			itr->second->currTime = itr->second->msTime = Time;		//修改时间间隔 并且等待时间间隔开始
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

}


bool EventableObject::eventHasEvent(u32 EventType)
{
	bool ret = false;
	if(!m_events.size())
	{
		return false;
	}
	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			if(!itr->second->deleted)
			{
				ret = true;
				break;
			}
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

	return ret;
}

EventableObjectHolder::EventableObjectHolder(s32 instance_id) : mInstanceId(instance_id)
{
	gEventMgr.addEventHolder(this, instance_id);
}

EventableObjectHolder::~EventableObjectHolder()
{
	gEventMgr.removeEventHolder(this);
	EventList::iterator itr = m_events.begin();
	for(; itr != m_events.end(); ++itr)
    {	(*itr)->decRef();}
}

void EventableObjectHolder::update(u32 time_difference)
{
	EventList::iterator itr = m_events.begin();
	EventList::iterator it2;
	TimedEvent * ev;

	while(itr != m_events.end())
	{
		it2 = itr++;
        ev = *it2;
		if(ev->instanceId != mInstanceId || ev->deleted || 
			( mInstanceId == WORLD_INSTANCE && ev->eventFlag & EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT))
		{
			ev->decRef();
			m_events.erase(it2);
			continue;
		}

		if((u32)ev->currTime <= time_difference)
		{
			if (ev->cb == NULL)
			{
                ev->decRef();
                m_events.erase(it2);
                continue;
			}
			
			if(ev->eventFlag & EVENT_FLAG_DELETES_OBJECT)
			{
				ev->cb->execute();
				ev->decRef();
                m_events.erase(it2);
				continue;
			}
			else
            {	
				ev->cb->execute();
			}

			
            if((ev >= 0 &&(ev->repeats && --ev->repeats == 0) )|| ev->deleted)
			{
				ev->decRef();
				m_events.erase(it2);
				continue;
			}

			ev->currTime = ev->msTime;
		}
		else
		{ 
			ev->currTime -= time_difference;
		}
	}

}

void EventableObject::eventRelocate()
{
	EventableObjectHolder * nh = gEventMgr.getEventHolder(eventGetInstanceID());
	if(nh != m_holder)
	{
		if(!nh)
        {   
            nh = gEventMgr.getEventHolder(-1); 
            assert(nh != NULL);
        }
		nh->addObject(this);
		m_event_Instanceid = nh->getInstanceID();
		m_holder = nh;
	}
}

u32 EventableObject::eventGetEventPeriod(u32 EventType)
{
	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
    {	return (u32)itr->second->msTime;}
    else
    {   return 0; }
}

void EventableObjectHolder::addEvent(TimedEvent * ev)
{
	ev->incRef();
	m_events.push_back( ev );
}

void EventableObjectHolder::addObject(EventableObject * obj)
{
    EventMap::iterator itr = obj->m_events.begin();
    EventMap::iterator itend = obj->m_events.end();
	for(; itr != itend; ++itr)
	{
		if(itr->second->deleted)
			continue;

		if(mInstanceId == WORLD_INSTANCE && itr->second->eventFlag & EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT)
			continue;

		itr->second->incRef();
		itr->second->instanceId = mInstanceId;
		m_events.push_back( itr->second );
	}

}
