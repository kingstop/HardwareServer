#ifndef _call_back_def_h__
#define _call_back_def_h__

struct CallbackBase
{
	virtual void execute(){;}
};

template <class Class>
struct CallbackP0 : public CallbackBase
{
	typedef void (Class::*method)()  ;

	CallbackP0(Class *obj, method f )
	{
		self = obj;
		fun = f;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)() ;}
	}

	Class * self;
	method fun;
};

template <class Class, typename P1>
struct CallbackP1 : public CallbackBase
{
	typedef void (Class::*method)(P1 a)  ;

	CallbackP1(Class *obj, method f , P1 a)
	{
		self = obj;
		fun = f;
		a1 = a;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)(a1) ;}
	}

	Class * self;
	method fun;
	P1 a1;
};
template <class Class, typename P1, typename P2>
struct CallbackP2 : public CallbackBase
{
	typedef void (Class::*method)(P1 a, P2 b)  ;

	CallbackP2(Class *obj, method f , P1 a, P2 b)
	{
		self = obj;
		fun = f;
		a1 = a;
		b2 = b;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)(a1, b2) ;}
	}

	Class * self;
	method fun;
	P1 a1;
	P2 b2;
};

template <class Class, typename P1, typename P2, typename P3>
struct CallbackP3 : public CallbackBase
{
	typedef void (Class::*method)(P1 a, P2 b, P3 c)  ;

	CallbackP3(Class *obj, method f , P1 a, P2 b, P3 c)
	{
		self = obj;
		fun = f;
		a1 = a;
		b2 = b;
		c3 = c;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)(a1, b2, c3) ;}
	}

	Class * self;
	method fun;
	P1 a1;
	P2 b2;
	P3 c3;
};
template <class Class, typename P1, typename P2, typename P3, typename P4>
struct CallbackP4 : public CallbackBase
{
	typedef void (Class::*method)(P1 a, P2 b, P3 c, P4 d)  ;

	CallbackP4(Class *obj, method me , P1 a, P2 b, P3 c, P4 d)
	{
		self = obj;
		fun = me;
		a1 = a;
		b2 = b;
		c3 = c;
		d4 = d;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)(a1, b2, c3, d4) ;}
	}

	Class * self;
	method fun;
	P1 a1;
	P2 b2;
	P3 c3;
	P4 d4;
};
template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5>
struct CallbackP5 : public CallbackBase
{
	typedef void (Class::*method)(P1 a, P2 b, P3 c, P4 d, P5 e)  ;

	CallbackP5(Class *obj, method me , P1 a, P2 b, P3 c, P4 d, P5 e)
	{
		self = obj;
		fun = me;
		a1 = a;
		b2 = b;
		c3 = c;
		d4 = d;
		e5 = e;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)(a1, b2, c3, d4, e5) ;}
	}

	Class * self;
	method fun;
	P1 a1;
	P2 b2;
	P3 c3;
	P4 d4;
	P5 e5;
};

template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
struct CallbackP6 : public CallbackBase
{
	typedef void (Class::*method)(P1 a, P2 b, P3 c, P4 d, P5 e, P6 f )  ;

	CallbackP6(Class *obj, method me , P1 a, P2 b, P3 c, P4 d, P5 e, P6 f)
	{
		self = obj;
		fun = me;
		a1 = a;
		b2 = b;
		c3 = c;
		d4 = d;
		e5 = e;
		f6 =f;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)(a1, b2, c3, d4, e5, f6) ;}
	}

	Class * self;
	method fun;
	P1 a1;
	P2 b2;
	P3 c3;
	P4 d4;
	P5 e5;
	P6 f6;
};

template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
struct CallbackP7 : public CallbackBase
{
	typedef void (Class::*method)(P1 a, P2 b, P3 c, P4 d, P5 e, P6 f, P7 g )  ;

	CallbackP7(Class *obj, method me , P1 a, P2 b, P3 c, P4 d, P5 e, P6 f, P7 g )
	{
		self = obj;
		fun = me;
		a1 = a;
		b2 = b;
		c3 = c;
		d4 = d;
		e5 = e;
		f6 = f;
		g7 = g;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)(a1, b2, c3, d4, e5, f6, g7) ;}
	}

	Class * self;
	method fun;
	P1 a1;
	P2 b2;
	P3 c3;
	P4 d4;
	P5 e5;
	P6 f6;
	P7 g7;
};

template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
struct CallbackP8 : public CallbackBase
{
	typedef void (Class::*method)(P1 a, P2 b, P3 c, P4 d, P5 e, P6 f, P7 g, P8 h )  ;

	CallbackP8(Class *obj, method me , P1 a, P2 b, P3 c, P4 d, P5 e, P6 f, P7 g, P8 h )
	{
		self = obj;
		fun = me;
		a1 = a;
		b2 = b;
		c3 = c;
		d4 = d;
		e5 = e;
		f6 =f;
		g7 = g;
		h8 = h;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)(a1, b2, c3, d4, e5, f6, g7, h8) ;}
	}

	Class * self;
	method fun;
	P1 a1;
	P2 b2;
	P3 c3;
	P4 d4;
	P5 e5;
	P6 f6;
	P7 g7;
	P8 h8;
};

template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
struct CallbackP9 : public CallbackBase
{
	typedef void (Class::*method)(P1 a, P2 b, P3 c, P4 d, P5 e, P6 f, P7 g, P8 h ,P9 i)  ;

	CallbackP9(Class *obj, method me , P1 a, P2 b, P3 c, P4 d, P5 e, P6 f, P7 g, P8 h ,P9 i)
	{
		self = obj;
		fun = me;
		a1 = a;
		b2 = b;
		c3 = c;
		d4 = d;
		e5 = e;
		f6 =f;
		g7 = g;
		h8 = h;
		i9 = i;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)(a1, b2, c3, d4, e5, f6, g7, h8, i9) ;}
	}

	Class * self;
	method fun;
	P1 a1;
	P2 b2;
	P3 c3;
	P4 d4;
	P5 e5;
	P6 f6;
	P7 g7;
	P8 h8;
	P9 i9;
};

template <class Class, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
struct CallbackP10 : public CallbackBase
{
	typedef void (Class::*method)(P1 a, P2 b, P3 c, P4 d, P5 e, P6 f, P7 g, P8 h ,P9 i, P10 j)  ;

	CallbackP10(Class *obj, method me , P1 a, P2 b, P3 c, P4 d, P5 e, P6 f, P7 g, P8 h ,P9 i, P10 j)
	{
		self = obj;
		fun = me;
		a1 = a;
		b2 = b;
		c3 = c;
		d4 = d;
		e5 = e;
		f6 =f;
		g7 = g;
		h8 = h;
		i9 = i;
		j10 = 10;
	}

	virtual void execute()
	{
		if (self && fun)
		{ (self->*fun)(a1, b2, c3, d4, e5, f6, g7, h8, i9, j10) ;}
	}

	Class * self;
	method fun;
	P1 a1;
	P2 b2;
	P3 c3;
	P4 d4;
	P5 e5;
	P6 f6;
	P7 g7;
	P8 h8;
	P9 i9;
	P10 j10;

};

#endif