
// MainWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SetDlg.h"
#include "LanChat.h"
#include "MainWnd.h"
#include "afxdialogex.h"
#include "MsgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainWnd �Ի���



CMainWnd::CMainWnd(CWnd* pParent /*=NULL*/)
: CDialogEx(CMainWnd::IDD, pParent)
, m_strName(_T(""))
//, m_pBuf(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nLocalPort = 1221;
	m_nPeerPort = 1221;
}

void CMainWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME, m_strName);
}

BEGIN_MESSAGE_MAP(CMainWnd, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_SETING, &CMainWnd::OnBnClickedSeting)
	ON_WM_DESTROY()
	ON_LBN_DBLCLK(IDC_MEMLIST, &CMainWnd::OnDblclkMemlist)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CMainWnd ��Ϣ�������

BOOL CMainWnd::OnInitDialog()
{
	TCHAR szName[MAX_PATH];
	DWORD nlen = sizeof(szName);
	::GetComputerName(szName, &nlen);
	CString string;
	string.Format(TEXT("%d"), m_nLocalPort);
	m_strName = m_strName + szName + string;

	if (!m_udp.Create(m_nLocalPort, SOCK_DGRAM)){
		AfxMessageBox(TEXT("Create socket error!"));
		return FALSE;
	}
	SetTimer(1, 1500, NULL);
	EnterOrQuit(CIN);

	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	CRect rect;
	GetClientRect(&rect);
	int cx = ::GetSystemMetrics(SM_CXSCREEN);
	int cy = ::GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(NULL, cx - rect.Width() - 45,
		(cy - rect.Height()) / 2 - 45, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER);
	
	//string.Format(TEXT("LanChat - %s"), m_strName);
	//SetWindowText(string);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMainWnd::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMainWnd::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMainWnd::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	CRect rect, rectitem;
	GetClientRect(&rect);
	CWnd *pwnd = GetDlgItem(IDC_MEMLIST);
	pwnd->GetWindowRect(&rectitem);
	ScreenToClient(&rectitem);
	pwnd->SetWindowPos(this, 0, 0,
		rect.right - rectitem.left * 2,
		rect.bottom - rectitem.top - rectitem.left,
		SWP_NOMOVE | SWP_NOZORDER);
}


void CMainWnd::OnBnClickedSeting()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CSetDlg dlg;
	dlg.m_strName = m_strName;
	dlg.m_nPeerPort = m_nPeerPort;
	dlg.m_nLocalPort = m_nLocalPort;

	if (dlg.DoModal() != IDOK)
		return;
	m_udp.Close();

	m_strName = dlg.m_strName;
	m_nLocalPort = dlg.m_nLocalPort;
	m_nPeerPort = dlg.m_nPeerPort;
	UpdateData(FALSE);

	if (!m_udp.Create(m_nLocalPort, SOCK_DGRAM)){
		AfxMessageBox(TEXT("Create socket error!"));
		return;
	}
	SetTimer(1, 1500, NULL);
	EnterOrQuit(CIN);

	//CString string;
	//string.Format(TEXT("LanChat - %s"), m_strName);
	//SetWindowText(string);
}


void CMainWnd::NewMsg(MyData *pdata, int nMsgLen, SOCKADDR_IN * pnewaddr)
{

	switch (pdata->operate)
	{
	case CIN:
		SendMyInfo(ONL, pnewaddr);
		//����
	case ONL:
		NewMember(pdata->data, pnewaddr);
		delete[] pdata;
		break;
	case QUT:
		SomeoneQuit(pdata->data, pnewaddr);
		delete[] pdata;
		break;
	case TXT:
		ShowNewMsg(pdata->data, pnewaddr);
		delete[] pdata;
		delete pnewaddr;
		break;
	case FIL:
	case FLD:
	case VOX:
	{
				//��ʾ�Է������͵��ļ������ȴ��û�ѡ��
				auto p = GetMsgDlg(pnewaddr);
				if (!p){
					MessageBox(TEXT("���յ�δ֪��Ա����Ϣ"));
					delete[] pdata;
				}
				else
					p->MsgOfTrans(pdata, nMsgLen);
				delete pnewaddr;
	}
		break;
	default:
		delete[] pdata;
		break;
	}

	return;
}


void CMainWnd::NewMember(LPCTSTR name, SOCKADDR_IN * pnewaddr)
{

	HOSTENT *hostent;
	char szHostName[MAX_HOSTNAME_LEN];

	gethostname(szHostName, sizeof(szHostName));
	hostent = gethostbyname(szHostName);
	for (char **p = hostent->h_addr_list; *p; ++p){
		ULONG myaddr = *((ULONG*)*p);
		if (myaddr == pnewaddr->sin_addr.s_addr){
			delete pnewaddr;
			return;
		}
	}


	//�ų����ڳ�Ա�б��еĹ㲥
	CListBox *pwnd = (CListBox *)GetDlgItem(IDC_MEMLIST);
	int count = pwnd->GetCount();

	while (count){
		auto p = (ItemData *)pwnd->GetItemDataPtr(--count);
		if (p && p->m_addr->sin_port == pnewaddr->sin_port&&
			p->m_addr->sin_addr.s_addr == pnewaddr->sin_addr.s_addr){
			if (p->m_pdlg)
				p->m_pdlg->StatusChange(true);
			pwnd->DeleteString(count);
			pwnd->InsertString(count, name);
			pwnd->SetItemDataPtr(count, p);
			delete pnewaddr;
			return;
		}
	}
	int nIndex = pwnd->AddString(name);
	auto pdata = new ItemData(pnewaddr);
	if (!pdata){
		MessageBox(TEXT("�ڴ�������"), TEXT("����"));
		return;
	}
	pwnd->SetItemDataPtr(nIndex, pdata);

	return;
}


void CMainWnd::SendMyInfo(Operate ope, SOCKADDR_IN *paddr)
{
	int nLen = m_strName.GetLength()*sizeof(TCHAR)+sizeof(MyData);
	auto buf = new char[nLen];
	if (!buf){
		MessageBox(TEXT("�ڴ�������"), TEXT("����"));
		return;
	}
	memset(buf, 0, nLen);

	MyData * p = (MyData *)buf;
	p->operate = ope;
	p->number = 0;
	wcscpy_s(p->data, (nLen - sizeof(WORD)* 2) / sizeof(TCHAR), m_strName);

	m_udp.SendTo(buf, nLen, (SOCKADDR *)paddr, sizeof(SOCKADDR_IN));

	delete[] buf;
	return;
}


void CMainWnd::EnterOrQuit(Operate ope)
{
	BOOL bValue = TRUE;
	m_udp.SetSockOpt(SO_BROADCAST, &bValue, sizeof(bValue));

	SOCKADDR_IN bcaddr;
	bcaddr.sin_family = AF_INET;
	bcaddr.sin_port = htons(m_nPeerPort);
	bcaddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

	SendMyInfo(ope, &bcaddr);

	bValue = FALSE;
	m_udp.SetSockOpt(SO_BROADCAST, &bValue, sizeof(bValue));
}


void CMainWnd::OnDestroy()
{
	KillTimer(1);

	CListBox *pwnd = (CListBox *)GetDlgItem(IDC_MEMLIST);
	int count = pwnd->GetCount();

	while (count){
		auto p = (ItemData *)pwnd->GetItemDataPtr(--count);
		delete p;
	}

	EnterOrQuit(QUT);
	m_udp.ClearMsgList();
	m_udp.Close();

	CDialogEx::OnDestroy();

	// TODO:  �ڴ˴������Ϣ����������
}


void CMainWnd::SomeoneQuit(LPCTSTR name, SOCKADDR_IN * paddr)
{
	CListBox *pwnd = (CListBox *)GetDlgItem(IDC_MEMLIST);
	int count = pwnd->GetCount();

	while (count){
		auto p = (ItemData *)pwnd->GetItemDataPtr(--count);
		if (p && p->m_addr->sin_port == paddr->sin_port&&
			p->m_addr->sin_addr.s_addr == paddr->sin_addr.s_addr){
			//delete p;
			//pwnd->DeleteString(count);
			if (p->m_pdlg)
				p->m_pdlg->StatusChange(false);
			pwnd->DeleteString(count);
			CString string;
			string.Format(TEXT("%s(����)"), name);
			pwnd->InsertString(count, string);
			pwnd->SetItemDataPtr(count, p);
			break;
		}
	}
	delete paddr;
	return;
}


void CMainWnd::OnDblclkMemlist()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	auto pwnd = (CListBox *)GetDlgItem(IDC_MEMLIST);
	int nIndex = pwnd->GetCurSel();
	if (nIndex == LB_ERR)
		return;
	auto pdata = (ItemData*)pwnd->GetItemDataPtr(nIndex);

	//�ж��Ƿ��Ѿ������˴���
	if (pdata->m_pdlg){
		pdata->m_pdlg->ShowWindow(SW_SHOW);
		pdata->m_pdlg->SetActiveWindow();
		return;
	}

	//δ����������
	CString strName;
	pwnd->GetText(nIndex, strName);

	auto pnewwnd = new CMsgDlg;

	if (pnewwnd){
		pnewwnd->m_pdata = pdata;
		pnewwnd->m_strTitle = strName;
		pdata->m_pdlg = pnewwnd;
		pnewwnd->Create(IDD_MSGDLG);
		pnewwnd->ShowWindow(SW_SHOW);
	}
	else{
		MessageBox(TEXT("�ڴ�������"), TEXT("����"));
	}

	return;
}


void CMainWnd::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_udp.Tick();

	CDialogEx::OnTimer(nIDEvent);
}

//��ԭʼ��Ϣ��װ�ɱ������ݸ�ʽ��Ȼ����
void CMainWnd::SendNewMsg(CString &msg, SOCKADDR_IN * lpSockAddr, WORD wNum)
{
	int nLen = msg.GetLength()*sizeof(TCHAR)+sizeof(MyData);
	if (nLen > MAX_MSG_SIZE){
		MessageBox(TEXT("���������Ϣ���ݹ����������ضϣ�"), TEXT("��ʾ"));
	}
	auto buf = new char[nLen];
	if (!buf){
		MessageBox(TEXT("�ڴ�������"), TEXT("����"));
		return;
	}
	memset(buf, 0, nLen);

	MyData * p = (MyData *)buf;
	p->operate = TXT;
	p->number = wNum;
	wcscpy_s(p->data, (nLen - sizeof(WORD)* 2) / sizeof(TCHAR), msg);

	m_udp.SendNewMsg(buf, nLen, lpSockAddr, sizeof(SOCKADDR_IN));
}


void CMainWnd::ShowNewMsg(LPCTSTR lpszMsg, SOCKADDR_IN * lpFromSockAddr, bool bIsSysMsg)
{
	auto p = GetMsgDlg(lpFromSockAddr);

	if (!p){
		MessageBox(TEXT("���յ�δ֪��Ա����Ϣ��"));
		return;
	}

	if (bIsSysMsg)
		p->AddSysMsg(lpszMsg);
	else
		p->AddNewMsg(lpszMsg);
}


CMsgDlg* CMainWnd::GetMsgDlg(SOCKADDR_IN* lpFromSockAddr)
{
	CListBox *pwnd = (CListBox *)GetDlgItem(IDC_MEMLIST);
	int count = pwnd->GetCount();

	while (count){
		auto p = (ItemData *)pwnd->GetItemDataPtr(--count);
		if (p && p->m_addr->sin_port == lpFromSockAddr->sin_port&&
			p->m_addr->sin_addr.s_addr == lpFromSockAddr->sin_addr.s_addr){
			if (!p->m_pdlg){
				p->m_pdlg = new CMsgDlg;
				if (p->m_pdlg){
					CString string;
					pwnd->GetText(count, string);
					p->m_pdlg->m_strTitle = string;
					p->m_pdlg->m_pdata = p;
					p->m_pdlg->Create(IDD_MSGDLG);
					p->m_pdlg->ShowWindow(SW_SHOW);
				}
				else{
					MessageBox(TEXT("�ڴ�������"), TEXT("����"));
				}
			}
			return p->m_pdlg;
		}
	}
	return NULL;
}


void CMainWnd::OnOK()
{
	// TODO:  �ڴ����ר�ô����/����û���

	//CDialogEx::OnOK();
}
