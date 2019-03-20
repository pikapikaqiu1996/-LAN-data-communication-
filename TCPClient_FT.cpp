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

//���ع��캯�������ָ�򴰿ڵ�ָ��
CTCPClient_FT::CTCPClient_FT(LPVOID ptr)
{
	strcpy(m_sIP, "127.0.0.1");
	m_iServerPort = 7000;
	m_pWnd = (CWnd*)ptr;
}

//���ܣ����÷�������ַ
//�������ַ�����ʽ��IP��ַָ��
//���أ�TRUE-�ɹ���FALSE-ʧ��
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

//���ܣ����÷����������˿�
//�������˿ڱ���
//���أ���
void CTCPClient_FT::SetServerPort(int iPort)
{
	m_iServerPort = iPort;
}


//���ܣ����������߳�
//����������ṹ�����
//���أ���
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


//���ܣ�����������Ϣ�����շ�����Ϣ
//������LPVOID lpParam
//���أ���������
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
		//��������
		int iSendCnt = send(sock, (char*)&pParam->msgRequest, sizeof(MSGREQUEST), 0);

		int iRecvCnt = -1;
		if(pParam->msgRequest.iCommand == FILELIST)		//����������ļ��б�
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
		
		else if(pParam->msgRequest.iCommand == FILEDATA)	//����������ļ�����
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

			if(lFileOffset < pParam->msgRequest.lFileLength && !CTCPClient_FT::m_bEndRecvThread)		//���û��������,��������
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

//���ܣ�ֹͣ���н����̣߳��û�ǿ�ƽ�����
//��������
//���أ���
void CTCPClient_FT::StopAllRecvThread()
{
	m_bEndRecvThread = TRUE;
}