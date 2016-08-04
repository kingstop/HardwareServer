
// virtualclientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

// CvirtualclientDlg �Ի���
class CvirtualclientDlg : public CDialogEx
{
// ����
public:
	CvirtualclientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIRTUALCLIENT_DIALOG };
#endif
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
// ʵ��
protected:
	HICON m_hIcon;
	int m_MilliSecond;
	CWinThread* _Thread;
	// ���ɵ���Ϣӳ�亯��
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