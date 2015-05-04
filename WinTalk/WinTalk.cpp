
// WinTalk.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "WinTalk.h"
#include "MainFrm.h"
#include "FDialog.h"

// ���������¼�֪ͨ��Ϣ
#define WM_SOCKET_MC WM_USER + 101
#define WM_SOCKET_MSG WM_USER + 102

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWinTalkApp

BEGIN_MESSAGE_MAP(CWinTalkApp, CWinApp)
	//ON_COMMAND(ID_APP_ABOUT, &CWinTalkApp::OnAppAbout)
END_MESSAGE_MAP()


// CWinTalkApp ����

CWinTalkApp::CWinTalkApp()
{
	// TODO:  ������Ӧ�ó��� ID �ַ����滻ΪΨһ�� ID �ַ�����������ַ�����ʽ
	//Ϊ CompanyName.ProductName.SubProduct.VersionInformation
	//SetAppID(_T("WinTalk.AppID.NoVersion"));
	// TODO:  �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}

// Ψһ��һ�� CWinTalkApp ����

CWinTalkApp theApp;


// CWinTalkApp ��ʼ��

BOOL CWinTalkApp::InitInstance()
{
	CWinApp::InitInstance();

	// ��ʼ��Winsock��
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 0);
	::WSAStartup(sockVersion, &wsaData);
	// ���������ڶԻ���
	MyDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	return FALSE;
}

int CWinTalkApp::ExitInstance()
{
	//TODO:  �����������ӵĸ�����Դ
	return CWinApp::ExitInstance();
}

// CWinTalkApp ��Ϣ�������

// MyDlg dialog

IMPLEMENT_DYNAMIC(MyDlg, CDialog)

MyDlg::MyDlg(CWnd* pParent /*=NULL*/)
: CDialog(MyDlg::IDD, pParent)
, mInputStr(_T(""))
, mLogStr(_T(""))
{
}

MyDlg::~MyDlg()
{
}

void MyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INPUT, mInputStr);
	DDV_MaxChars(pDX, mInputStr, 500);
	DDX_Text(pDX, IDC_EDIT_LOG, mLogStr);
	DDV_MaxChars(pDX, mLogStr, 10000);
}


BEGIN_MESSAGE_MAP(MyDlg, CDialog)
	ON_MESSAGE(WM_SOCKET_MC,OnSocketMC)
	ON_MESSAGE(WM_SOCKET_MSG, OnSocketMSG)
	ON_BN_CLICKED(IDC_BUTTON_MSG, &MyDlg::OnClickedButtonMsg)
	ON_BN_CLICKED(IDC_BUTTON_FILE, &MyDlg::OnBnClickedButtonFile)
	ON_BN_CLICKED(IDC_BUTTON_FLUSH, &MyDlg::OnClickedButtonFlush)
END_MESSAGE_MAP()


// MyDlg message handlers
void MyDlg::OnCancel()
{
	//TODO 
	SendMC('Q');
	if(socketMC != INVALID_SOCKET)
	::closesocket(socketMC);
	if(socketMsg != INVALID_SOCKET)
	::closesocket(socketMsg);
	::WSACleanup();
	CDialog::OnCancel();
}

BOOL MyDlg::OnInitDialog()
{
	mListHost.SubclassDlgItem(IDC_LIST_HOST,this);
	LONG lStyle;
	lStyle = GetWindowLong(mListHost.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style
	lStyle &= ~LVS_TYPEMASK; //�����ʾ��ʽλ
	lStyle |= LVS_REPORT; //����style
	SetWindowLong(mListHost.m_hWnd, GWL_STYLE, lStyle);//����style
	mListHost.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES/*|LVS_EX_CHECKBOXES*/); //������չ���
	mListHost.InsertColumn( 0, "IP", LVCFMT_LEFT, 175 );//������

	
	if(gethostname(hostname,128)!=0){
		::AfxMessageBox("��ȡ�û���ʧ��");
	}
	pHost = gethostbyname(hostname);

	socketMC = ::socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	UDPMulticast();	//��ʼ���鲥���緢��
	SendMC('J');	//�����鲥��Ϣ,Jȡ"JOIN"
	if(!CreateServer()){
		::AfxMessageBox("��ʼ��ʧ��");
	}
	return 1;
}

void MyDlg::AddList(sockaddr_in sa){
	for (int i = 0; i < s_List.GetCount(); i++){	//�Ѿ��и�IP��return
		if (s_List.GetAt(i).sin_addr.S_un.S_addr == sa.sin_addr.S_un.S_addr)
			return;
	}
	s_List.Add(sa);
	int index = mListHost.GetItemCount();
	int nRow = mListHost.InsertItem(index, inet_ntoa(sa.sin_addr));//������
	//mListHost.SetItemText(nRow, 1, "jack");//��������
}
//UDPʵ���鲥���緢��
BOOL MyDlg::UDPMulticast(){
	if(socketMC == INVALID_SOCKET){
		return 0;
	}
	int ret;
	sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_port = htons(MCPORT);
	local.sin_addr.S_un.S_addr = INADDR_ANY;
	memset(local.sin_zero, 0, 8);
	ret = bind(socketMC, (struct sockaddr *)&local, sizeof(local));	//�󶨵�ַ
	if(ret==SOCKET_ERROR) return FALSE;

	ret = ::WSAAsyncSelect(socketMC,m_hWnd,WM_SOCKET_MC,FD_READ|FD_CLOSE);	//�����첽ģʽ
	if(ret==SOCKET_ERROR) return FALSE;
	//�����鲥
	ip_mreq mreq;
	memset(&mreq, 0, sizeof(struct ip_mreq));
	mreq.imr_multiaddr.S_un.S_addr = inet_addr("234.0.12.34");    //�鲥Դ��ַ
	mreq.imr_interface.S_un.S_addr = inet_addr(inet_ntoa(*(struct in_addr*)pHost->h_addr_list[0]));       //���ص�ַ
	int m = setsockopt(socketMC, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(struct ip_mreq));
	if (m == SOCKET_ERROR) return FALSE;
	//�� IP ͷ�����ó��ֶ��㲥���ݱ��ġ���Чʱ�䡱��TTL��
	int optval = 8;
	setsockopt(socketMC, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&optval, sizeof(int));
	//ָ�������������Ƕ��㲥��ĳ�Աʱ���Ƿ񽫳��ֶ��㲥���ݱ��ĸ�����������������
	int loop = 0;
	setsockopt(socketMC, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&loop, sizeof(int));

	remote.sin_addr.S_un.S_addr = inet_addr("234.0.12.34");  //�鲥��ַ
	remote.sin_family = AF_INET;  
	remote.sin_port = htons(MCPORT);  
	memset(remote.sin_zero,0,8);

	//setsockopt(sock,IPPROTO_IP,IP_DROP_MEMBERSHIP,(char*)&mcast,sizeof(mcast));	//!!!�ر�ʱ���ã��뿪�鲥
	//closesocket(sock);	//!!!
	return 1;
}

//�����鲥
BOOL MyDlg::SendMC(char c){
	char buff[2] = { c, '\0' };		//�鲥���緢�ֱ�ʶ
	return sendto(socketMC, buff, 2, 0, (struct sockaddr*)&remote, sizeof(remote))==2?TRUE:FALSE;
}

BOOL MyDlg::CreateServer(){
	socketMsg = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (socketMsg == INVALID_SOCKET) return FALSE;
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(MSGPORT);
	localaddr.sin_addr.S_un.S_addr = INADDR_ANY;
	memset(localaddr.sin_zero, 0, 8);
	//�󶨵�ַ
	if (bind(socketMsg, (struct sockaddr *)&localaddr, sizeof(localaddr)) == SOCKET_ERROR) return FALSE;
	//�����첽ģʽ
	if (::WSAAsyncSelect(socketMsg, m_hWnd, WM_SOCKET_MSG, FD_READ | FD_CLOSE) == SOCKET_ERROR) return FALSE;
	return TRUE;
}

long MyDlg::OnSocketMC(WPARAM wParam,LPARAM lParam){
	SOCKET s = wParam;
	if (WSAGETSELECTERROR(lParam)){
		::closesocket(s);
		return 0;//TODO
	}
	switch (WSAGETSELECTEVENT(lParam)){
	case FD_READ:
		{
			//TODO
			sockaddr_in from;
			int fromlen = sizeof(from);
			char recv[2];
			memset(recv, 0, sizeof(char) * 2);
			if (::recvfrom(s, recv, sizeof(recv), 0, (sockaddr*)&from, &fromlen) == -1){
				::closesocket(s);
				break;
			}
			if (recv[0] == 'R' || recv[0] == 'J'){
				AddList(from);	//��IP�����б���
			}
			if (recv[0] == 'J'){
				//JOIN,���ͻ�ӦRESPONSE
				char buffT[2] = { 'R', '\0' };
				::sendto(s, buffT, strlen(buffT), 0, (struct sockaddr*)&from, sizeof(from));
			}
			else if (recv[0] == 'Q'){	//Quit�˳���ɾ����ӦIP
				for (int i = 0; i < s_List.GetCount(); i++){	
					if (s_List.GetAt(i).sin_addr.S_un.S_addr == from.sin_addr.S_un.S_addr){
						s_List.RemoveAt(i);
						mListHost.DeleteItem(i);
						break;
					}
				}
			}
		}
		break;
	case FD_CLOSE:
		::closesocket(s);
		break;
	}

	return 0;
}

long MyDlg::OnSocketMSG(WPARAM wParam, LPARAM lParam){
	SOCKET s = wParam;
	if (WSAGETSELECTERROR(lParam)){
		::closesocket(s);
		return 0;//TODO
	}
	switch (WSAGETSELECTEVENT(lParam)){
	case FD_READ:
		{
			//TODO
			UpdateData();
			sockaddr_in from;
			int fromlen = sizeof(from);
			char recv[MAX_INPUT];
			memset(recv, 0, sizeof(char) * MAX_INPUT);
			if (::recvfrom(s, recv, sizeof(recv), 0, (sockaddr*)&from, &fromlen) == -1){
				::closesocket(s);
				break;
			}
			CString s = recv;
			int n = s.Find(':');
			CString s1 = s.Left(n);
			CString s2 = s.Right(s.GetLength()-n-1);
			if(s1 == "FILESEND"){			//�����ļ���Ϣ
				//TODO
				//FDialog dlg;
				//if(dlg.DoModal()==IDOK){
					//TODO�����ļ�
				::AfxMessageBox(s2);
					char fileName[100] = "";
					OPENFILENAME file={0};
					file.lStructSize=sizeof(file);
					file.lpstrFile=fileName;
					file.nMaxFile = 100;
					file.lpstrFilter = "All Files(*.*)\0*.*\0\0";
					file.nFilterIndex = 1;
					if(!::GetSaveFileName(&file))
						return 0;
					CreateFClient(fileName,from);
				//}else return 0;
				
				//return 0;
			}
			if (mLogStr.GetLength() + 300 > 10000)
				mLogStr.Delete(0, 300);
			mLogStr += "[";					//������Ϣ���Ҹ���Log
			mLogStr += inet_ntoa(from.sin_addr);
			mLogStr += "]˵��\r\n	";
			mLogStr += recv; mLogStr += "\r\n\r\n";
			UpdateData(FALSE);
		}

		break;
	case FD_CLOSE:
		::closesocket(s);
		break;
	}

	return 0;
}

void MyDlg::OnClickedButtonMsg()
{
	if (mListHost.GetFirstSelectedItemPosition() == NULL) return;
	//��ȡ������ı�
	UpdateData();
	CString s = "���";
	char* buff = (LPSTR)(LPCTSTR)mInputStr;
	int len = mInputStr.GetLength();
	// ����ѡ�е�IP

	for (int i = 0; i<mListHost.GetItemCount(); i++)
	{
		int ii;
		if ( (ii= mListHost.GetItemState(i, LVIS_SELECTED))
			== LVIS_SELECTED /*|| mListHost.GetCheck(i)*/)
		{
			sockaddr_in remoteT /*= s_List.GetAt(i)*/;
			remoteT.sin_addr.S_un.S_addr = (s_List.GetAt(i)).sin_addr.S_un.S_addr;
			remoteT.sin_family = AF_INET;
			remoteT.sin_port = htons(MSGPORT);
			memset(remoteT.sin_zero, 0, 8);
			int ret = sendto(socketMsg, buff, len, 0, (sockaddr*)&remoteT, sizeof(remoteT));	//����Ϣ
			s += "["; s += inet_ntoa(s_List.GetAt(i).sin_addr); s += "]";
		}
	}
	s += "˵��\r\n	";
	if (mLogStr.GetLength() + s.GetLength() > 10000)
		mLogStr.Delete(0, s.GetLength());
	mLogStr += (s + mInputStr+"\r\n\r\n");
	mInputStr = "";
	UpdateData(FALSE);
}


void MyDlg::OnBnClickedButtonFile()
{
	//TODO
	char fileName[100] = "";
	OPENFILENAME file={0};
	file.lStructSize=sizeof(file);
	file.lpstrFile=fileName;
	file.nMaxFile = 100;
	file.lpstrFilter = "All Files(*.*)\0*.*\0\0";
	file.nFilterIndex = 1;
	if(!::GetOpenFileName(&file)){
		return;
	}
	
	CString buff = "FILESEND";buff += ":";
	buff += inet_ntoa(*(struct in_addr*)pHost->h_addr_list[0]);
	buff += "���㷢���ļ� ";buff += fileName;
	int len = buff.GetLength();
	for (int i = 0; i<mListHost.GetItemCount(); i++)
	{
		int ii;
		if ( (ii= mListHost.GetItemState(i, LVIS_SELECTED))
			== LVIS_SELECTED /*|| mListHost.GetCheck(i)*/)
		{
			sockaddr_in remoteT /*= s_List.GetAt(i)*/;
			remoteT.sin_addr.S_un.S_addr = (s_List.GetAt(i)).sin_addr.S_un.S_addr;
			//::AfxMessageBox(inet_ntoa(s_List.GetAt(i).sin_addr));
			remoteT.sin_family = AF_INET;
			remoteT.sin_port = htons(MSGPORT);
			memset(remoteT.sin_zero, 0, 8);
			int ret = sendto(socketMsg, buff, len, 0, (sockaddr*)&remoteT, sizeof(remoteT));//�����ļ����ӣ��ȴ��Է�ͬ��
			CreateFServer(fileName);
			break;
		}
	}
}

BOOL MyDlg::CreateFServer(char * file_name){
	//char file_name[FILE_NAME_MAX_SIZE+1]= fName;
	sockaddr_in server_addr; 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY; 
	server_addr.sin_port = htons(FILEPORT);

	// ����socket 
	SOCKET m_Socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (SOCKET_ERROR == m_Socket) 
	{ 
		::AfxMessageBox("Create Socket Error!"); //closesocket(c_Socket); 
				return 0;
	} 

	//��socket�ͷ����(����)��ַ 
	if (SOCKET_ERROR == bind(m_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr))) 
	{ 
		::AfxMessageBox("Server Bind Failed: "+ WSAGetLastError()); 
		return 0;
	} 

	//���� 
	if (SOCKET_ERROR == listen(m_Socket, 10)) 
	{ 
		::AfxMessageBox("Server Listen Failed: "+ WSAGetLastError()); closesocket(m_Socket); 
				return 0;
	} 

	//while(1) 
	//{ 
		::AfxMessageBox("��ȷ����ʼ����..\n"); 

		sockaddr_in client_addr; 
		int client_addr_len = sizeof(client_addr); 

		m_Socket = accept(m_Socket, (sockaddr *)&client_addr, &client_addr_len); 
		if (SOCKET_ERROR == m_Socket) 
		{ 
			::AfxMessageBox("Server Accept Failed: "+ WSAGetLastError()); closesocket(m_Socket); 
				return 0;
		} 

		char buffer[BUFFER_SIZE];

		memset(buffer, 0, BUFFER_SIZE); 

		
		FILE * fp = fopen(file_name, "rb"); //windows����"rb",��ʾ��һ��ֻ���Ķ������ļ� 
		if (NULL == fp) 
		{ 
			::AfxMessageBox("File:  Not Found\n"); 
		} 
		else
		{ 
			memset(buffer, 0, BUFFER_SIZE); 
			int length = 0; 

			while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) 
			{ 
				if (send(m_Socket, buffer, length, 0) < 0) 
				{ 
					::AfxMessageBox("Send File:  Failed\n"); closesocket(m_Socket); 
				return 0;
				} 
				memset(buffer, 0, BUFFER_SIZE); 
			} 

			fclose(fp); 
			closesocket(m_Socket); 
		}
		
		::AfxMessageBox("File:  Transfer Successful!\n"); 

	//}
	return 1;
}
BOOL MyDlg::CreateFClient(char * file_name,sockaddr_in addr){
	//����socket 
	SOCKET c_Socket = socket(AF_INET, SOCK_STREAM, 0); 
	if (SOCKET_ERROR == c_Socket) 
	{ 
		::AfxMessageBox("Create Socket Error!"); closesocket(c_Socket); 
				return 0;
	} 

	//ָ������˵ĵ�ַ 
	sockaddr_in server_addr; 
	server_addr.sin_family = AF_INET; 
	server_addr.sin_addr.S_un.S_addr = addr.sin_addr.S_un.S_addr; 
	server_addr.sin_port = htons(FILEPORT); 

	if (SOCKET_ERROR == connect(c_Socket, (LPSOCKADDR)&server_addr, sizeof(server_addr))) 
	{ 
		::AfxMessageBox("Can Not Connect To Server IP!\n"); 
		closesocket(c_Socket); 
				return 0;
	} 

	//�����ļ��� 
	//char file_name[FILE_NAME_MAX_SIZE+1]="D:/1.jpg"; 

	char buffer[BUFFER_SIZE]; 
	FILE * fp = fopen(file_name, "wb"); //windows����"wb",��ʾ��һ��ֻд�Ķ������ļ� 
	if(NULL == fp) 
	{ 
		::AfxMessageBox("File:  Can Not Open To Write\n"); 
	} 
	else
	{ 
		memset(buffer, 0, BUFFER_SIZE); 
		int length = 0; 
		while ((length = recv(c_Socket, buffer, BUFFER_SIZE, 0)) > 0) 
		{ 
			if (fwrite(buffer, sizeof(char), length, fp) < length) 
			{ 
				::AfxMessageBox("File:  Write Failed\n"); 
				fclose(fp); 
				closesocket(c_Socket); 
				return 0;
			} 
			memset(buffer, 0, BUFFER_SIZE); 
		} 
		//TODO
	} 
	
	::AfxMessageBox("Receive File:  From client Successful!\n"); 
	fclose(fp); 
	closesocket(c_Socket);
	return 1;
}

void MyDlg::OnClickedButtonFlush()
{
	//FDialog dlg();
	//dlg.DoModal();
	s_List.RemoveAll();
	mListHost.DeleteAllItems();
	if(socketMC == INVALID_SOCKET)	::AfxMessageBox("�鲥�׽���ʧ��");
	if(!SendMC('J')) ::AfxMessageBox("�鲥����ʧ��");
}
