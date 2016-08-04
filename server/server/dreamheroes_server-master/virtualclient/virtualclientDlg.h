
// virtualclientDlg.h : 头文件
//

#pragma once
#include "afxwin.h"

// CvirtualclientDlg 对话框
class CvirtualclientDlg : public CDialogEx
{
// 构造
public:
	CvirtualclientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIRTUALCLIENT_DIALOG };
#endif
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
// 实现
protected:
	HICON m_hIcon;
	int m_MilliSecond;
	CWinThread* _Thread;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	int getAccountBegin();
	int getAccountEnd();
	void ClientListModify(const char* sztemp, enClientType en, bool add);
public:
	CListBox _list_online;
	CListBox _list_offline;
	afx_msg void OnBnClickedBtnGenerateAccount();
	afx_msg void OnBnClickedBtnLogin();
	afx_msg void OnBnClickedBtnStopLogin();
};
extern CvirtualclientDlg* g_app;