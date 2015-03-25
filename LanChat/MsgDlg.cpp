// MsgDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "resource.h"
#include "MsgDlg.h"
#include "afxdialogex.h"
#include "MainWnd.h"
#include "FolderDlg.h"



//�б�Ԫ�����ݽṹ
ItemData::ItemData()
: m_addr(nullptr)
, m_pdlg(nullptr)
{
}

ItemData::ItemData(SOCKADDR_IN *addr)
: m_addr(addr)
, m_pdlg(nullptr)
{
}

ItemData::~ItemData()
{
	if (m_addr)
		delete m_addr;
	if (m_pdlg)
		m_pdlg->DestroyWindow();
}


// CMsgDlg �Ի���

IMPLEMENT_DYNAMIC(CMsgDlg, CDialogEx)

CMsgDlg::CMsgDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CMsgDlg::IDD, pParent)
, m_pdata(nullptr)
, m_strTitle(_T(""))
, m_wNum(0)
, m_wLastRecvNum(0)
, m_bIsItOnLine(true)
, m_bCancel(false)
, m_strRecvFolder(_T(""))
, m_bChatInVoice(false)
, m_vc(this)
, m_vs(this)
, m_bInFileTrans(false)
{

}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMsgDlg, CDialogEx)
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_SENDMSG, &CMsgDlg::OnClickedSendmsg)
	ON_BN_CLICKED(IDC_SENDFILE, &CMsgDlg::OnClickedSendfile)
	ON_BN_CLICKED(IDC_CANCEL, &CMsgDlg::OnClickedCancel)
	ON_BN_CLICKED(IDC_SENDFOLDER, &CMsgDlg::OnBnClickedSendfolder)
	ON_BN_CLICKED(IDC_VOICE, &CMsgDlg::OnClickedVoice)
	ON_MESSAGE(MM_WIM_OPEN, &CMsgDlg::OnWimOpen)
	ON_MESSAGE(MM_WIM_DATA, &CMsgDlg::OnWimData)
	ON_MESSAGE(MM_WIM_CLOSE, &CMsgDlg::OnWinClose)
	ON_MESSAGE(MM_WOM_OPEN, &CMsgDlg::OnWomOpen)
	ON_MESSAGE(MM_WOM_DONE, &CMsgDlg::OnWomDone)
	ON_MESSAGE(MM_WOM_CLOSE, &CMsgDlg::OnWomClose)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CMsgDlg ��Ϣ�������


BOOL CMsgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CRect rect;
	GetClientRect(&rect);
	int cx = ::GetSystemMetrics(SM_CXSCREEN);
	int cy = ::GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(NULL, (cx - rect.Width()) / 2 - 45,
		(cy - rect.Height()) / 2 - 45, 0, 0, SWP_NOSIZE);

	SetWindowText(m_strTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CMsgDlg::PostNcDestroy()
{
	// TODO:  �ڴ����ר�ô����/����û���
	CDialogEx::PostNcDestroy();
	m_pdata->m_pdlg = nullptr;
	delete this;

}


void CMsgDlg::OnCancel()
{
	// TODO:  �ڴ����ר�ô����/����û���

	//���ڴ����ļ����У�����������ʱ��Ҫѯ���û��Ƿ�ȷ��Ҫ�رմ���
	if (m_bChatInVoice){
		if (MessageBox(TEXT("�����������죬�رմ��ڻ�����������죬ȷ��Ҫ�ر���"),
			TEXT("�رմ���"), MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
	}
	if (m_bInFileTrans){
		if (MessageBox(TEXT("���ڴ����ļ����رմ��ڻ��ж��ļ����ͣ�ȷ��Ҫ�ر���"),
			TEXT("�رմ���"), MB_YESNO | MB_ICONQUESTION) != IDYES)
			return;
	}
	DestroyWindow();
}


void CMsgDlg::OnClickedSendmsg()
{
	CString string;
	auto pwnd = (CEdit*)GetDlgItem(IDC_EDITMSG);
	pwnd->GetWindowTextW(string);
	if (string.IsEmpty()){
		SetTimer(TIPS_TIMER, 3000, NULL);
		GetDlgItem(IDC_WARNING)->SetWindowText(TEXT("���ܷ��Ϳ���Ϣ��"));
		return;
	}
	if (!m_bIsItOnLine){
		SetTimer(TIPS_TIMER, 3000, NULL);
		GetDlgItem(IDC_WARNING)->SetWindowText(
			TEXT("�Է������ߣ������޷��յ�������Ϣ��"));
	}
	CMainWnd *pMainWnd = (CMainWnd *)AfxGetMainWnd();
	pwnd->SetWindowText(TEXT(""));
	AddNewMsg(string, pMainWnd->m_strName);
	//������Ϣ
	pMainWnd->SendNewMsg(string, m_pdata->m_addr, ++m_wNum);
}


void CMsgDlg::AddNewMsg(LPCTSTR lpszMsg, LPCTSTR lpszFromName)
{
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	CString string, string1;
	string.Format(TEXT("%s %d:%0.2d\r\n%s\r\n\r\n"), lpszFromName ? lpszFromName : m_strTitle,
		tm.wHour, tm.wMinute, lpszMsg);

	auto pedit = (CEdit*)GetDlgItem(IDC_SHOWMSG);

	pedit->GetWindowText(string1);
	string1 += string;
	pedit->SetWindowText(string1);
	pedit->LineScroll(pedit->GetLineCount());//�����������ӵ��ı���

	return;
}


// ״̬���ĺ�������������Ϊ������true������false
void CMsgDlg::StatusChange(bool bEnter)
{
	m_bIsItOnLine = bEnter;
	if (m_bIsItOnLine)
		SetWindowText(m_strTitle);
	else
		SetWindowText(m_strTitle + TEXT("�����ߣ�"));
}


HBRUSH CMsgDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (pWnd->GetDlgCtrlID() == IDC_WARNING){
		pDC->SetTextColor(RGB(255, 0, 0));
		pDC->SetBkMode(TRANSPARENT);
	}
	else if (pWnd->GetDlgCtrlID() == IDC_FILESIZE){
		//pDC->SetBkMode(TRANSPARENT);
		//return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


void CMsgDlg::OnClickedSendfile()
{
	auto btnSendFile = (CButton*)GetDlgItem(IDC_SENDFILE);
	auto btnSendFolder = (CButton*)GetDlgItem(IDC_SENDFOLDER);

	CFileDialog filedlg(TRUE, nullptr, nullptr,
		/*OFN_ALLOWMULTISELECT |*/ OFN_FILEMUSTEXIST | OFN_HIDEREADONLY);
	if (filedlg.DoModal() != IDOK)
		return;

	CString filePath, fileName;
	auto pos = filedlg.GetStartPosition();
	while (pos){
		filePath = filedlg.GetNextPathName(pos);
		fileName = filePath.Mid(filePath.ReverseFind(TEXT('\\')) + 1);

		CFile file;
		if (!file.Open(filePath, CFile::modeRead)){
			MessageBox(TEXT("��ȡ�ļ�ʧ�ܣ�"));
			return;
		}
		auto fileLen = file.GetLength();

		m_bCancel = false;
		SendMsgOfTrans(FIL, fileLen, fileName);
		file.Close();

		m_mapSendFile[fileName] = filePath;
		btnSendFile->EnableWindow(FALSE);
		btnSendFolder->EnableWindow(FALSE);
	}
}


void CMsgDlg::RecvFile(MyData* lpData, int nDataLen)
{
	auto btnSendFile = (CButton*)GetDlgItem(IDC_SENDFILE);
	auto btnSendFolder = (CButton*)GetDlgItem(IDC_SENDFOLDER);
	btnSendFile->EnableWindow(FALSE);
	btnSendFolder->EnableWindow(FALSE);
	auto pMainWnd = (CMainWnd*)AfxGetMainWnd();

	auto lpFileInfo = (MyFileInfo*)lpData->data;
	lpFileInfo->subOper = FILE_REFUSE;

	CString string;
	if (lpData->operate == FIL)//�ļ�
		string.Format(TEXT("%s�����ļ���\n���ƣ�%s\n��С��%s\n�Ƿ���գ�"),
		m_strTitle, lpFileInfo->fileName, GetSizeString(lpFileInfo->fileSize));
	else //�ļ���
		string.Format(TEXT("%s�����ļ��У�\n���ƣ�%s\n��С��%s\n�Ƿ���գ�"),
		m_strTitle, lpFileInfo->fileName, GetSizeString(lpFileInfo->fileSize));
	int result = MessageBox(string, TEXT("�������ļ�"), MB_YESNO | MB_ICONINFORMATION);

	if (result == IDYES){
		//���շ����ļ�
		CString filePathName;
		if (lpData->operate == FIL){
			CFileDialog dlg(FALSE, nullptr, lpFileInfo->fileName);
			if (dlg.DoModal() != IDOK)
				goto o3;
			filePathName = dlg.GetPathName();
		}
		else{
			CFolderDlg dlg;
			if (!dlg.DoModal())
				goto o3;
			filePathName = dlg.GetPathName();
			if (filePathName.Right(1) != TEXT("\\"))
				filePathName += TEXT("\\");
			filePathName += lpFileInfo->fileName;
			if (!CreateDirectory(filePathName, nullptr)){
				MessageBox(TEXT("�����ļ���ʧ��"));
				goto o3;
			}
		}

		auto arg = new ThreadArg;
		arg->dlg = this;
		arg->ullFileSize = lpFileInfo->fileSize;
		arg->fileName = filePathName;

		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == INVALID_SOCKET){
			MessageBox(TEXT("�����׽���ʧ��"));
			goto o2;
		}
		SOCKADDR_IN localaddr;
		localaddr.sin_family = AF_INET;
		localaddr.sin_port = 0;
		localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(s, (SOCKADDR*)&localaddr, sizeof(localaddr)) == SOCKET_ERROR){
			MessageBox(TEXT("��ʧ��"));
			goto o1;
		}
		SOCKADDR_IN addr;
		int nAddrLen = sizeof(addr);
		if (getsockname(s, (SOCKADDR*)&addr, &nAddrLen) == SOCKET_ERROR){
			MessageBox(TEXT("��ȡ�˿ڳ���"));
			goto o1;
		}
		arg->port = addr.sin_port;

		if (listen(s, 1) == SOCKET_ERROR){
			MessageBox(TEXT("����ʧ�ܣ�"));
			goto o1;
		}

		lpFileInfo->subOper = FILE_ACCEPT;
		lpFileInfo->fileSize = addr.sin_port;
		pMainWnd->m_udp.SendNewMsg(lpData, nDataLen, m_pdata->m_addr, sizeof(SOCKADDR_IN));

		arg->s = s;
		m_bCancel = false;
		m_bInFileTrans = true;
		if (lpData->operate == FIL)
			AfxBeginThread(RecvFileFunc, arg);
		else
			AfxBeginThread(RecvFolderFunc, arg);

		return;

	o1:
		closesocket(s);
	o2:
		delete arg;
	}
	//�ܾ������ļ�
o3:
	btnSendFile->EnableWindow();
	btnSendFolder->EnableWindow();
	pMainWnd->m_udp.SendNewMsg(lpData, nDataLen, m_pdata->m_addr, sizeof(SOCKADDR_IN));

	return;
}


void CMsgDlg::MsgOfTrans(MyData* lpData, int nDataLen)
{
	auto lpFileInfo = (MyFileInfo*)lpData->data;
	switch (lpFileInfo->subOper)
	{
	case FILE_SEND:
		if (lpData->operate == VOX)
			AskForVoice(lpData, nDataLen);
		else
			RecvFile(lpData, nDataLen);
		break;
	case FILE_ACCEPT:
	case FILE_REFUSE:
		if (lpData->operate == VOX)
			AcceptVoice(lpData);
		else
			SendFile(lpData);
		break;
	case FILE_CANCEL:
		if (lpData->operate == VOX)
			m_vs.Close();
		else
			CancelTransFile();
	default://����
		delete[] lpData;
		break;
	}
}


void CMsgDlg::SendFile(MyData* lpData)
{
	auto lpFileInfo = (MyFileInfo*)lpData->data;

	auto ps1 = (CStatic*)GetDlgItem(IDC_FILESIZE);
	auto btnCancel = (CButton*)GetDlgItem(IDC_CANCEL);

	if (lpFileInfo->subOper == FILE_ACCEPT){
		auto arg = new ThreadArg;
		arg->dlg = this;
		arg->port = (USHORT)lpFileInfo->fileSize;
		arg->fileName = lpFileInfo->fileName;

		m_bInFileTrans = true;
		if (lpData->operate == FIL)
			AfxBeginThread(SendFileFunc, arg);
		else
			AfxBeginThread(SendFolderFunc, arg);
	}
	else{
		CString string;
		string.Format(TEXT("�Է��ܾ������ļ�%s:%s"),
			lpData->operate == FLD ? TEXT("��") : TEXT(""), lpFileInfo->fileName);
		AddSysMsg(string);
		Reinstatement();

		m_mapSendFile.RemoveKey(lpFileInfo->fileName);
	}
	delete[] lpData;
}


void CMsgDlg::CancelTransFile()
{
	m_bCancel = true;
}


void CMsgDlg::OnClickedCancel()
{
	auto ps1 = (CStatic*)GetDlgItem(IDC_FILESIZE);
	auto pprog = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pprog->ShowWindow(SW_HIDE);
	ps1->ShowWindow(SW_SHOW);
	ps1->SetWindowTextW(TEXT("����ȡ��..."));

	int nLen = sizeof(MyData)+sizeof(MyFileInfo);
	char *pbuf = new char[nLen];
	memset(pbuf, 0, nLen);

	auto p = (MyData*)pbuf;
	p->operate = FIL;
	p->number = ++m_wNum;
	auto pf = (MyFileInfo*)p->data;
	pf->subOper = FILE_CANCEL;

	auto pMainWnd = (CMainWnd*)AfxGetMainWnd();
	pMainWnd->m_udp.SendNewMsg(pbuf, nLen, m_pdata->m_addr, sizeof(SOCKADDR_IN));

	CancelTransFile();
	return;
}


void CMsgDlg::OnBnClickedSendfolder()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	auto btnSendFile = (CButton*)GetDlgItem(IDC_SENDFILE);
	auto btnSendFolder = (CButton*)GetDlgItem(IDC_SENDFOLDER);

	CFolderDlg dlg;
	if (!dlg.DoModal())
		return;
	CString folderPath = dlg.GetPathName();
	CString folderName = folderPath.Mid(folderPath.ReverseFind(TEXT('\\')) + 1);
	//��ȡ�ļ��д�С
	auto ullLength = GetDirectorySize(folderPath);
	SendMsgOfTrans(FLD, ullLength, folderName);
	m_mapSendFile[folderName] = folderPath;
	btnSendFile->EnableWindow(FALSE);
	btnSendFolder->EnableWindow(FALSE);
}


void CMsgDlg::SendMsgOfTrans(Operate ope, ULONGLONG &ullLength, CString &name)
{
	auto ps1 = (CStatic*)GetDlgItem(IDC_FILENAME);
	auto ps2 = (CStatic*)GetDlgItem(IDC_FILESIZE);
	auto ps3 = (CStatic*)GetDlgItem(IDC_PROGTEXT);
	auto btnCancel = (CButton*)GetDlgItem(IDC_CANCEL);
	auto btnSendFile = (CButton*)GetDlgItem(IDC_SENDFILE);
	auto btnSendFolder = (CButton*)GetDlgItem(IDC_SENDFOLDER);
	auto pprog = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	ps1->SetWindowTextW(name);
	ps2->SetWindowTextW(TEXT("�ȴ��Է�����..."));
	ps3->SetWindowTextW(GetSizeString(ullLength));
	btnCancel->ShowWindow(SW_SHOW);
	btnCancel->EnableWindow();
	btnSendFile->EnableWindow(FALSE);
	btnSendFolder->EnableWindow(FALSE);

	m_bCancel = false;

	int nLen = sizeof(MyData)+sizeof(MyFileInfo)+(name.GetLength() - 1)*sizeof(TCHAR);
	char *pbuf = new char[nLen];
	memset(pbuf, 0, nLen);

	auto p1 = (MyData*)pbuf;
	p1->operate = ope;
	p1->number = ++m_wNum;
	auto p2 = (MyFileInfo*)p1->data;
	p2->subOper = FILE_SEND;
	p2->fileSize = ullLength;
	int nSizeInWord = name.GetLength() + 1;
	wcscpy_s(p2->fileName, nSizeInWord, name);

	auto pMainWnd = (CMainWnd*)AfxGetMainWnd();
	pMainWnd->m_udp.SendNewMsg(pbuf, nLen, m_pdata->m_addr, sizeof(SOCKADDR_IN));
}


void CMsgDlg::Reinstatement()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	auto ps1 = (CStatic*)GetDlgItem(IDC_FILENAME);
	auto ps2 = (CStatic*)GetDlgItem(IDC_FILESIZE);
	auto ps3 = (CStatic*)GetDlgItem(IDC_PROGTEXT);
	auto btnCancel = (CButton*)GetDlgItem(IDC_CANCEL);
	auto btnSendFile = (CButton*)GetDlgItem(IDC_SENDFILE);
	auto btnSendFolder = (CButton*)GetDlgItem(IDC_SENDFOLDER);
	auto pprog = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);

	ps1->SetWindowTextW(TEXT(""));
	ps2->SetWindowTextW(TEXT(""));
	ps3->SetWindowTextW(TEXT(""));
	btnCancel->ShowWindow(SW_HIDE);
	pprog->ShowWindow(SW_HIDE);
	btnSendFile->EnableWindow();
	btnSendFolder->EnableWindow();
}


void CMsgDlg::AddSysMsg(LPCTSTR lpszMsg)
{
	CString string, string1;
	string.Format(TEXT("ϵͳ��Ϣ��\r\n%s\r\n\r\n"), lpszMsg);

	auto pedit = (CEdit*)GetDlgItem(IDC_SHOWMSG);

	pedit->GetWindowText(string1);
	string1 += string;
	pedit->SetWindowText(string1);
	pedit->LineScroll(pedit->GetLineCount());//�����������ӵ��ı���

	return;
}


void CMsgDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == TIPS_TIMER){
		KillTimer(TIPS_TIMER);
		auto p = (CStatic*)GetDlgItem(IDC_WARNING);
		p->SetWindowTextW(TEXT(""));
	}
	else{
		auto p = (CStatic*)GetDlgItem(IDC_FILESIZE);
		CString string;
		SYSTEMTIME stm;
		GetLocalTime(&stm);
		string.Format(TEXT("%d:%0.2d:%0.2d"), stm.wHour - m_timeBegin.wHour,
			stm.wMinute - m_timeBegin.wMinute, stm.wSecond - m_timeBegin.wSecond);
		p->SetWindowTextW(string);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CMsgDlg::OnClickedVoice()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	auto ps1 = (CStatic*)GetDlgItem(IDC_FILENAME);
	auto ps2 = (CStatic*)GetDlgItem(IDC_FILESIZE);
	auto btnVoice = (CButton*)GetDlgItem(IDC_VOICE);
	btnVoice->EnableWindow(FALSE);

	if (m_bChatInVoice){
		EndOfVoice();

	}
	else{
		ps1->SetWindowTextW(TEXT("��������"));
		ps2->SetWindowTextW(TEXT("�ȴ��Է�����..."));

		SendVoiceMsg(FILE_SEND);
	}

}


void CMsgDlg::AskForVoice(MyData* lpData, int nDataLen)
{
	auto pMainWnd = (CMainWnd*)AfxGetMainWnd();
	auto pinfo = (MyFileInfo*)lpData->data;

	CString string;
	string.Format(TEXT("%s���������������죬�Ƿ���ܣ�"), m_strTitle);
	if (MessageBox(string, m_strTitle, MB_YESNO | MB_ICONINFORMATION) == IDYES){
		if (m_vs.Create() && m_vs.Listen(1))
		{
			SOCKADDR_IN addr;
			int nLen = sizeof(addr);
			m_vs.GetSockName((SOCKADDR*)&addr, &nLen);
			pinfo->subOper = FILE_ACCEPT;
			pinfo->fileSize = addr.sin_port;
			pMainWnd->m_udp.SendNewMsg(lpData, nDataLen, m_pdata->m_addr, sizeof(SOCKADDR_IN));

			auto ps1 = (CStatic*)GetDlgItem(IDC_FILENAME);
			auto ps2 = (CStatic*)GetDlgItem(IDC_FILESIZE);
			auto btnVoice = (CButton*)GetDlgItem(IDC_VOICE);
			ps1->SetWindowTextW(TEXT("��������"));
			ps2->SetWindowTextW(TEXT("���ڽ�������..."));
			btnVoice->EnableWindow(FALSE);

			return;
		}
	}
	pinfo->subOper = FILE_REFUSE;
	pMainWnd->m_udp.SendNewMsg(lpData, nDataLen, m_pdata->m_addr, sizeof(SOCKADDR_IN));
}


void CMsgDlg::AcceptVoice(MyData *lpData)
{
	auto pinfo = (MyFileInfo*)lpData->data;
	auto ps1 = (CStatic*)GetDlgItem(IDC_FILENAME);
	auto ps2 = (CStatic*)GetDlgItem(IDC_FILESIZE);
	auto btnVoice = (CButton*)GetDlgItem(IDC_VOICE);

	if (pinfo->subOper == FILE_ACCEPT){
		m_vc.Create();
		SOCKADDR_IN addr;
		addr.sin_family = AF_INET;
		addr.sin_port = (UINT)pinfo->fileSize;
		addr.sin_addr = m_pdata->m_addr->sin_addr;
		if (!m_vc.Connect((SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) &&
			m_vc.GetLastError() != WSAEWOULDBLOCK){
			m_vc.Close();
			MessageBox(TEXT("����ʱ����"));
			//���Է�����ȡ��,�Թرռ����׽���
			SendVoiceMsg(FILE_CANCEL);
		}
	}
	else{
		AddSysMsg(TEXT("�Է��ܾ��������죡"));
		btnVoice->EnableWindow();
		Reinstatement();
	}
	delete[] lpData;
}

void CMsgDlg::ConnectIsBulided()
{
	auto ps1 = (CStatic*)GetDlgItem(IDC_FILENAME);
	auto ps2 = (CStatic*)GetDlgItem(IDC_FILESIZE);
	auto btnVoice = (CButton*)GetDlgItem(IDC_VOICE);
	ps2->SetWindowTextW(TEXT("�����ѽ���"));
	m_sound.Initialize(this);
	//��ʼ��ʧ��ʱȡ����������
	//SendVoiceMsg(FILE_CANCEL);
	m_bChatInVoice = true;
	ps1->SetWindowTextW(TEXT("������������"));
	btnVoice->SetWindowTextW(TEXT("��������"));
	btnVoice->EnableWindow();
	SetTimer(VOICE_TIMER, 1000, NULL);
	GetLocalTime(&m_timeBegin);
}

//���ܵ�����ʱ����
void CMsgDlg::ConnectForVoice()
{
	m_vs.Accept(m_vc);
	m_vs.Close();

	auto ps1 = (CStatic*)GetDlgItem(IDC_FILENAME);
	auto ps2 = (CStatic*)GetDlgItem(IDC_FILESIZE);
	auto btnVoice = (CButton*)GetDlgItem(IDC_VOICE);
	ps2->SetWindowTextW(TEXT("�����ѽ���"));
	m_sound.Initialize(this);
	m_bChatInVoice = true;
	ps1->SetWindowTextW(TEXT("������������"));
	btnVoice->SetWindowTextW(TEXT("��������"));
	btnVoice->EnableWindow();
	SetTimer(VOICE_TIMER, 1000, NULL);
	GetLocalTime(&m_timeBegin);
}


LRESULT CMsgDlg::OnWimOpen(WPARAM wParam, LPARAM lParam)
{
	m_sound.BeginRecord();
	waveInStart(m_sound.m_hWaveIn);
	return TRUE;
}


LRESULT CMsgDlg::OnWimData(WPARAM wParam, LPARAM lParam)
{
	if (!m_bChatInVoice){
		waveInClose(m_sound.m_hWaveIn);
		return TRUE;
	}
	auto p = (PWAVEHDR)lParam;
	int ret = m_vc.Send(p->lpData, p->dwBytesRecorded);
	if (ret == SOCKET_ERROR && m_vc.GetLastError() != WSAEWOULDBLOCK){
		MessageBox(TEXT("������Ƶ����"));
	}
	waveInAddBuffer(m_sound.m_hWaveIn, p, sizeof(WAVEHDR));
	return TRUE;
}


LRESULT CMsgDlg::OnWinClose(WPARAM wParam, LPARAM lParam)
{
	m_sound.CloseRecord();
	return TRUE;
}


LRESULT CMsgDlg::OnWomOpen(WPARAM wParam, LPARAM lParam)
{
	SetWindowText(TEXT("����豸��"));
	for (int i = 0; i < SOUND_BUF_NUM; ++i)
		m_sound.m_RecvList.AddTail(m_sound.m_pWaveHdrOut[i]);

	RecvSound();
	return TRUE;
}


LRESULT CMsgDlg::OnWomDone(WPARAM wParam, LPARAM lParam)
{
	if (!m_bChatInVoice){
		waveOutClose(m_sound.m_hWaveOut);
		return TRUE;
	}

	auto p1 = (PWAVEHDR)lParam;
	m_sound.m_RecvList.AddTail(p1);
	RecvSound();
	return TRUE;
}


LRESULT CMsgDlg::OnWomClose(WPARAM wParam, LPARAM lParam)
{
	m_sound.ClosePlay();
	return TRUE;
}

void CMsgDlg::RecvSound()
{
	auto pos = m_sound.m_RecvList.GetHeadPosition();
	while (pos){
		auto curPos = pos;
		auto mem = m_sound.m_RecvList.GetNext(pos);
		int ret = m_vc.Receive(mem->lpData, mem->dwBufferLength);
		if (ret == SOCKET_ERROR && m_vc.GetLastError() != WSAEWOULDBLOCK){
			MessageBox(TEXT("������Ƶ����"));
			return;
		}
		else{
			m_sound.m_RecvList.RemoveAt(curPos);
			waveOutWrite(m_sound.m_hWaveOut, mem, sizeof(WAVEHDR));
		}
	}
}


void CMsgDlg::EndOfVoice()
{
	auto ps2 = (CStatic*)GetDlgItem(IDC_FILESIZE);
	auto btnVoice = (CButton*)GetDlgItem(IDC_VOICE);

	m_bChatInVoice = false;
	KillTimer(VOICE_TIMER);
	waveInReset(m_sound.m_hWaveIn);
	waveOutReset(m_sound.m_hWaveOut);

	CString string;
	ps2->GetWindowTextW(string);
	AddSysMsg(TEXT("�������������ʱ����") + string);
	btnVoice->SetWindowTextW(TEXT("��������"));
	btnVoice->EnableWindow();
	Reinstatement();
	m_vc.Close();
}


void CMsgDlg::OnDestroy()
{
	if (m_bChatInVoice){
		EndOfVoice();
		waveInClose(m_sound.m_hWaveIn);
		waveOutClose(m_sound.m_hWaveOut);
		m_sound.ClosePlay();
		m_sound.CloseRecord();
	}
	if (m_bInFileTrans)
		CancelTransFile();

	CDialogEx::OnDestroy();
}


void CMsgDlg::SendVoiceMsg(WORD wSubOper)
{
	int nLen = sizeof(MyData)+sizeof(MyFileInfo)-sizeof(TCHAR)* 2;
	char *pbuf = new char[nLen];
	memset(pbuf, 0, nLen);

	auto p1 = (MyData*)pbuf;
	p1->operate = VOX;
	p1->number = ++m_wNum;
	auto p2 = (MyFileInfo*)p1->data;
	p2->subOper = wSubOper;

	auto pMainWnd = (CMainWnd*)AfxGetMainWnd();
	pMainWnd->m_udp.SendNewMsg(pbuf, nLen, m_pdata->m_addr, sizeof(SOCKADDR_IN));
}
