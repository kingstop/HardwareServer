
// virtualclient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CvirtualclientApp: 
// �йش����ʵ�֣������ virtualclient.cpp
//

class CvirtualclientApp : public CWinApp
{
public:
	CvirtualclientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CvirtualclientApp theApp;