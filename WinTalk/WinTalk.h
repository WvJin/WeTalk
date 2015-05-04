
// WinTalk.h : WinTalk Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������
#include "afxwin.h"
#include <afxcmn.h>
#include <winsock2.h> 
#include <commctrl.h>
#include <afxtempl.h>

#define BUFFER_SIZE 512
//using std::queue;

// ������������WS2_32������
#pragma comment(lib,"WS2_32.lib")
// ���ӵ�comctl32.lib��
#pragma comment(lib,"comctl32.lib")


// CWinTalkApp:
// �йش����ʵ�֣������ WinTalk.cpp
//


class CWinTalkApp : public CWinApp
{
public:
	CWinTalkApp();


// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

public:
	//afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWinTalkApp theApp;

// MyDlg dialog

class MyDlg : public CDialog
{
	DECLARE_DYNAMIC(MyDlg)

public:
	MyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~MyDlg();
	void AddList(sockaddr_in);
	BOOL UDPMulticast();
	BOOL CreateServer();
	BOOL SendMC(char);	//�����鲥
	BOOL CreateFServer(char *);
	BOOL CreateFClient(char *,sockaddr_in);
// Dialog Data
	enum { IDD = IDD_MAINDIALOG };

protected:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// �׽���֪ͨ�¼�
	afx_msg long OnSocketMC(WPARAM wParam, LPARAM lParam);
	afx_msg long OnSocketMSG(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl mListHost;
	CEdit mEditInput;
	CEdit mEditLog;
	CArray<sockaddr_in, sockaddr_in&> s_List;
	SOCKET socketMC;
	SOCKET socketMsg;
	sockaddr_in localaddr;
	sockaddr_in remote;
	char hostname[128];
	struct hostent*pHost;

	CString mLogStr ;
	CString mInputStr ;
	afx_msg void OnClickedButtonMsg();
	afx_msg void OnBnClickedButtonFile();
	afx_msg void OnClickedButtonFlush();
};
