// Client_FileTransfer.h : main header file for the CLIENT_FILETRANSFER application
//

#if !defined(AFX_CLIENT_FILETRANSFER_H__746DD7D8_DD8F_49A5_BBD9_07506B7DAE83__INCLUDED_)
#define AFX_CLIENT_FILETRANSFER_H__746DD7D8_DD8F_49A5_BBD9_07506B7DAE83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CClient_FileTransferApp:
// See Client_FileTransfer.cpp for the implementation of this class
//

class CClient_FileTransferApp : public CWinApp
{
public:
	CClient_FileTransferApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClient_FileTransferApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CClient_FileTransferApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENT_FILETRANSFER_H__746DD7D8_DD8F_49A5_BBD9_07506B7DAE83__INCLUDED_)
