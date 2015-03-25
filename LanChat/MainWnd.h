
// MainWnd.h : ͷ�ļ�
//

#pragma once
#include "Resource.h"
#include "MyUdp.h"

class CMsgDlg;

// CMainWnd �Ի���
class CMainWnd : public CDialogEx
{
// ����
public:
	CMainWnd(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	//char *m_pBuf;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString m_strName;
	CMyUdp m_udp;
	UINT m_nLocalPort;
	UINT m_nPeerPort;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedSeting();
	
	void NewMsg(MyData *pdata, int nMsgLen, SOCKADDR_IN * pnewaddr);
	void NewMember(LPCTSTR name, SOCKADDR_IN * pnewaddr);
	void SendMyInfo(Operate ope, SOCKADDR_IN *paddr);
	void EnterOrQuit(Operate ope);
	afx_msg void OnDestroy();
	void SomeoneQuit(LPCTSTR name, SOCKADDR_IN * paddr);
	afx_msg void OnDblclkMemlist();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void SendNewMsg(CString &msg, SOCKADDR_IN * lpSockAddr, WORD wNum);
	
	void ShowNewMsg(LPCTSTR lpszMsg, SOCKADDR_IN * lpFromSockAddr, bool bIsSysMsg=false);
	CMsgDlg* GetMsgDlg(SOCKADDR_IN* lpFromSockAddr);
	virtual void OnOK();
};

