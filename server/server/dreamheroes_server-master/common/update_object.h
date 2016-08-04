#ifndef __update_object_h__
#define __update_object_h__

#include "mask_update.h"
#include "common_helper.h"

//一个更新数据的基类
//需要使用到更新的时候,需要在派生类初始化对应的数组m_UValues以及长度m_UVCount
enum U8_FILE_INDEX
{
    _FILED_UINT8_0 = 0,
    _FILED_UINT8_1,
    _FILED_UINT8_2,
    _FILED_UINT8_3,
    _FILED_UINT8_MAX,
};
enum U16_FILE_INDEX
{
    _FILED_UINT16_0 = 0,
    _FILED_UINT16_1,
    _FILED_UINT16_MAX,
};

class UpdateObject
{
protected:
	UpdateObject()
	{
		_UVCount = 0;
		_UValues = NULL;
	}
public:

	UpdateObject(u32* pkValues, u32 nValusCount) : _UpdateMask(nValusCount)
	{
		_UVCount = nValusCount;
		_UValues = pkValues;
		memset(_UValues, 0, sizeof( u32 )* nValusCount );
	}
	virtual ~UpdateObject(){;}

	void setU32Value(u32 index, u32 v)
	{
		assert( index < _UVCount);
		if (_UValues[ index ] == v)
		{
			return ;
		}
		_UValues[ index ] = v;
		_UpdateMask.setBit(index);
	}
	void setS32Value(u32 index, s32 v)
	{
		assert( index < _UVCount);
		if( *((s32*)&_UValues[index]) == v)
		{
			return ;
		}
		*((s32*)&_UValues[index]) = v;
		_UpdateMask.setBit(index);
	}
	s32 getS32Value(u32 index)
	{
		assert( index < _UVCount);
		return (s32)_UValues[index];
	}
	void setU16Value(u32 index, u16 v, U16_FILE_INDEX filed)
	{
		assert( index < _UVCount);
		u32 nLastValue = _UValues[index];
		if (filed == _FILED_UINT16_0)
		{
			memcpy( &nLastValue, &v, sizeof(u16));
		}else
		{
			memcpy( &nLastValue + sizeof(u16), &v, sizeof(u16));
		}

		if (nLastValue != _UValues[index])
		{
			_UpdateMask.setBit(index);
		}
	}
	u16 getU16Value(u32 index, U16_FILE_INDEX filed )
	{
		assert( index < _UVCount);
		u32 nValue = _UValues[index];
		if (filed == _FILED_UINT16_1)
		{
			return nValue& 0xFFFF;
		}else if (filed == _FILED_UINT16_0)
		{
			return (nValue >> 16) & 0xFFFF;
		}

		assert( false);
		return 0;
	}
	void setU8Value(u32 index, u8 v, U8_FILE_INDEX filed)
	{
		assert( index < _UVCount);
		u32 nLastValue = _UValues[index];
		u32 n32v = v;
		switch(filed)
		{
		case _FILED_UINT8_0:
			_UValues[index] = (n32v << 24) + (_UValues[index] & 0x0FFF);
			break;
		case _FILED_UINT8_1:
			_UValues[index] = (n32v << 16) + (_UValues[index] & 0xF0FF) ;
			break;
		case _FILED_UINT8_2:
			_UValues[index] = (n32v << 8) + (_UValues[index] & 0xFF0F);
			break;
		case _FILED_UINT8_3:
			_UValues[index] = n32v + (_UValues[index] & 0xFFF0);
			break;
		}

		if (nLastValue != _UValues[index])
		{
			_UpdateMask.setBit(index);
		}
		
	}
	u8 getU8Value(u32 index, U8_FILE_INDEX filed )
	{
		assert( index < _UVCount);
		u32 nValue = _UValues[index];
		switch(filed)
		{
		case _FILED_UINT8_0:
			return ((nValue & 0xF000 ) >> 24 ) & 0xFF;
			break;
		case _FILED_UINT8_1:
			return ((nValue & 0x0F00) >> 16) & 0xFF;
			break;
		case _FILED_UINT8_2:
			return ((nValue & 0x00F0) >> 8) & 0xFF;
			break;
		case _FILED_UINT8_3:
			return (nValue & 0x000F) & 0xFF;
			break;
		}

		assert( false);
		return 0x00;
	}
	void setU64Value(u32 index, u64 v)
	{
		assert( index + 1 < _UVCount);

		if (getU64Value(index) == v)
		{ return ;}

		_UValues[index] = v & 0xffffffff;
		_UValues[index+1] = (v >> 32) & 0xffffffff;

		_UpdateMask.setBit(index);
		_UpdateMask.setBit(index + 1);
	}
	u64  getU64Value(u32 index)
	{
		assert( index + 1 < _UVCount );
		return (_UValues[index] + (((u64)_UValues[index + 1]) << 32));
	}
	u32  getU32Value(u32 index)
	{
		assert( index < _UVCount);
		return _UValues[ index ];
	}
	void setU32Flag(u32 index, u32 flag)
	{
		assert( index < _UVCount);
		CheckFlag::addFlag( _UValues[index], flag);
		_UpdateMask.setBit(index);
	}
	void setU32Flags(u32 index, u32 flag)
	{
		assert( index < _UVCount);
		CheckFlag::addFlags( _UValues[index], flag);
		_UpdateMask.setBit(index);
	}
	bool hasU32Flag(u32 index, u32 flag)
	{
		assert( index < _UVCount);
		return CheckFlag::hasFlag(_UValues[index], flag);
	}
	void removeU32Flags(u32 index, u32 flag)
	{
		assert( index < _UVCount);
		CheckFlag::removeFlags(_UValues[index], flag);
		_UpdateMask.setBit(index);
	}
	void removeU32Flag(u32 index, u32 flag)
	{
		assert( index < _UVCount);
		CheckFlag::removeFlag(_UValues[index], flag);
		_UpdateMask.setBit(index);
	}
    
    u32* getUValues() { return _UValues; }
    u32 getUVCount() const { return _UVCount; }
    UpdateMask* getUpdateMask() { return &_UpdateMask; }

    void setUpdateMaskAll(){ _UpdateMask.setAll();}
protected:
	u32* _UValues;
	u32  _UVCount;
	UpdateMask _UpdateMask;
};
#endif