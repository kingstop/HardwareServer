
// virtualclientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "virtualclient.h"
#include "virtualclientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CvirtualclientDlg* g_app = NULL;

UINT ThreadFunc(LPVOID lpParam)
{
	boost::posix_time::millisec_posix_time_system_config::time_duration_type time_elapse;
	boost::posix_time::ptime nLastTime = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::ptime nStartTime = nLastTime;
	while (!Instance.client_manage_.isStop())
	{
		nStartTime = boost::posix_time::microsec_clock::universal_time();
		time_elapse = nStartTime - nLastTime;
		nLastTime = nStartTime;
		Instance.client_manage_.runOnce((u32)(time_elapse.total_milliseconds()));
		Sleep(3);
	}
	return 0;
}

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CvirtualclientDlg �Ի���



CvirtualclientDlg::CvirtualclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_VIRTUALCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CvirtualclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ONLINE, _list_online);
	DDX_Control(pDX, IDC_LIST_OFFLINE, _list_offline);
}

BEGIN_MESSAGE_MAP(CvirtualclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_BTN_GENERATE_ACCOUNT, &CvirtualclientDlg::OnBnClickedBtnGenerateAccount)
	ON_BN_CLICKED(ID_BTN_LOGIN, &CvirtualclientDlg::OnBnClickedBtnLogin)
	ON_BN_CLICKED(ID_BTN_STOP_LOGIN, &CvirtualclientDlg::OnBnClickedBtnStopLogin)
END_MESSAGE_MAP()


// CvirtualclientDlg ��Ϣ�������

BOOL CvirtualclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_MilliSecond = 10;
	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��
	g_app = this;
	UpdateData(TRUE);

	_Thread = AfxBeginThread(ThreadFunc, 0);    //����MFC�߳�
	SetDlgItemInt(IDC_EDT_COUNT_BEGIN, 0);
	SetDlgItemInt(IDC_EDT_COUNT_END, 100);
	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	Mylog::log_init(LOG4CXX_LOG_CONFIG);
	service_config conf;
	if (!ServerFrame::loadServiceConfig(conf, SERVER_CONFIG))
	{
		return false;
	}
	net_global::init_net_service(conf.thread_count, conf.proc_interval, NULL, conf.speed_, conf.msg_pool_size);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CvirtualclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CvirtualclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CvirtualclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CvirtualclientDlg::ClientListModify(const char* sztemp, enClientType en, bool add)
{
	CListBox* list_box = NULL;

	std::string temp = sztemp;
	switch (en)
	{
	case  offline_client:
		{
			list_box = &_list_offline;
		}
		break;
	case  online_client:
		{
			list_box = &_list_online;
		}
		break;
	}
	if (add)
	{
		list_box->AddString(sztemp);
	}
	else
	{
		CString temp;
		int count_temp = list_box->GetCount();
		for (int i = 0; i < count_temp; i ++)
		{
			list_box->GetText(i, temp);
			if (temp == sztemp)
			{
				list_box->DeleteString(i);
				break;
			}
		}		
	}
	SetDlgItemInt(IDC_EDT_ONLINE_NUMBER, _list_online.GetCount());
	SetDlgItemInt(IDC_EDT_OFFLINE_COUNT, _list_offline.GetCount());
}

void CvirtualclientDlg::OnBnClickedBtnGenerateAccount()
{
	Instance.config_.GenerateAccount();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CvirtualclientDlg::OnBnClickedBtnLogin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CvirtualclientDlg::OnBnClickedBtnStopLogin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

int CvirtualclientDlg::getAccountBegin()
{
	int number = GetDlgItemInt(IDC_EDT_COUNT_BEGIN);
	return number;

}

int CvirtualclientDlg::getAccountEnd()
{
	int number = GetDlgItemInt(IDC_EDT_COUNT_END);
	return number;
}

