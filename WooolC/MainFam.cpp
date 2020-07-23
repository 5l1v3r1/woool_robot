#include "MainFrm.h"
#include "framework.h"
#include <iostream>
#include "pch.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()


CMainFrame::CMainFrame() noexcept
	: m_SID(_T(""))
	, m_CharacterName(_T(""))
	, m_pSocket(NULL)
	, m_IsMoveRun(false)
	, m_MapCaption(_T(""))
	, m_MapFilename(_T(""))
	, m_DStarArray(NULL)
	, m_DStarWidth(0)
	, m_DStarHeight(0)
{
	m_account = "srauni";
	m_password = "123456";
	m_server_host = "192.168.1.107";
	m_server_port = 7000;
	m_server_name = "��ϼ";
	m_game_path = "C:\\Program Files (x86)\\Shanda\\Woool\\";

	this->OnLoginGame();
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// ����һ����ͼ��ռ�ÿ�ܵĹ�����
	if (!m_wndView.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, nullptr))
	{
		TRACE0("δ�ܴ�����ͼ����\n");
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// ������ǰ�Ƶ���ͼ����
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ����ͼ��һ�γ��Ը�����
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// ����ִ��Ĭ�ϴ���
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}




BOOL CMainFrame::OnIdle(LONG lCount)
{
	CString str, str2;
	str.Format("��ǰλ��:%s[%s](%u,%u)", m_MapCaption, m_MapFilename, m_CharSelf.pos_x, m_CharSelf.pos_y);
	if (this->m_DStarArray != NULL)
	{
		POINT current;
		current.x = m_CharSelf.pos_x;
		current.y = m_CharSelf.pos_y;
		long g = m_DStarArray[current.x * m_DStarHeight + current.y];
		str2.Format("ʣ%d��", g);
		if (g > 0)
		{
			str += str2;
		}
	}
	LogSystem(str);
	return TRUE;
}

void CMainFrame::OnLoginGame()
{
	char t[20];
	sprintf(t, "%d", m_server_port);
	LogSystem("�û�ѡ�������:" + m_server_host + ":" + t + "(" + m_server_name + ")", RGB(255, 255, 0));
	CClientSocket* socket = new CClientSocket(this);
	if (!socket->Create())
	{
		LogSystem("�����׽��ֳ���!", RGB(255, 0, 0));
		int error = socket->GetLastError();
		return;
	}
	m_chFlag = '1';
	m_GameStatus = WGS_LOGINGATE;
	socket->Connect(m_server_host, m_server_port);
}

void CMainFrame::OnConnect(int nErrorCode, CClientSocket* socket)
{
	if (socket != m_pSocket)
	{
		delete socket;
		return;
	}
	if (nErrorCode == 0)
	{
		if (m_GameStatus == WGS_LOGINGATE)
		{
			LogSystem("���ӵ�¼�������ɹ�", RGB(0, 255, 255));
			Sleep(500);
			LogSystem("�����˺�/����");
			char buf[STRING_LENGTH];
			MMSG msg;
			ZeroMemory(&msg, sizeof(MMSG));
			sprintf(buf, "%s/%s", m_account, m_password);
			msg.wCmd = 0x7d1;
			socket->SendMsg(&msg, buf);
		}
		else if (m_GameStatus == WGS_SELGATE)
		{
			LogSystem("���ӽ�ɫ���سɹ�", RGB(0, 255, 255));
			Sleep(500);
			char buf[STRING_LENGTH];
			MMSG msg;
			ZeroMemory(&msg, sizeof(MMSG));
			sprintf(buf, "%s/%s", m_account, m_SID);
			LogSystem(buf,RGB(64,255,64));
			msg.wCmd = 0x64;
			socket->SendMsg(&msg, buf);
		}
		else if (m_GameStatus == WGS_GAMEGATE)
		{
			LogSystem("������Ϸ���سɹ�", RGB(0, 255, 255));
			Sleep(500);
			char buf[STRING_LENGTH];
			ZeroMemory(&buf, STRING_LENGTH);
			sprintf(buf, "**%s/%s/%s/20041118/0", m_account, m_CharacterName, m_SID);
			LogSystem(buf);
			socket->SendMsg((MMSG*)buf, buf + 12);
		}
	}
	else
	{
		LogSystem("���ӷ�������,�׽��ֱ����!", RGB(255, 0, 0));
		delete socket;
	}

}

char g_RecvBuf[BUF_LENGTH * 8] = { (char)0 };
void CMainFrame::OnReceive(int nErrorCode, CClientSocket* socket)
{
	if (socket != m_pSocket)
	{
		delete socket;
		return;
	}
	if (nErrorCode != 0)
	{
		LogSystem("�������ݳ���!", RGB(255, 0, 0));
		return;
	}

	char buf[BUF_LENGTH];
	MMSG msg;
	int retval;
	int i;
	int pos = 0;
	int NewStart = 0;
	CString str;

	ZeroMemory(buf, sizeof(buf));
	retval = socket->Receive(buf, BUF_LENGTH);
	buf[retval] = (char)0;
	//LogSystem(buf,RGB(0,255,0));
	strcat(g_RecvBuf, buf);

	ZeroMemory(buf, sizeof(buf));
	NewStart = 0;
	long len = strlen(g_RecvBuf);
	for (i = 0; i < len; i++)
	{
		if (g_RecvBuf[i] == '#')
		{
			pos = 0;
			while (i < len)
			{
				buf[pos] = g_RecvBuf[i];
				if (buf[pos] == '!')
				{
					break;
				}
				//if(i>strlen(m_RecvBuf))break;
				i++;
				pos++;
			}
			if (buf[pos] == '!')
			{
				NewStart = i + 1;
				buf[pos + 1] = (char)0;
				buf[pos] = (char)0;
				if (buf[1] == '+')
				{
					char* token;
					bool isG = false;
					token = strtok(buf, "/");
					if (token != NULL)
					{
						if (token[2] == 'G')
						{
							LogSystem("�����ɹ�", RGB(0, 255, 0));
							isG = true;
						}
						else if ((*(WORD*)(token + 2)) == ((WORD)'FL'))
						{
							LogSystem("����ʧ��");
						}
					}
					token = strtok(NULL, "/");
					if (token != NULL)
					{
						m_CharSelf.pos_x = atol(token);
					}
					token = strtok(NULL, "/");
					if (token != NULL)
					{
						m_CharSelf.pos_y = atol(token);
					}
				}
				else
				{
					ZeroMemory(&msg, sizeof(msg));
					//LogSystem(buf);
					Decode6BitBuf(buf + 1, (char*)&msg, sizeof(msg));
					if (!GameLoop(msg, buf + 16 + 1, socket))
					{
						//LogString(buf,clFuchsia);
					}
				}
			}
		}
		if (g_RecvBuf[i] == '*')
		{
			socket->Send("*", 1);
		}
	}
	strcpy(buf, &g_RecvBuf[NewStart]);
	strcpy(g_RecvBuf, buf);

	return;
}

void CMainFrame::OnClose(int nErrorCode, CClientSocket* socket)
{
	if (socket != m_pSocket)
	{
		delete socket;
		return;
	}
	LogSystem("���ӱ��ر�", RGB(255, 0, 0));
}


CString CMainFrame::ParseActorName(CString name)
{
	int FindAt = name.Find("/");
	if (FindAt != -1)
	{
		name = name.Left(FindAt);
	}
	FindAt = name.Find("\\");
	if (FindAt != -1)
	{
		name = name.Left(FindAt);
	}
	return name;
}


const int xofs_walk[] = { 0, 1, 1, 1, 0, -1, -1, -1 };
const int yofs_walk[] = { -1, -1, 0, 1, 1, 1, 0, -1 };
char dir_string[8][8] = {
	"��","�J","��","�K","��","�L","��","�I" };
void CMainFrame::Walk(int dir)
{
	CString s;
	s.Format("��%s",dir_string[dir]);
	LogSystem(s);
	MMSG msg;
	dir &= 7;
	ZeroMemory(&msg, sizeof(MMSG));
	msg.wa = m_CharSelf.pos_x + xofs_walk[dir];
	msg.wb = m_CharSelf.pos_y + yofs_walk[dir];
	msg.wCmd = 0xbc3;
	msg.w3 = dir;
	if (m_GameMap.TestMap(msg.wa, msg.wb))
		m_pSocket->SendMsg(&msg, "");
	else
		LogSystem("ǰ���޷�ͨ��");
}


void CMainFrame::Run(int dir)
{
	CString s;
	s.Format("��%s",dir_string[dir]);
	LogSystem(s);
	MMSG msg;
	dir &= 7;
	ZeroMemory(&msg, sizeof(MMSG));
	msg.wa = m_CharSelf.pos_x + xofs_walk[dir] * 2;
	msg.wb = m_CharSelf.pos_y + yofs_walk[dir] * 2;
	msg.wCmd = 0xbc5;
	msg.w3 = dir;
	if (m_GameMap.TestMap(msg.wa, msg.wb) && m_GameMap.TestMap(m_CharSelf.pos_x + xofs_walk[dir], m_CharSelf.pos_y + yofs_walk[dir]))
		m_pSocket->SendMsg(&msg, "");
	else
		LogSystem("ǰ���޷�ͨ��");
}


inline void GetTypeName(CString& name, unsigned char type)
{
	switch (type)
	{
	case 0:
		name = "���";
		break;
	case 3:
		name = "����-ʬ��";
		break;
	case 10:
		name = "����-����";
		break;
	case 11:
		name = "����(������)";
		break;
	case 12:
		name = "��ʿ";
		break;
	case 19:
		name = "����";
		break;
	case 50:
		name = "NPC";
		break;
	case 54:
		name = "����-������";
		break;
	case 55:
		name = "����-����";
		break;
	default:
		name.Format("%d", type);
		break;
	}
}

CActor* CMainFrame::GetActorByIndex(DWORD idx)
{
	POSITION pos;
	CActor* pActor;
	pos = m_ActorList.GetHeadPosition();
	while (pos != NULL)
	{
		pActor = (CActor*)m_ActorList.GetAt(pos);
		if (pActor->id == idx)
		{
			return pActor;
		}
		m_ActorList.GetNext(pos);
	}
	return NULL;
}

//��ɫ����
void CMainFrame::ActorAppear(DWORD idx, long x, long y, LPCTSTR name, unsigned char type)
{
	POSITION pos;
	CActor* pActor;
	bool hasName = (strlen(name) > 0);
	int count, i;
	CString str;
	///////////////////////////////////////////////////////////////////////////////////
	// ���ݸ���
	///////////////////////////////////////////////////////////////////////////////////
	pos = m_ActorList.GetHeadPosition();
	while (pos != NULL)//���������ش洢�Ľ�ɫ�б�
	{
		pActor = (CActor*)m_ActorList.GetAt(pos);
		if (pActor->id == idx)//����Ѿ����ھ͸���
		{
			pActor->x = x;
			pActor->y = y;
			if (hasName)pActor->name = name;
			pActor->type = type;
			break;
		}
		m_ActorList.GetNext(pos);
	}
	if (pos == NULL)//��������� ���½�һ�������ɫ�б�
	{
		pActor = new CActor;
		pActor->id = idx;
		pActor->x = x;
		pActor->y = y;
		if (hasName)pActor->name = name;
		pActor->type = type;
		m_ActorList.AddTail(pActor);
	}
}

//��ɫ��ʧ
void CMainFrame::ActorDisappear(DWORD idx)
{
	POSITION pos;
	CActor* pActor;
	int count;
	CString str;
	///////////////////////////////////////////////////////////////////////////////////
	// ���ݸ���
	///////////////////////////////////////////////////////////////////////////////////
	pos = m_ActorList.GetHeadPosition();
	while (pos != NULL)//���������ش洢�Ľ�ɫ�б�
	{
		pActor = (CActor*)m_ActorList.GetAt(pos);
		if (pActor->id == idx)//������ھʹ��б���ɾ��
		{
			m_ActorList.RemoveAt(pos);
			break;
		}
		m_ActorList.GetNext(pos);
	}
}

void CMainFrame::OnDestroy()
{
	if (m_pSocket)
	{
		m_pSocket->Close();
		delete m_pSocket;
	}
	m_pSocket = NULL;
}