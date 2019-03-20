// TCPClient_FT.h: interface for the CTCPClient_FT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCLIENT_FT_H__D1CD6225_FE1E_4086_984B_FE43FAFC7C67__INCLUDED_)
#define AFX_TCPCLIENT_FT_H__D1CD6225_FE1E_4086_984B_FE43FAFC7C67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define			WM_RECVFILELIST		WM_USER + 1005		//接收文件列表
#define			WM_FILESTATUS		WM_USER + 1006		//反映文件下载状态
#define			WM_CONNECTERROR		WM_USER + 1007		//连接失败


#define			FILELIST		    1000				//请求文件列表
#define			FILEDATA			2000				//请求文件数据

#define			RECVSIZE			1024*16		    	//一次性发送/接收的数据块大小

//与服务器通信语言结构
typedef struct
{
	int  iCommand;
	long lFileOffset;
	long lFileLength;
	char sClientPath[128];
	char sServerPath[128];
}MSGREQUEST;

//请求线程参数
typedef struct
{
	char sIP[15];
	int  iPort;
	MSGREQUEST msgRequest;
	LPVOID ptr;
}PARAMREQUEST;

//文件列表结构
typedef struct
{
	long lFileLength;
	char sServerPath[128];
}MSGFILELIST;


//反映文件状态的结构
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

	//重载构造函数，添加指向窗口指针
	CTCPClient_FT(LPVOID ptr);

	//设置服务器ip
	BOOL SetServerIP(char* sIP);

	//设置服务器端口
	void SetServerPort(int iPort);

	//创建请求线程
	void RequestFile(MSGREQUEST msgRequest);
	
	//发送请求并接收反馈信息的线程
	static DWORD WINAPI ThreadRequestFile(LPVOID lpParam);

	//停止所有接收线程
	static void StopAllRecvThread(void);

private:
	//服务器地址
	char m_sIP[15];

	//服务器端口
	int m_iServerPort;

	//指向窗口的指针
	CWnd* m_pWnd;

	//接收线程结束符
	static BOOL m_bEndRecvThread;
};

#endif // !defined(AFX_TCPCLIENT_FT_H__D1CD6225_FE1E_4086_984B_FE43FAFC7C67__INCLUDED_)
