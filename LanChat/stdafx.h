
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��
#include <afxsock.h>

#include <mmsystem.h>//wave֧��ͷ�ļ�
#pragma comment(lib,"winmm.lib")


//��Ϣ���ߴ�
#define MAX_MSG_SIZE		512
//����ͳ���ʱ��:600�루10���ӣ�
#define MAX_MSG_TIME		600
//���ͽ����ļ����У��û�������С
#define MAX_BUF_SIZE	1024*1024
//�����ļ�ʱ�õ��ľ������
#define FILE_SEND			1	//�������ļ�
#define FILE_ACCEPT			2	//���շ��͵��ļ�
#define FILE_REFUSE			3	//�ܾ�����
#define FILE_CANCEL			4	//ȡ���ļ��ķ��ͻ����

//������
enum Operate
{
	OVR,//��������
	CIN,//����
	ONL,//����
	QUT,//����
	TXT,//��������
	ACK,//ȷ�ϰ�
	FIL,//�����ļ�
	FLD,//�ļ���
	VOX//��������
};
//#define CIN				0x0000	//����
//#define QUT				0x0001	//����
//#define TXT				0x0002	//������������
//#define ACK				0x0003	//ȷ�ϰ�
//#define FIL				0x0004	//�������ļ����У�
//#define VOX				0x0005	//������������

//MyUdp�����ݽṹ
struct MyData
{
	WORD operate;
	WORD number;
	TCHAR data[1];
};

//�����ļ�ʱ�õ����ļ���Ϣ�ṹ
struct MyFileInfo
{
	WORD subOper;
	ULONGLONG fileSize;
	TCHAR fileName[1];
};

//���ļ���Сת�����ʺ������Ķ��ĸ�ʽ
CString GetSizeString(ULONGLONG &ullSize);
//�����ļ��̺߳���
UINT SendFileFunc(LPVOID pParam);
//�����ļ��̺߳���
UINT RecvFileFunc(LPVOID pParam);
//�����ļ����̺߳���
UINT SendFolderFunc(LPVOID pParam);
//�����ļ����̺߳���
UINT RecvFolderFunc(LPVOID pParam);





#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


