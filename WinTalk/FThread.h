#pragma once


#include "resource.h"
#include "afxdialogex.h"

// FThread

class FThread : public CWinThread
{
	DECLARE_DYNCREATE(FThread)

protected:
	virtual ~FThread();

public:
	FThread();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	BOOL CreateServer(CString FileName);
	BOOL CreateClient(CString FileName);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedFcancel();
};

/*
#pragma once


// FDlg �Ի���

public class FDlg : public CDialog
{
	DECLARE_DYNAMIC(FDlg)

public:
	FDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~FDlg();

// �Ի�������
	enum { IDD = IDD_FDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
*/