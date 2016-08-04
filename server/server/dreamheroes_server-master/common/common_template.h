#ifndef __common_template_h__
#define __common_template_h__
#include <map>
#include <stdio.h>

template< typename T >
class singletion
{
public:
    virtual ~singletion() {}

    static T& instance()
    {
        static T s_Instance;
        return s_Instance;
    }
protected:
    singletion() {}

private:
    singletion( const singletion& );
    singletion& operator = ( const singletion& );
};


template<class KEY, class T>
class obj_ptr_map
{
public:
    T* getData(KEY id)
    {
        typename std::map<KEY, T*>::iterator it = m_DataMap.find(id);
        if (it != m_DataMap.end())
        {
            return it->second;
        }

        return NULL ; 
    }

    size_t getSize() const {return m_DataMap.size() ;}

    bool addData(KEY id, T* data)
    {
        typename std::map<KEY, T*>::iterator it = m_DataMap.find(id);
        if (it == m_DataMap.end())
        {
            m_DataMap[id] = data ;
            return true ;
        }
        return false ;
    }

    T* eraseData(KEY id)
    {
        typename std::map<KEY, T*>::iterator it = m_DataMap.find(id);
        if (it != m_DataMap.end())
        {
            T*  p = it->second; 
            m_DataMap.erase(it);
            return p;
        }
        return NULL;
    }

    const std::map<KEY, T*>& getDataMap()const 
    {
        return m_DataMap;
    }
protected:
    std::map<KEY, T*> m_DataMap ; 
};


template<class KEY, class T>
class obj_map
{
public:
    bool getData(KEY id, T& data)
    {
        typename std::map<KEY, T>::iterator it = m_DataMap.find(id);
        if (it != m_DataMap.end())
        {   
            data = (it->second);
            return true;
        }

        return false; 
    }

    bool hasData(KEY id)
    {
        return (m_DataMap.find(id) != m_DataMap.end());
    }
    bool addData(KEY id, T data)
    {
        typename std::map<KEY, T>::iterator it = m_DataMap.find(id);
        if (it == m_DataMap.end())
        {
            m_DataMap[id] = data ;
            return true ;
        }
        return false ;
    }

    size_t getSize() const {return m_DataMap.size() ;}

    bool eraseData(KEY id, T& v)
    {
        typename std::map<KEY, T>::iterator it = m_DataMap.find(id);
        if (it != m_DataMap.end())
        {
            v = it->second;
            m_DataMap.erase(it);
            return true;
        }
        return false;
    }

	bool getKey(KEY& k, T t)
	{
		typename std::map<KEY, T>::iterator it = m_DataMap.begin();
		typename std::map<KEY, T>::iterator itend = m_DataMap.end();
		for(it; it != itend;)
		{
			T& a = it->second;
			if (a == t)
			{   
				k = it->first;
				return true;
			}
			else
			{    ++it;}
		}
		return false;
	}
    void eraseValue(T v)
    {
        typename std::map<KEY, T>::iterator it = m_DataMap.begin();
        typename std::map<KEY, T>::iterator itend = m_DataMap.end();
        
        for(it; it != itend;)
        {
#ifdef WIN32
			T& a = it->second;
			if (a == v)
			{
				it = m_DataMap.erase(it);
			}
			else
			{
				++it;
			}
#else

			T& a = it->second;
			if (a == v)
			{
				m_DataMap.erase(it);
			}

			++it;

#endif // WIN32

                         
        }
       
    }

    bool eraseData(KEY id)
    {
        typename std::map<KEY, T>::iterator it = m_DataMap.find(id);
        if (it != m_DataMap.end())
        {
            m_DataMap.erase(it);
            return true;
        }
        return false;
    }

    const std::map<KEY, T>& getDataMap()const 
    {   return m_DataMap;   }
protected:
    std::map<KEY, T> m_DataMap ; 
};
#endif

