// TCPClient_FT.h: interface for the CTCPClient_FT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCLIENT_FT_H__D1CD6225_FE1E_4086_984B_FE43FAFC7C67__INCLUDED_)
#define AFX_TCPCLIENT_FT_H__D1CD6225_FE1E_4086_984B_FE43FAFC7C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define			WM_RECVFILELIST		WM_USER + 1005		//�����ļ��б�
#define			WM_FILESTATUS		WM_USER + 1006		//��ӳ�ļ�����״̬
#define			WM_CONNECTERROR		WM_USER + 1007		//����ʧ��


#define			FILELIST		    1000				//�����ļ��б�
#define			FILEDATA			2000				//�����ļ�����

#define			RECVSIZE			1024*16		    	//һ���Է���/���յ����ݿ��С

//�������ͨ�����Խṹ
typedef struct
{
	int  iCommand;
	long lFileOffset;
	long lFileLength;
	char sClientPath[128];
	char sServerPath[128];
}MSGREQUEST;

//�����̲߳���
typedef struct
{
	char sIP[15];
	int  iPort;
	MSGREQUEST msgRequest;
	LPVOID ptr;
}PARAMREQUEST;

//�ļ��б�ṹ
typedef struct
{
	long lFileLength;
	char sServerPath[128];
}MSGFILELIST;


//��ӳ�ļ�״̬�Ľṹ
typedef struct
{
	long lFileOffset;
	char sServerPath[128];
}MSGFILESTATUS;

class CTCPClient_FT  
{
public:
	CTCPClient_FT();
	virtual ~CTCPClient_FT();

	//���ع��캯�������ָ�򴰿�ָ��
	CTCPClient_FT(LPVOID ptr);

	//���÷�����ip
	BOOL SetServerIP(char* sIP);

	//���÷������˿�
	void SetServerPort(int iPort);

	//���������߳�
	void RequestFile(MSGREQUEST msgRequest);
	
	//�������󲢽��շ�����Ϣ���߳�
	static DWORD WINAPI ThreadRequestFile(LPVOID lpParam);

	//ֹͣ���н����߳�
	static void StopAllRecvThread(void);

private:
	//��������ַ
	char m_sIP[15];

	//�������˿�
	int m_iServerPort;

	//ָ�򴰿ڵ�ָ��
	CWnd* m_pWnd;

	//�����߳̽�����
	static BOOL m_bEndRecvThread;
};

#endif // !defined(AFX_TCPCLIENT_FT_H__D1CD6225_FE1E_4086_984B_FE43FAFC7C67__INCLUDED_)
