// FThread.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FThread.h"



// FThread

IMPLEMENT_DYNCREATE(FThread, CWinThread)

CString fNmae;

FThread::FThread()
{
}

FThread::~FThread()
{
}

BOOL FThread::InitInstance()
{
	// TODO:    �ڴ�ִ���������̳߳�ʼ��
	// ���������ڶԻ���
	CDialog dlg;
	dlg.SubclassDlgItem(IDD_FDIALOG,this->m_pMainWnd);
	m_pMainWnd = &dlg;
	dlg.DoModal();
	//if(isSend==TRUE)
		CreateServer("");
	//else
		CreateClient("");
	return TRUE;
}

int FThread::ExitInstance()
{
	// TODO:    �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(FThread, CWinThread)
	ON_BN_CLICKED(IDC_FCANCEL, &FThread::OnClickedFcancel)
END_MESSAGE_MAP()


BOOL CreateServer(CString FileName){
	/*sockaddr_in server_addr; 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY; 
	server_addr.sin_port = htons(FILEPORT);
	::memset(server_addr.sin_zero,0,8);
	// ����socket 
  SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0); 
    if (SOCKET_ERROR == m_Socket) 
    { 
		::AfxMessageBox("Create Socket Error!"); 
    } 
  
  //��socket�ͷ����(����)��ַ 
  if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr))) 
  { 
    ::AfxMessageBox("Server Bind Failed: "+ WSAGetLastError()); 
  } 
  
  //���� 
  if (SOCKET_ERROR == listen(m_Socket, 10)) 
  { 
    ::AfxMessageBox("Server Listen Failed: "+WSAGetLastError()); 
  }*/
	return 1;
}
BOOL CreateClient(CString FileName){
	return 1;
}

// FThread ��Ϣ�������


void FThread::OnClickedFcancel()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
// C:\Users\Jim\Desktop\WinTalk03\WinTalk\FThread.cpp : ʵ���ļ�
//


/*

// FDlg �Ի���

IMPLEMENT_DYNAMIC(FDlg, CDialog)

FDlg::FDlg(CWnd* pParent /*=NULL*//*)
	: CDialog(FDlg::IDD, pParent)
{

}

FDlg::~FDlg()
{
}

void FDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(FDlg, CDialog)
END_MESSAGE_MAP()


// FDlg ��Ϣ�������
*/