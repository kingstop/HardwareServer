#pragma once

class CharDatabase : public Database
{
public:
	CharDatabase();
	virtual ~CharDatabase();
public:
	void set_close_gs_id(u16 gs_id);
protected:
	u16 _close_gs_id;
};

