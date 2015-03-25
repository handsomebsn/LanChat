// MyUdp.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MyUdp.h"
#include "MainWnd.h"
#include "MsgDlg.h"



Message::Message()
: m_pMsg(nullptr)
, m_nLen(0)
, m_pAddr(nullptr)
, m_nNextTime(1.5)
, m_nCurTime(0.0)
, m_nSumTime(0.0)
{

}

Message::Message(MyData *pdata, int nLen, SOCKADDR_IN *lpSockAddr)
: m_pMsg(pdata)
, m_nLen(nLen)
, m_pAddr(lpSockAddr)
, m_nNextTime(1.5)
, m_nCurTime(0.0)
, m_nSumTime(0.0)
{

}

Message::~Message()
{
	if (m_pMsg)
		delete[](char*)m_pMsg;
}

// CMyUdp

CMyUdp::CMyUdp()
: m_wNum(0)
{
}

CMyUdp::~CMyUdp()
{
}


// CMyUdp ��Ա����


void CMyUdp::OnReceive(int nErrorCode)
{
	// TODO:  �ڴ����ר�ô����/����û���
	char *pbuf = new char[MAX_MSG_SIZE];
	if (!pbuf){
		MessageBox(NULL, TEXT("�ڴ�������"), TEXT("����"), 0);
		return;
	}
	memset(pbuf, 0, MAX_MSG_SIZE);

	SOCKADDR_IN *pnewaddr = new SOCKADDR_IN;
	if (!pnewaddr){
		MessageBox(NULL, TEXT("�ڴ�������"), TEXT("����"), 0);
		delete[] pbuf;
		return;
	}
	int nLen = sizeof(SOCKADDR_IN);
	int nRecvLen;

	nRecvLen = ReceiveFrom(pbuf, MAX_MSG_SIZE, (SOCKADDR *)pnewaddr, &nLen);
	if (nRecvLen == SOCKET_ERROR){
		TRACE(TEXT("��ȡ��Ϣ����%d\n"), WSAGetLastError());
		delete pnewaddr;
		delete[] pbuf;
	}
	else{
		MyData *p = (MyData *)pbuf;
		if (p->operate == ACK){
			//����Ϣ����ɾ��
			auto pos = m_msglist.GetHeadPosition();
			while (pos){
				auto mem = m_msglist.GetAt(pos);
				if (mem->m_pAddr->sin_addr.s_addr == pnewaddr->sin_addr.s_addr &&
					mem->m_pAddr->sin_port == pnewaddr->sin_port &&
					mem->m_pMsg->number == p->number){
					delete mem;
					m_msglist.RemoveAt(pos);
					break;
				}
				m_msglist.GetNext(pos);
			}
			delete[] pbuf;
			delete pnewaddr;
		}
		else{
#pragma message("���һ�����ƣ���ֹ��ʧ����ظ�")

			//���Է�����ACK
			auto prep = new MyData;
			if (prep){
				prep->operate = ACK;
				prep->number = p->number;//���һ�����ƣ���ֹ��ʧ����ظ�
				SendTo(prep, sizeof(MyData), (SOCKADDR*)pnewaddr, sizeof(SOCKADDR_IN));
				delete prep;
			}
			
			CMainWnd *pwnd = (CMainWnd *)AfxGetMainWnd();
			pwnd->NewMsg(p, nRecvLen, pnewaddr);
		}
	}

	CAsyncSocket::OnReceive(nErrorCode);
}


void CMyUdp::OnSend(int nErrorCode)
{
	// TODO:  �ڴ����ר�ô����/����û���

	CAsyncSocket::OnSend(nErrorCode);
}



void CMyUdp::Tick()
{
	auto pos = m_msglist.GetHeadPosition();
	while (pos){
		auto mem = m_msglist.GetAt(pos);
		mem->m_nCurTime += 1.5;
		mem->m_nSumTime += 1.5;
		if (mem->m_nCurTime >= mem->m_nNextTime){
			SendTo(mem->m_pMsg, mem->m_nLen, (SOCKADDR*)mem->m_pAddr, sizeof(SOCKADDR_IN));
			mem->m_nCurTime = 0.0;
			mem->m_nNextTime = min(mem->m_nNextTime * 2, 64);
		}
		if (mem->m_nSumTime > MAX_MSG_TIME){
			//��ʾ�û��ö����ݷ���ʧ��
			CMainWnd *pwnd = (CMainWnd *)AfxGetMainWnd();

			CString string;
			if (mem->m_pMsg->operate == FIL || mem->m_pMsg->operate == FLD){
				auto pinfo = (MyFileInfo*)mem->m_pMsg->data;
				CString opeName;
				switch (pinfo->subOper)
				{
				case FILE_SEND:
				case FILE_ACCEPT:
					opeName = TEXT("����");
					break;
				default:
					break;
				}
				if (!opeName.IsEmpty()){
					string.Format(TEXT("%s%s\"%s\"ʧ�ܣ�"), opeName,
						mem->m_pMsg->operate == FIL ? TEXT("�ļ�") : TEXT("�ļ���"),
						pinfo->fileName);
					pwnd->ShowNewMsg(string, mem->m_pAddr, true);
				}
					
				auto pdlg = pwnd->GetMsgDlg(mem->m_pAddr);
				pdlg->Reinstatement();
			}
			else if (mem->m_pMsg->operate == TXT){
				string.Format(TEXT("%s\r\n����ʧ�ܣ�"), mem->m_pMsg->data);
				pwnd->ShowNewMsg(string, mem->m_pAddr, true);
			}

			auto temp = pos;
			delete mem;
			m_msglist.GetNext(pos);
			m_msglist.RemoveAt(temp);
		}
		else{
			m_msglist.GetNext(pos);
		}
	}
}


int CMyUdp::SendNewMsg(void * lpBuf, int nLength, SOCKADDR_IN * lpSockAddr, int nSockAddrLen)
{
	auto p = new Message((MyData*)lpBuf, nLength, lpSockAddr);
	if (!p){
		MessageBox(NULL, TEXT("�ڴ�������"), TEXT("����"), 0);
		return -1;
	}
	m_msglist.AddTail(p);

	return SendTo(lpBuf, nLength, (SOCKADDR*)lpSockAddr, nSockAddrLen);
}


void CMyUdp::ClearMsgList()
{
	auto pos = m_msglist.GetHeadPosition();
	while (pos){
		delete m_msglist.GetNext(pos);
	}

	m_msglist.RemoveAll();
}
