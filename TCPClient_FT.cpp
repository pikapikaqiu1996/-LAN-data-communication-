// TCPClient_FT.cpp: implementation of the CTCPClient_FT class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TCPClient_FT.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL CTCPClient_FT::m_bEndRecvThread = FALSE;

CTCPClient_FT::CTCPClient_FT()
{
	
}

CTCPClient_FT::~CTCPClient_FT()
{

}

//重载构造函数，添加指向窗口的指针
CTCPClient_FT::CTCPClient_FT(LPVOID ptr)
{
	strcpy(m_sIP, "127.0.0.1");
	m_iServerPort = 7000;
	m_pWnd = (CWnd*)ptr;
}

//功能：设置服务器地址
//参数：字符串形式的IP地址指针
//返回：TRUE-成功，FALSE-失败
BOOL CTCPClient_FT::SetServerIP(char* sIP)
{
	if(sIP == NULL)
		return FALSE;
	else
	{
		strcpy(m_sIP, sIP);
		return TRUE;
	}
}

//功能：设置服务器监听端口
//参数：端口变量
//返回：无
void CTCPClient_FT::SetServerPort(int iPort)
{
	m_iServerPort = iPort;
}


//功能：创建请求线程
//参数：请求结构体变量
//返回：无
void CTCPClient_FT::RequestFile(MSGREQUEST msgRequest)
{
	PARAMREQUEST* pParam = new PARAMREQUEST;
	strcpy(pParam->sIP, m_sIP);
	pParam->iPort = m_iServerPort;
	pParam->msgRequest.iCommand = msgRequest.iCommand ;
	pParam->msgRequest.lFileLength = msgRequest.lFileLength;
	pParam->msgRequest.lFileOffset = msgRequest.lFileOffset;
	strcpy(pParam->msgRequest.sClientPath, msgRequest.sClientPath);
	strcpy(pParam->msgRequest.sServerPath, msgRequest.sServerPath);
	pParam->ptr = m_pWnd;

	DWORD id;
	HANDLE h = CreateThread(NULL, 0, ThreadRequestFile, pParam, 0, &id);
	CloseHandle(h);
}


//功能：发送请求信息并接收反馈信息
//参数：LPVOID lpParam
//返回：暂无意义
DWORD WINAPI CTCPClient_FT::ThreadRequestFile(LPVOID lpParam)
{
	PARAMREQUEST* pParam = (PARAMREQUEST*)lpParam;
	CWnd* pWnd = (CWnd*)pParam->ptr;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(pParam->sIP);
	sin.sin_port = htons(pParam->iPort);

	if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR)
	{
		//发送请求
		int iSendCnt = send(sock, (char*)&pParam->msgRequest, sizeof(MSGREQUEST), 0);

		int iRecvCnt = -1;
		if(pParam->msgRequest.iCommand == FILELIST)		//如果是请求文件列表
		{
			while(!CTCPClient_FT::m_bEndRecvThread)
			{
				MSGFILELIST msgFileList;
				iRecvCnt = recv(sock, (char*)&msgFileList, sizeof(msgFileList), 0);
				if(iRecvCnt <= 0)
					break;
				
				pWnd->SendMessage(WM_RECVFILELIST, (WPARAM)&msgFileList, 0);
			}
		}
		
		else if(pParam->msgRequest.iCommand == FILEDATA)	//如果是请求文件数据
		{
			char sRecvBuf[RECVSIZE] = "\0";
			long lFileOffset = pParam->msgRequest.lFileOffset;
			CFile file;
			BOOL bResult = file.Open(pParam->msgRequest.sClientPath, CFile::modeWrite|CFile::modeNoTruncate, NULL);
			while(!CTCPClient_FT::m_bEndRecvThread && bResult && lFileOffset < pParam->msgRequest.lFileLength)
			{
				int iRecvCnt = recv(sock, sRecvBuf, RECVSIZE, 0);
				if(iRecvCnt <= 0)
					break;
				
				file.Seek(lFileOffset, CFile::begin);
				file.Write(sRecvBuf, iRecvCnt);
				lFileOffset += iRecvCnt;

				MSGFILESTATUS msgFileStatus;
				msgFileStatus.lFileOffset = lFileOffset;
				strcpy(msgFileStatus.sServerPath, pParam->msgRequest.sServerPath);
				pWnd->SendMessage(WM_FILESTATUS, (WPARAM)&msgFileStatus, 0);
			}
			file.Close();

			if(lFileOffset < pParam->msgRequest.lFileLength && !CTCPClient_FT::m_bEndRecvThread)		//如果没有下载完,请求续传
			{
				PARAMREQUEST* pParamResume = new PARAMREQUEST;
				strcpy(pParamResume->sIP, pParam->sIP);
				pParamResume->iPort = pParam->iPort;
				pParamResume->ptr = pParam->ptr;
				pParamResume->msgRequest.iCommand = pParam->msgRequest.iCommand ;
				pParamResume->msgRequest.lFileLength = pParam->msgRequest.lFileLength;
				pParamResume->msgRequest.lFileOffset = lFileOffset;
				strcpy(pParamResume->msgRequest.sClientPath, pParam->msgRequest.sClientPath);
				strcpy(pParamResume->msgRequest.sServerPath, pParam->msgRequest.sServerPath);

				DWORD id;
				HANDLE h = CreateThread(NULL, 0, ThreadRequestFile, pParamResume, 0, &id);
				CloseHandle(h);
			}
		}
				
	}//end of if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR)

	else
	{
		pWnd->SendMessage(WM_CONNECTERROR);
	}

	closesocket(sock);
	delete pParam;
	return 0;
}

//功能：停止所有接收线程（用户强制结束）
//参数：无
//返回：无
void CTCPClient_FT::StopAllRecvThread()
{
	m_bEndRecvThread = TRUE;
}