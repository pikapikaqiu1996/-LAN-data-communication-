// Client_FileTransferDlg.cpp : implementation file
//

#include "stdafx.h"  
#include "Client_FileTransfer.h"
#include "Client_FileTransferDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClient_FileTransferDlg dialog

CClient_FileTransferDlg::CClient_FileTransferDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClient_FileTransferDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClient_FileTransferDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClient_FileTransferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClient_FileTransferDlg)
	DDX_Control(pDX, IDC_LIST_FILELIST, m_ListFile);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClient_FileTransferDlg, CDialog)
	//{{AFX_MSG_MAP(CClient_FileTransferDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILELIST, OnDblclkListFilelist)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RECVFILELIST, OnFileList)
	ON_MESSAGE(WM_FILESTATUS, OnFileStatus)
	ON_MESSAGE(WM_CONNECTERROR, OnConnectError)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClient_FileTransferDlg message handlers

BOOL CClient_FileTransferDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	m_IPCtrl.SetAddress(127, 0, 0, 1);

	InitListColumn();

	m_pClient =new CTCPClient_FT(this);//

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClient_FileTransferDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClient_FileTransferDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CClient_FileTransferDlg::InitListColumn()
{
	int iWidth = 50;
	m_ListFile.InsertColumn(0, "File Name", LVCFMT_LEFT, 3*iWidth, -1);
	m_ListFile.InsertColumn(1, "File Size", LVCFMT_LEFT, 2*iWidth, -1);
	m_ListFile.InsertColumn(2, "Status",    LVCFMT_LEFT, 2*iWidth, -1);
	m_ListFile.InsertColumn(3, "Velocity",  LVCFMT_LEFT, 2*iWidth, -1);
	m_ListFile.InsertColumn(4, "Time",      LVCFMT_LEFT, 2*iWidth, -1);
	m_ListFile.InsertColumn(5, "Server Path", LVCFMT_LEFT, 0, -1);
	m_ListFile.InsertColumn(6, "Start Tick", LVCFMT_LEFT, 0, -1);

	m_ListFile.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
}

void CClient_FileTransferDlg::OnButtonConnect() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	BYTE i1,i2,i3,i4;
	m_IPCtrl.GetAddress(i1,i2,i3,i4);

	char sIP[15];
	sprintf(sIP, "%d.%d.%d.%d", i1,i2,i3,i4);

	m_ListFile.DeleteAllItems();

	m_pClient->SetServerIP(sIP);
	m_pClient->SetServerPort(7000);

	MSGREQUEST msgRequest;
	msgRequest.iCommand = FILELIST;
	msgRequest.lFileOffset = 0;
	msgRequest.lFileLength = 0;
	strcpy(msgRequest.sClientPath, "\0");
	strcpy(msgRequest.sServerPath, "\0");

	m_pClient->RequestFile(msgRequest);
}

void CClient_FileTransferDlg::OnDblclkListFilelist(NMHDR pNMHDR, LRESULT *pResult) 
{
	// TODO: Add your control notification handler code here
	NMLISTVIEW* pListView = (NMLISTVIEW*)pNMHDR;
	int iSel = pListView.iItem;
	
	if(iSel == -1)
		return;
	CString strFileName = m_ListFile.GetItemText(iSel, 0);
	CFileDialog dlg(FALSE, NULL, strFileName.GetBuffer(0), OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, "All Files(*.*)|*.*||", this);
	if(dlg.DoModal() != IDOK)
		return;
	CString strClientPath = dlg->GetPathName();

	CFile file;
	BOOL bOpen = file.Open(strClientPath, CFile::modeCreate|CFile::modeWrite, NULL);
	if(!bOpen)
	{
		MessageBox("文件创建或打开失败！请确认路径再重试！");
		return;
	}
	file.Close();
	MSGREQUEST msgRequest;
	msgRequest.iCommand = FILEDATA;
	msgRequest.lFileLength = m_ListFile.GetItemData(iSel);
	msgRequest.lFileOffset = 0;
	strcpy(msgRequest.sClientPath, strClientPath.GetBuffer(0));
	strcpy(msgRequest.sServerPath, m_ListFile.GetItemText(iSel, 5)); 
	
	DWORD dwStartTick = GetTickCount();
	char sStartTick[20];
	sprintf(sStartTick, "%u", dwStartTick);
	m_ListFile.SetItemText(iSel, 6, sStartTick);
	m_pClient->RequestFile(msgRequest);

	*pResult = 0;
}

void CClient_FileTransferDlg::OnButtonExit() 
{
	// TODO: Add your control notification handler code here
	OnOK();
}

LRESULT CClient_FileTransferDlg::OnFileStatus(WPARAM wParam, LPARAM lParam)
{
	MSGFILESTATUS* pmsg = (MSGFILESTATUS*)wParam;
	CString strPath;
	strPath.Format("%s", pmsg->sServerPath);
	CString strListPath;
	int iTotal = m_ListFile.GetItemCount();
	int iSel = 0;
	for(; iSel<iTotal; iSel++)
	{
		strListPath = m_ListFile.GetItemText(iSel, 5);
		if(strPath.Compare(strListPath) == 0)
			break;
	}
	if(iSel == iTotal)
		return 1;
	long lFileLength = m_ListFile.GetItemData(iSel);

	CString strStatus;
	strStatus.Format("%0.2f%%", (float)pmsg->lFileOffset/lFileLength*100);
	m_ListFile.SetItemText(iSel, 2, strStatus.GetBuffer(0));

	DWORD dwNowTick, dwStartTick;
	dwNowTick = GetTickCount();
	sscanf(m_ListFile.GetItemText(iSel, 6), "%u", &dwStartTick);

	int iEllapsed = (dwNowTick-dwStartTick)/1000;
	strStatus.Format("%ds", iEllapsed);
	m_ListFile.SetItemText(iSel, 4, strStatus.GetBuffer(0));

	if(iEllapsed > 0)
	{
		strStatus.Format("%dk/s", pmsg->lFileOffset/1024/iEllapsed);
		m_ListFile.SetItemText(iSel, 3, strStatus.GetBuffer(0));
	}


	return 0;
}

LRESULT CClient_FileTransferDlg::OnConnectError(WPARAM wParam, LPARAM lParam)
{
	MessageBox("连接服务器失败!");
	return 0;
}

LRESULT CClient_FileTransferDlg::OnFileList(WPARAM wParam, LPARAM lParam)
{
	MSGFILELIST* pmsgList = (MSGFILELIST*)wParam;
	int iItem = m_ListFile.GetItemCount();
	CString strServerPath, strFileName, strFileSize;
	strServerPath.Format("%s", pmsgList->sServerPath);
	int iPos = strServerPath.ReverseFind('\\');
	strFileName = strServerPath.Right(strServerPath.GetLength()-iPos-1);
	strFileSize.Format("%0.2fk", (float)pmsgList->lFileLength/1024);
	LV_ITEM lvi;
	lvi.mask = LVIF_TEXT|LVIF_PARAM;
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.lParam = pmsgList->lFileLength;
	lvi.pszText = strFileName.GetBuffer(0);
	m_ListFile.InsertItem(&lvi);
	m_ListFile.SetItemText(iItem, 1, strFileSize.GetBuffer(0));
	m_ListFile.SetItemText(iItem, 2, "0.00%");
	m_ListFile.SetItemText(iItem, 3, "0k/s");
	m_ListFile.SetItemText(iItem, 4, "0s");
	m_ListFile.SetItemText(iItem, 5, strServerPath.GetBuffer(0));
	m_ListFile.SetItemText(iItem, 6, "0");
	return 0;
}

void CClient_FileTransferDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_pClient != NULL)
	{
		m_pClient->StopAllRecvThread();
		delete m_pClient;
		m_pClient = NULL;
	}
}
