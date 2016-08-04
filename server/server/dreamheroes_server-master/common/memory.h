#ifndef __memory_h__
#define __memory_h__
#pragma warning(disable:4291)
#include "object_pool.h"

#define REGISTER_POOL_INFO( STR, NEXT, MAXS ) public: \
	static const char* getStaticClassName(){return #STR;}			\
	static const unsigned int getStaticNextSize(){ return NEXT;}	\
	static const unsigned int getStaticMaxSize(){return MAXS;}

#define GET_POOL_CLASS_NAME(CLASS) CLASS::getStaticClassName()
#define GET_POOL_NEXT_SIZE(CLASS) CLASS::getStaticNextSize()
#define GET_POOL_MAX_SIZE(CLASS) CLASS::getStaticMaxSize()

#define MEFREE( CLASS, POINTE) Memory::releaseObject<CLASS>(POINTE)
#define MENEW( CLASS) Memory::createObject<CLASS>()
#define MEMORY_LOG_INFO(CLASS) Memory::flushObjectMemory<CLASS>()

class Memory
{
public:
	template< class Class >
	static Class* createObject();

	template< class Class, typename P0 >
	static Class* createObject(P0 a0);

	template< class Class, typename P0 , typename P1>
	static Class* createObject(P0 a0, P1 a1);

	template< class Class, typename P0 , typename P1, typename P2>
	static Class* createObject(P0 a0, P1 a1, P2 a2);

	template< class Class, typename P0 , typename P1, typename P2, typename P3>
	static Class* createObject(P0 a0, P1 a1, P2 a2, P3 a3);

	template< class Class, typename P0 , typename P1, typename P2, typename P3, typename P4>
	static Class* createObject(P0 a0, P1 a1, P2 a2, P3 a3, P4 a4);

	template< class Class, typename P0 , typename P1, typename P2, typename P3, typename P4, typename P5>
	static Class* createObject( P0 a0, P1 a1, P2 a2, P3 a3, P4 a4, P5 a5);

    template< class Class, typename P0 , typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    static Class* createObject( P0 a0, P1 a1, P2 a2, P3 a3, P4 a4, P5 a5, P6 a6);

	template< class Class>
	static void releaseObject( Class* pkPointer);

	template< class Class>
	static ObjectPool<Class>* getObjectPool();

	template< class Class>
	static void flushObjectMemory();
};
template< class Class >
Class* Memory::createObject()
{
	ENTER_FUN
	ObjectPool<Class>* pkPool = getObjectPool<Class>();
	if (pkPool)
	{
		Class* pkPointer = pkPool->newObject();
		if (NULL == pkPointer)
		{ return NULL;}
		return new(pkPointer) Class();
	}else
	{	
		THROW_EXCEPTION(MemoryException(GET_POOL_CLASS_NAME(Class)));
	}
	LEAVE_FUN
	return ::new Class();
}

template< class Class, typename P0 >
Class* Memory::createObject( P0 a0)
{
	ENTER_FUN
	ObjectPool<Class>* pkPool = getObjectPool<Class>();
	if (pkPool)
	{
		Class* pkPointer = pkPool->newObject();
		if (NULL == pkPointer)
		{ return NULL;}
		return new(pkPointer) Class(a0);
	}else
	{
		THROW_EXCEPTION(MemoryException(GET_POOL_CLASS_NAME(Class)));
	}
	LEAVE_FUN
	return ::new Class(a0);
}

template< class Class, typename P0 , typename P1>
Class* Memory::createObject(P0 a0, P1 a1)
{
	ENTER_FUN
	ObjectPool<Class>* pkPool = getObjectPool<Class>();
	if (pkPool)
	{
		Class* pkPointer = pkPool->newObject();
		if (NULL == pkPointer)
		{ return NULL;}
		return new(pkPointer) Class(a0,a1);
	}else
	{
		THROW_EXCEPTION(MemoryException(GET_POOL_CLASS_NAME(Class)));
	}
	LEAVE_FUN
	return ::new Class(a0,a1);
}

template< class Class, typename P0 , typename P1, typename P2>
Class* Memory::createObject( P0 a0, P1 a1, P2 a2)
{
	ENTER_FUN
	ObjectPool<Class>* pkPool = getObjectPool<Class>();
	if (pkPool)
	{
		Class* pkPointer = pkPool->newObject();
		if (NULL == pkPointer)
		{ return NULL;}
		return new(pkPointer) Class(a0,a1,a2);
	}else
	{
		THROW_EXCEPTION(MemoryException(GET_POOL_CLASS_NAME(Class)));
	}
	LEAVE_FUN
	return ::new Class(a0,a1,a2);
}

template< class Class, typename P0 , typename P1, typename P2, typename P3>
Class* Memory::createObject( P0 a0, P1 a1, P2 a2, P3 a3)
{
	ENTER_FUN
	ObjectPool<Class>* pkPool = getObjectPool<Class>();
	if (pkPool)
	{
		Class* pkPointer = pkPool->newObject();
		if (NULL == pkPointer)
		{ return NULL;}
		return new(pkPointer) Class(a0,a1,a2,a3);
	}else
	{
		THROW_EXCEPTION(MemoryException(GET_POOL_CLASS_NAME(Class)));
	}
	LEAVE_FUN
	return ::new Class(a0,a1,a2,a3);
}

template< class Class, typename P0 , typename P1, typename P2, typename P3, typename P4>
Class* Memory::createObject( P0 a0, P1 a1, P2 a2, P3 a3, P4 a4)
{
	ENTER_FUN
	ObjectPool<Class>* pkPool = getObjectPool<Class>();
	if (pkPool)
	{
		Class* pkPointer = pkPool->newObject();
		if (NULL == pkPointer)
		{ return NULL;}
		return new(pkPointer) Class(a0,a1,a2,a3,a4);
	}else
	{
		THROW_EXCEPTION(MemoryException(GET_POOL_CLASS_NAME(Class)));
	}
	LEAVE_FUN
	return ::new Class(a0,a1,a2,a3,a4);
}

template< class Class, typename P0 , typename P1, typename P2, typename P3, typename P4, typename P5>
Class* Memory::createObject( P0 a0, P1 a1, P2 a2, P3 a3, P4 a4, P5 a5)
{
	ENTER_FUN
	ObjectPool<Class>* pkPool = getObjectPool<Class>();
	if (pkPool)
	{
		Class* pkPointer = pkPool->newObject();
		if (NULL == pkPointer)
		{ return NULL;}
		return new(pkPointer) Class(a0,a1,a2,a3,a4,a5);
	}else
	{
		THROW_EXCEPTION(MemoryException(GET_POOL_CLASS_NAME(Class)));
	}
	LEAVE_FUN
	return ::new Class(a0,a1,a2,a3,a4,a5);
}
template< class Class, typename P0 , typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
Class* Memory::createObject( P0 a0, P1 a1, P2 a2, P3 a3, P4 a4, P5 a5, P6 a6)
{
    ENTER_FUN
    ObjectPool<Class>* pkPool = getObjectPool<Class>();
    if (pkPool)
    {
        Class* pkPointer = pkPool->newObject();
        if (NULL == pkPointer)
        { return NULL;}
        return new(pkPointer) Class(a0,a1,a2,a3,a4,a5, a6);
    }else
    {
        THROW_EXCEPTION(MemoryException(GET_POOL_CLASS_NAME(Class)));
    }
    LEAVE_FUN
    return ::new Class(a0,a1,a2,a3,a4,a5, a6);
}

template< class Class>
void Memory::releaseObject( Class* pkPointer)
{
	ENTER_FUN
	ObjectPool<Class>* pkPool = getObjectPool<Class>();
	if (pkPool)
	{
		pkPool->deleteObject(pkPointer);
		return;
	}else
	{	
		THROW_EXCEPTION(MemoryException(GET_POOL_CLASS_NAME(Class)));
	}
	LEAVE_FUN
	::delete pkPointer;
}
template< class Class>
ObjectPool<Class>* Memory::getObjectPool()
{
	static ObjectPool<Class>* pool = NULL;
	if (NULL == pool)
	{
		assert( GET_POOL_NEXT_SIZE(Class) > 0);
		if ( GET_POOL_NEXT_SIZE(Class) <= 0 )
		{ return NULL;}

		pool = new ObjectPool<Class>(GET_POOL_NEXT_SIZE(Class), GET_POOL_MAX_SIZE(Class));
	}
	return pool;
}

template< class Class>
void Memory::flushObjectMemory()
{
	ObjectPool<Class>* pkPool = getObjectPool<Class>();
	if (pkPool)
	{
		pkPool->log(GET_POOL_CLASS_NAME(Class), GET_POOL_NEXT_SIZE(Class), GET_POOL_MAX_SIZE(Class));
	}else
	{
		Mylog::log_memory(LOG_ERROR, "Memory: not find class:[%s] pool info", GET_POOL_CLASS_NAME(Class));
	}
}
class BaseMemoryObject
{
	friend class Memory;
	REGISTER_POOL_INFO(BaseMemoryObject, 0,0)
protected:
	void* operator new(size_t count, void* pkbuff)
	{
		return pkbuff ;
	}
	void operator delete(void* pkbuff,size_t count)
	{
		assert(false);
		//::delete pkbuff;
	}
};



#define PUBLIC_BASE_OBJECT(CLASS) public BaseMemoryObject
#pragma warning(default:4291)
#endif /*__memory_h__*/