// Client_FileTransferDlg.h : header file
//

#if !defined(AFX_CLIENT_FILETRANSFERDLG_H__4AE600AE_EF7C_4472_BD39_2E7A3D233B9C__INCLUDED_)
#define AFX_CLIENT_FILETRANSFERDLG_H__4AE600AE_EF7C_4472_BD39_2E7A3D233B9C__INCLUDED_

#include "TCPClient_FT.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CClient_FileTransferDlg dialog
#include "TCPClient_FT.h"

class CClient_FileTransferDlg : public CDialog
{
// Construction
public:
	long m_lLastOffset;
	DWORD m_dwLastTick;
	DWORD m_dwStartTick;
	CTCPClient_FT* m_pClient;
	void InitListColumn(void);
	CClient_FileTransferDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CClient_FileTransferDlg)
	enum { IDD = IDD_CLIENT_FILETRANSFER_DIALOG };
	CListCtrl	m_ListFile;
	CIPAddressCtrl	m_IPCtrl;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClient_FileTransferDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CClient_FileTransferDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonConnect();
	afx_msg void OnDblclkListFilelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonExit();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg LRESULT OnFileList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFileStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConnectError(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENT_FILETRANSFERDLG_H__4AE600AE_EF7C_4472_BD39_2E7A3D233B9C__INCLUDED_)
