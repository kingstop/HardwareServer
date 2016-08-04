#ifndef __run_object_h__
#define __run_object_h__

#include "common_type.h"
struct RunObject
{
    enum
    {
        _init_obj_,		//׼���׶�
        _run_obj_,		//���н׶�
        _wait_stop_,	//׼��ֹͣ�׶�
        _stop_obj_,		//ֹͣ
    };

    RunObject()
    {
        _obj_state = _init_obj_;
    }

    void setObjState( u8 state)
    {
        _obj_state = state;
    }
    virtual void checkStop() = 0;
    bool isObjStop()
    {
        return _obj_state == _stop_obj_;
    }
    bool isObjState( u8 state)
    {
        return _obj_state == state;
    }
    u8 _obj_state;
};

#endif