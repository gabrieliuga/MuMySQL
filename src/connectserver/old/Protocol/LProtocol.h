#ifndef LPROTOCOL_H
#define LPROTOCOL_H

#include "StdAfx.h"
#include "ProtocolStructs.h"

enum 
{
	CS_SERVER_SELECT	= 0x03,
	CS_CLIENT_CONNECT	= 0x06
};

class CConServ
{
public:
	CConServ();
	~CConServ();

	void ConnectResultSend(CGameObject &Obj);
	void ServerListSend(CGameObject &Obj);
	void GetServerList(CGameObject &Obj);
	void GetServerInfo(CGameObject &Obj, USHORT ServerId);
	void LoadNewsFile(LPSTR szFile);
	void SendNews(CGameObject &Obj);
	void LoadServerList(LPSTR szFile);
	CONNECT_OBJECT cObj[1000];

}; extern CConServ g_ConnectServer;
void LProtocolCore(CGameObject &Obj, BYTE hCode, LPBYTE aRecv, int aLen);
void AddServer(PMSG_SERVERINFO * pMsg);
void LSProtocolCore(BYTE hCode, LPBYTE aRecv, int aLen);
//extern WzUdp g_CSSocket;
#endif

