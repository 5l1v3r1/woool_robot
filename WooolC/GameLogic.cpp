#include "framework.h"
#include "MainFrm.h"

extern TCHAR g_szGamePath[STRING_LENGTH];
char buf[BUF_LENGTH * 16];
BOOL CMainFrame::GameLoop(const MMSG& msg, const char* data, CClientSocket* socket)
{
	MMSG send_msg;
	CString str, str2, str3;
	CStringList strList;
	int retval = 0;
	char* token;
	switch (msg.wCmd)
	{
		///////////////////////////////////////////////////////////////////////////////////////////////////
		// ������ǽ����¼���غ����Ϣ
		///////////////////////////////////////////////////////////////////////////////////////////////////
	case 0x211:
		LogSystem("�ʺ�/���� ͨ����֤. ���ͷ���������=>" + m_server_name);
		ZeroMemory((char*)&send_msg, sizeof(send_msg));
		send_msg.wCmd = 0x68;
		socket->SendMsg(&send_msg, m_server_name);
		if (msg.wb > 0)
		{
			str.Format("�����˻���ʣ��%dСʱ!", msg.wb);
			LogSystem(str);
		}
		break;
	case 0x1f7:
		socket->Close();
		if (msg.dwFlag == 0xffffffff)
		{
			LogSystem("�������!", RGB(255, 0, 0));
		}
		if (msg.dwFlag == 0)
		{
			LogSystem("���û�������!", RGB(255, 0, 0));
		}
		if (msg.dwFlag == 0xfffffffe)
		{
			LogSystem("����������󣬷������ܾ�!", RGB(255, 0, 0));
		}
		if (msg.dwFlag == 0xfffffffd)
		{
			LogSystem("���û�����ʹ���У����ܱ��߳���������", RGB(255, 0, 0));
		}
		delete socket;
		break;
	case 0x212:
		LogSystem("");

		ZeroMemory(buf, sizeof(buf));
		Decode6BitBuf(data, buf, sizeof(buf));

		token = strtok(buf, "/");
		while (token != NULL)
		{
			str = token;
			if (str.GetLength() > 0)
				break;
			token = strtok(NULL, "/");
		}
		token = strtok(NULL, "/");
		while (token != NULL)
		{
			str2 = token;
			if (str2.GetLength() > 0)
				break;
			token = strtok(NULL, "/");
		}
		token = strtok(NULL, "/");
		while (token != NULL)
		{
			str3 = token;
			if (str3.GetLength() > 0)
				break;
			token = strtok(NULL, "/");
		}
		m_SID = str3;
		LogSystem("�õ���ɫ���ص�ַ:" + str + ":" + str2 + " ��֤��=" + str3, RGB(255, 255, 0));

		{
			CClientSocket* socket = new CClientSocket(this);
			if (!socket->Create())
			{
				LogSystem("�����׽��ֳ���!", RGB(255, 0, 0));
				break;
			}
			LogSystem("��ʼ���ӽ�ɫ����...");
			m_GameStatus = WGS_SELGATE;
			socket->Connect(str, atol(str2));
		}
		break;


		///////////////////////////////////////////////////////////////////////////////////////////////////
		// ������ǽ����ɫ���غ����Ϣ
		///////////////////////////////////////////////////////////////////////////////////////////////////
	case 0x20F:
		LogSystem("��ȡ��ɫ�б�ʧ��!", RGB(255, 0, 0));
		socket->Close();
		delete socket;
		break;
	case 0x208:
		ZeroMemory(buf, sizeof(buf));
		Decode6BitBuf(data, buf, sizeof(buf));
		LogSystem("�õ���ɫ�б�:");
		LogSystem("==================================");
		{
			CString CharInfo[20];
			unsigned int i = 0;
			strList.RemoveAll();
			token = strtok(buf, "/");
			while (token != NULL)
			{
				CharInfo[i++] = token;
				token = strtok(NULL, "/");
			}
			for (i = 0; i < msg.dwFlag; i++)
			{
				LogSystem(CharInfo[5 * i], RGB(192, 192, 192));
			}
			LogSystem("==================================");
			m_CharacterName = CharInfo[5 * 0];
			if (m_CharacterName[0] == '*')
			{
				m_CharacterName = m_CharacterName.Mid(1);
			}
			LogSystem("ʹ�á�" + m_CharacterName + "����¼!");
			ZeroMemory((char*)&send_msg, sizeof(send_msg));
			send_msg.wCmd = 0x67;
			socket->SendMsg(&send_msg, m_account + "/" + m_CharacterName);
		}
		break;
	case 0x20d:
		Sleep(1000);

		LogSystem("");

		ZeroMemory(buf, sizeof(buf));
		Decode6BitBuf(data, buf, sizeof(buf));

		token = strtok(buf, "/");
		while (token != NULL)
		{
			str = token;
			if (str.GetLength() > 0)
				break;
			token = strtok(NULL, "/");
		}
		token = strtok(NULL, "/");
		while (token != NULL)
		{
			str2 = token;
			if (str2.GetLength() > 0)
				break;
			token = strtok(NULL, "/");
		}
		LogSystem("�õ���Ϸ���ص�ַ:" + str + ":" + str2, RGB(255, 255, 0));
		{
			CClientSocket* socket = new CClientSocket(this);
			if (!socket->Create())
			{
				LogSystem("�����׽��ֳ���!", RGB(255, 0, 0));
				break;
			}
			LogSystem("��ʼ������Ϸ����...", RGB(255, 0, 255));
			m_GameStatus = WGS_GAMEGATE;
			socket->Connect(str, atol(str2));
		}
		break;


		///////////////////////////////////////////////////////////////////////////////////////////////////
		// ������ǽ�����Ϸ���غ����Ϣ
		///////////////////////////////////////////////////////////////////////////////////////////////////
	case 0x9:
		if (msg.dwFlag == m_CharSelf.dwServerId)
		{
			str.Format("�������ý�ɫ������%d,%d...", msg.w1, msg.w2);
			LogSystem(str);
		}
		break;
	case 0xa:
	case 0xb:
	case 0xd://��ɫ����
		ZeroMemory(buf, sizeof(buf));
		Decode6BitBuf(data, buf, sizeof(buf));
		ActorAppear(msg.dwFlag, msg.w1, msg.w2, buf + 12, buf[0]);
		break;
	case 0xe:
		/*����
		flag = attacker id
		wcmd = 0x0e
		w1 = x
		w2 = y
		w3 = dir
		*/
		str.Format("%s ����,��(%d,%d)!����%d!%s",
			ParseActorName(GetActorByIndex(msg.dwFlag)->name),
			msg.w1, msg.w2, msg.w3, data);
		LogSystem(str, RGB(128, 128, 255));
		break;
	case 0x1e://��ɫ��ʧ
		ActorDisappear(msg.dwFlag);
		break;
	case 0x1f:
		/*������
		flag = char id
		wcmd = 0x1f
		w1 = cur hp
		w2 = max hp
		w3 = dir
		data = feather [4bytes] +
		0 [4bytes] +
		attacker id[4 bytes]+
		0 [4bytes]
		*/
		ZeroMemory(buf, sizeof(buf));
		retval = Decode6BitBuf(data, buf, sizeof(buf));
		if (msg.dwFlag == m_CharSelf.dwServerId)
		{
			str.Format("%s ������,���%d�˺�,ʣ��%d������,�ܹ�%d������.",
				ParseActorName(GetActorByIndex(*((DWORD*)(buf + 8)))->name),
				msg.w3, msg.w1, msg.w2);
			//OnHpChange(recv_msg.head.w1,recv_msg.head.w2);
			LogSystem(str, RGB(255, 0, 0));
			m_CharSelf.Hp.min = msg.w1;
			m_CharSelf.Hp.max = msg.w2;

			//char strbuf[512];
			//str="";
			//for(int i=0;i<retval;i++)
			//{
			//	sprintf(strbuf,"%02x ",(unsigned char)buf[i]);
			//	str+=strbuf;
			//}
			//LogSystem(str,RGB(0,255,0));
		}
		else
		{
			str2 = "<δ֪>";
			str3 = "<δ֪>";
			if (GetActorByIndex(*((DWORD*)(buf + 8))) != NULL)
				str2 = ParseActorName(GetActorByIndex(*((DWORD*)(buf + 8)))->name);
			if (GetActorByIndex(msg.dwFlag) != NULL)
				str3 = ParseActorName(GetActorByIndex(msg.dwFlag)->name);
			str.Format("%s ���� %s,���%d�˺�,ʣ��%d������,�ܹ�%d������.",
				str2, str3,
				msg.w3, msg.w1, msg.w2);
			LogSystem(str, RGB(255, 128, 128));

			//char strbuf[512];
			//str="";
			//for(int i=0;i<20;i++)
			//{
			//	sprintf(strbuf,"%02x ",(unsigned char)buf[i]);
			//	str+=strbuf;
			//}
			//LogSystem(str,RGB(0,255,0));
		}
		break;
	case 0x22:
		/*����
		wMsgFlag = char id
		wcmd = 0x22
		w1 = x
		w2 = y
		w3 = dir
		data = feather and 0 */
		ZeroMemory(buf, sizeof(buf));
		retval = Decode6BitBuf(data, buf, sizeof(buf));
		if (msg.dwFlag == m_CharSelf.dwServerId)
		{
			str.Format("�㱻ɱ����!%s", data);
			LogSystem(str, RGB(255, 0, 0));
			char strbuf[512];
			str = "";
			for (int i = 0; i < retval; i++)
			{
				sprintf(strbuf, "%02x ", (unsigned char)buf[i]);
				str += strbuf;
			}
			LogSystem(str, RGB(0, 0, 255));
		}
		else
		{
			str.Format("%s����ɱ����!%s", ParseActorName(GetActorByIndex(msg.dwFlag)->name), data);
			LogSystem(str);
			char strbuf[512];
			str = "";
			for (int i = 0; i < retval; i++)
			{
				sprintf(strbuf, "%02x ", (unsigned char)buf[i]);
				str += strbuf;
			}
			LogSystem(str, RGB(0, 0, 255));
		}
		break;
	case 0x2a://��ҽ�ɫ����
		ZeroMemory(buf, sizeof(buf));
		Decode6BitBuf(data, buf, sizeof(buf));
		if (m_CharSelf.dwServerId == msg.dwFlag)
		{
			str.Format("��ҽ�ɫ����[%s]", buf);
			LogSystem(str);
			m_CharSelf.Name = buf;
			//UpdateSelfStatus();
		}
		else
		{
			str.Format("���ֲ�ѯ���0x%x=%s", msg.dwFlag, ParseActorName(buf));
			LogSystem(str);
		}
		break;
	case 0x32:
		str.Format("�ҵ�ID:0x%08x / �����ڵ�ͼ(%d,%d)��.", msg.dwFlag, msg.w1, msg.w2);
		LogSystem(str);
		m_CharSelf.pos_x = msg.w1;
		m_CharSelf.pos_y = msg.w2;
		m_CharSelf.dwServerId = msg.dwFlag;
		break;
	case 0x33:
		ZeroMemory(buf, sizeof(buf));
		Decode6BitBuf(data, buf, sizeof(buf));
		str.Format("�ҵ�ID:0x%08x / �����ڵ�ͼ[%s](%d,%d)��.", msg.dwFlag, buf, msg.w1, msg.w2);
		LogSystem(str);
		m_CharSelf.pos_x = msg.w1;
		m_CharSelf.pos_y = msg.w2;
		m_CharSelf.dwServerId = msg.dwFlag;
		m_MapFilename = buf;
		str.Format("%sData\\map\\%s.nmp", m_game_path, buf);
		m_GameMap.LoadMap(str);
		break;
	case 0x34:
#define pmsginfo ((MMSGSETCHARINFO*)&buf)
		ZeroMemory(buf, sizeof(buf));
		memcpy(buf, &msg, sizeof(MMSG));
		Decode6BitBuf(data, buf + sizeof(MMSG), sizeof(buf));

		m_CharSelf.prof = pmsginfo->bProf;
		m_CharSelf.gold = pmsginfo->dwGold;
		m_CharSelf.level = pmsginfo->bLevel;
		m_CharSelf.AC.hi = pmsginfo->ACHi;
		m_CharSelf.AC.low = pmsginfo->ACLow;
		m_CharSelf.MAC.hi = pmsginfo->MACHi;
		m_CharSelf.MAC.low = pmsginfo->MACLow;
		m_CharSelf.DC.hi = pmsginfo->DCHi;
		m_CharSelf.DC.low = pmsginfo->DCLow;
		m_CharSelf.MC.hi = pmsginfo->MCHi;
		m_CharSelf.MC.low = pmsginfo->MCLow;
		m_CharSelf.SC.hi = pmsginfo->SCHi;
		m_CharSelf.SC.low = pmsginfo->SCLow;
		m_CharSelf.HandWeight.min = pmsginfo->HandWeight;
		m_CharSelf.HandWeight.max = pmsginfo->MaxHandWeight;
		m_CharSelf.BodyWeight.min = pmsginfo->BodyWeight;
		m_CharSelf.BodyWeight.max = pmsginfo->MaxBodyWeight;
		m_CharSelf.BagWeight.min = pmsginfo->BagWeight;
		m_CharSelf.BagWeight.max = pmsginfo->MaxBagWeight;

		m_CharSelf.Hp.min = pmsginfo->Hp;
		m_CharSelf.Hp.max = pmsginfo->MaxHp;
		m_CharSelf.Mp.min = pmsginfo->Mp;
		m_CharSelf.Mp.max = pmsginfo->MaxMp;
		m_CharSelf.Exp.min = pmsginfo->Exp;
		m_CharSelf.Exp.max = pmsginfo->MaxExp;
#undef pmsginfo
		break;
	case 0x35:
		/*ħ����hp�ı�
		dwMsgFlag = char id
		wcmd = 0x35
		w1 = curhp
		w2 = curmp
		w3 = maxhp*/
		if (msg.dwFlag == m_CharSelf.dwServerId)
		{
			str.Format("���� HP:%d/%d MP:%d", msg.w1, msg.w3, msg.w2);
			LogSystem(str);
			m_CharSelf.Hp.min = msg.w1;
			m_CharSelf.Hp.max = msg.w3;
			m_CharSelf.Mp.min = msg.w2;
		}
		else
		{
			if (GetActorByIndex(msg.dwFlag) != NULL)
			{
				str2 = ParseActorName(GetActorByIndex(msg.dwFlag)->name);
			}
			str.Format("%s �� HP:%d/%d MP:%d", str2, msg.w1, msg.w3, msg.w2);
			LogSystem(str);
		}
		//OnHpChange(recv_msg.head.w1,recv_msg.head.w3);
		//OnMpChange(recv_msg.head.w2,0);
		break;
	case 0x36:
		ZeroMemory(buf, sizeof(buf));
		Decode6BitBuf(data, buf, sizeof(buf));
		str.Format("��ǰ������ͼ[%s].", buf);
		LogSystem(str);
		//LogChat(str);
		m_MapCaption = buf;
		//ZeroMemory(&send_msg,sizeof(send_msg));
		//send_msg.head.wCmd=0x409;
		//SendMsg(send_msg,Socket);
		break;
	case 0x28://��ͨ����
	case 0x64://ϵͳ��Ϣ
	case 0x65://�����Ϣ
	case 0x66://��
	case 0x67://˽��
	case 0x68://�л���Ϣ
	{
		BYTE b = msg.w1 & 0xff;
		BYTE f = (msg.w1 & 0xff00) >> 8;
		ZeroMemory(buf, sizeof(buf));
		Decode6BitBuf(data, buf, sizeof(buf));
		LogChat(buf,
			RGB(ColorTable[b * 3], ColorTable[b * 3 + 1], ColorTable[b * 3 + 2]),
			RGB(ColorTable[f * 3], ColorTable[f * 3 + 1], ColorTable[f * 3 + 2]));
	}
	break;
	ZeroMemory(buf, sizeof(buf));
	Decode6BitBuf(data, buf, sizeof(buf));
	LogChat(buf, RGB(0, 0, 0), RGB(255, 0, 0));
	memprintf(buf, (unsigned char*)&msg, sizeof(msg));
	LogSystem(buf, RGB(255, 0, 255));
	break;
	ZeroMemory(buf, sizeof(buf));
	Decode6BitBuf(data, buf, sizeof(buf));
	LogChat(buf, RGB(255, 255, 64));
	break;
	ZeroMemory(buf, sizeof(buf));
	Decode6BitBuf(data, buf, sizeof(buf));
	LogChat(buf, 0, RGB(255, 255, 64));
	break;
	ZeroMemory(buf, sizeof(buf));
	Decode6BitBuf(data, buf, sizeof(buf));
	LogChat(buf, RGB(64, 64, 255));
	break;
	ZeroMemory(buf, sizeof(buf));
	Decode6BitBuf(data, buf, sizeof(buf));
	LogChat(buf, RGB(64, 255, 64));
	break;
	case 0xd3://ħ���б�
		ZeroMemory(buf, sizeof(buf));
		retval = Decode6BitBuf(data, buf, sizeof(buf));
		//LogSystem("�õ�ħ���б�",RGB(255,0,0));
		{
			char strbuf[512];
			magic_in_buf* p;
			int i;
			for (i = 0; i < MAX_MAGIC_COUNT; i++)
			{
				m_MagicList[i].name = "";
			}
			//p=(magic_in_buf*)buf;
			for (i = 0;/*i<msg.w3*/; i++)
			{
				p = (magic_in_buf*)(buf + i * 84);
				if (p->name_len > 0 && ((DWORD)p - (DWORD)buf) < retval)
				{
					ZeroMemory(strbuf, 512);
					memcpy(strbuf, p->name, ((p->name_len > 14) ? 14 : p->name_len));
					//LogSystem(strbuf,RGB(255,128,128));
					//LogSystem(p->name);
					m_MagicList[i].name = strbuf;
					m_MagicList[i].magic_id = p->magic_id;
					m_MagicList[i].level = p->level;
					m_MagicList[i].exp = p->exp;
					m_MagicList[i].exp_need[0] = p->exp_need[0];
					m_MagicList[i].exp_need[1] = p->exp_need[1];
					m_MagicList[i].exp_need[2] = p->exp_need[2];
				}
				else
				{
					break;
				}
			}
			//str="";
			//for(int i=0;i<retval;i++)
			//{
			//	sprintf(strbuf,"%02x ",(unsigned char)buf[i]);
			//	str+=strbuf;
			//	if((i&31)==31)
			//	{
			//		LogSystem(str,RGB(0,0,255));
			//		str="";
			//	}
			//	else if((i&15)==15)
			//	{
			//		str+="- ";
			//	}
			//}
			//LogSystem(str,RGB(0,0,255));
		}
		//LogSystem("",RGB(255,0,0));
		break;
	case 0x26d://���װ��
		ZeroMemory(buf, sizeof(buf));
		retval = Decode6BitBuf(data, buf, sizeof(buf));
		{
			char strbuf[512];
			item_in_buf* p = (item_in_buf*)buf;
			for (int i = 0; i < MAX_BODY_ITEM_COUNT; i++)
			{
				m_BodyItem[i].name = "";
			}
			while (p->magic > 0)
			{
				ZeroMemory(strbuf, 16);
				memcpy(strbuf, p->name, ((p->magic > 14) ? 14 : p->magic));
				m_BodyItem[p->idx].name = strbuf;
				m_BodyItem[p->idx].dura.low = p->dura;
				m_BodyItem[p->idx].dura.hi = p->dura_max;
				p++;
			}
		}
		ZeroMemory(&send_msg, sizeof(send_msg));
		send_msg.wCmd = 0x51;
		m_pSocket->SendMsg(&send_msg, "");
		break;
	case 0xc9://��������
		ZeroMemory(buf, sizeof(buf));
		retval = Decode6BitBuf(data, buf, sizeof(buf));
		{
			char strbuf[512];
			bagitem_in_buf* p = (bagitem_in_buf*)buf;
			int i = 0;
			while (p->magic > 0)
			{
				if (((DWORD)p - (DWORD)buf) > retval)
					break;
				ZeroMemory(strbuf, 512);
				memcpy(strbuf, p->name, ((p->magic > 14) ? 14 : p->magic));
				m_BagItem[i].name = strbuf;
				m_BagItem[i].dura.low = p->dura;
				m_BagItem[i].dura.hi = p->dura_max;
				p++;
				i++;
			}
			m_BagItem[i].name = "";
			////char strbuf[512];
			//str="";
			//for(int i=0;i<retval;i++)
			//{
			//	sprintf(strbuf,"%02x ",(unsigned char)buf[i]);
			//	str+=strbuf;
			//	if((i&31)==31)
			//	{
			//		LogSystem(str,RGB(0,0,255));
			//		str="";
			//	}
			//	else if((i&15)==15)
			//	{
			//		str+="- ";
			//	}
			//}
			//LogSystem(str,RGB(0,0,255));
		}
		break;
	case 0x262:// ���ϳ�����Ʒ
		ZeroMemory(buf, sizeof(buf));
		Decode6BitBuf(data, buf, sizeof(buf));
		str.Format("��Ʒ %s ������:(%d,%d)", buf, msg.w1, msg.w2);
		LogSystem(str);
		break;
	case 0x263:
		LogSystem("������ʧ��һ����Ʒ");
		break;
		///////////////////////////////////////////////////////////////////////////////////////////////////
		// ������Ϸ����
		///////////////////////////////////////////////////////////////////////////////////////////////////
	case 0x292:
	{
		ZeroMemory(buf, sizeof(buf));
		Decode6BitBuf(data, buf, sizeof(buf));
		char* pStart;
		unsigned int len = strlen(buf);
		pStart = buf;
		for (unsigned int ii = 0; ii < len; ii++)
		{
			if (buf[ii] == (char)0x1b)
			{
				buf[ii] = (char)0;
				LogChat(pStart, RGB(255, 0, 255));
				pStart = buf + ii + 1;
			}
		}
		ZeroMemory(&send_msg, sizeof(send_msg));
		send_msg.wCmd = 0x3fa;
		ZeroMemory(buf, sizeof(buf));
		socket->SendMsg(&send_msg, buf);
		break;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	// ����δ֪����
	///////////////////////////////////////////////////////////////////////////////////////////////////
	default:
		//str.Format("δ֪������[% 4X],% 8X,% 4X,% 4X,% 4X, %s",msg.wCmd,msg.dwFlag,msg.w1,msg.w2,msg.w3,data);
		//LogSystem(str,RGB(255,0,255));
		//ZeroMemory(buf,BUF_LENGTH);
		//Decode6BitBuf(data,buf,BUF_LENGTH);
		//LogSystem(buf,RGB(255,0,255));
		break;
	}
	return 0;
}
