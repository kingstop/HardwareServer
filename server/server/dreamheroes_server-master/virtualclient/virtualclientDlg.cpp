
// virtualclientDlg.cpp : 实现文件
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

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CvirtualclientDlg 对话框



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


// CvirtualclientDlg 消息处理程序

BOOL CvirtualclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_MilliSecond = 10;
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标
	g_app = this;
	UpdateData(TRUE);

	_Thread = AfxBeginThread(ThreadFunc, 0);    //创建MFC线程
	SetDlgItemInt(IDC_EDT_COUNT_BEGIN, 0);
	SetDlgItemInt(IDC_EDT_COUNT_END, 100);
	// TODO: 在此添加额外的初始化代码

	Mylog::log_init(LOG4CXX_LOG_CONFIG);
	service_config conf;
	if (!ServerFrame::loadServiceConfig(conf, SERVER_CONFIG))
	{
		return false;
	}
	net_global::init_net_service(conf.thread_count, conf.proc_interval, NULL, conf.speed_, conf.msg_pool_size);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CvirtualclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	// TODO: 在此添加控件通知处理程序代码
}


void CvirtualclientDlg::OnBnClickedBtnLogin()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CvirtualclientDlg::OnBnClickedBtnStopLogin()
{
	// TODO: 在此添加控件通知处理程序代码
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

