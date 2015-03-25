// VoiceServer.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "VoiceSocket.h"
#include "MsgDlg.h"

// CVoiceServer

CVoiceServer::CVoiceServer(CMsgDlg *pdlg)
: pwnd(pdlg)
{
}

CVoiceServer::~CVoiceServer()
{
}


// CVoiceServer ��Ա����


void CVoiceServer::OnAccept(int nErrorCode)
{
	// TODO:  �ڴ����ר�ô����/����û���
	pwnd->ConnectForVoice();
	CAsyncSocket::OnAccept(nErrorCode);
}


//CVoiceClient����ʵ��

CVoiceClient::CVoiceClient(CMsgDlg *pdlg)
:pwnd(pdlg)
{

}


CVoiceClient::~CVoiceClient()
{

}

void CVoiceClient::OnClose(int nErrorCode)
{
	// TODO:  �ڴ����ר�ô����/����û���
	//pwnd->MessageBox(TEXT("���ӹر�"));
	pwnd->EndOfVoice();
	CAsyncSocket::OnClose(nErrorCode);
}


void CVoiceClient::OnConnect(int nErrorCode)
{
	// TODO:  �ڴ����ר�ô����/����û���
	pwnd->ConnectIsBulided();
	CAsyncSocket::OnConnect(nErrorCode);
}


void CVoiceClient::OnReceive(int nErrorCode)
{
	// TODO:  �ڴ����ר�ô����/����û���
	pwnd->SetWindowTextW(TEXT("�ѽ��յ�����"));

	if (!pwnd->m_sound.m_RecvList.IsEmpty()){
		auto p2 = pwnd->m_sound.m_RecvList.RemoveHead();
		waveOutWrite(pwnd->m_sound.m_hWaveOut, p2, sizeof(WAVEHDR));
	}

	pwnd->RecvSound();
	CAsyncSocket::OnReceive(nErrorCode);
}

