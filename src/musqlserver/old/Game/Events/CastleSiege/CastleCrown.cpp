// CastleCrown.cpp: implementation of the CCastleCrown class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CastleCrown.h"
#include "CastleSiege.h"
#include "User/CUserData.h"
#include "Logging/Log.h"

CCastleCrown g_CsNPC_CastleCrown;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCastleCrown::CCastleCrown()
{

}

CCastleCrown::~CCastleCrown()
{

}

void CCastleCrown::CastleCrownAct(CGameObject &Obj)
{
	if ( !gObjIsConnected(Obj.m_Index))
		return;

	if ( Obj.Type != OBJ_NPC ||
		 Obj.Class != 216 )
		 return;

	if ( !gObjIsConnected(g_CastleSiege.GetCrownUserIndex()) )
	{
		g_CastleSiege.ResetCrownUserIndex();
		g_CastleSiege.SetCrownAccessUserX(0);
		g_CastleSiege.SetCrownAccessUserY(0);
		g_CastleSiege.ResetCrownAccessTickCount();

		return;
	}

	CGameObject* lpObj = Obj;
	CGameObject lpUserObj = getGameObject(g_CastleSiege.GetCrownUserIndex());

	if ( lpUserObj.MapNumber == MAP_INDEX_CASTLESIEGE &&
		 lpUserObj.m_btCsJoinSide >= 2 &&
		 lpUserObj.m_bCsGuildInvolved == 1 )
	{
		if ( lpUserObj.Y == g_CastleSiege.GetCrownAccessUserY() &&
			 lpUserObj.X == g_CastleSiege.GetCrownAccessUserX() )
		{
			if ( gObjIsConnected(g_CastleSiege.GetCrownUserIndex()) &&
				 gObjIsConnected(g_CastleSiege.GetCrownSwitchUserIndex(217)) &&
				 gObjIsConnected(g_CastleSiege.GetCrownSwitchUserIndex(218)) )
			{
				if ( getGameObject(g_CastleSiege.GetCrownUserIndex()]->m_btCsJoinSide == getGameObject(g_CastleSiege.GetCrownSwitchUserIndex(217))->m_btCsJoinSide &&
					 getGameObject(g_CastleSiege.GetCrownUserIndex()]->m_btCsJoinSide == getGameObject(g_CastleSiege.GetCrownSwitchUserIndex(218))->m_btCsJoinSide )	// #warning ??? maybe is 218??? 
				{
					DWORD dwTime = GetTickCount() - g_CastleSiege.GetCrownAccessTickCount();
		
					sLog->outBasic("[CastleSiege] [Reg. Accumulating] Accumulated Crown AccessTime : acc(%d) + %d [%s](%s)(%s)",
						lpUserObj.m_iAccumulatedCrownAccessTime, dwTime,
						lpUserObj.m_PlayerData->GuildName, lpUserObj.AccountID, lpUserObj.Name);

				}
				else
				{
					gGameProtocol.GCAnsCsAccessCrownState(lpUserObj.m_Index, 2);
					g_CastleSiege.ResetCrownUserIndex();
					g_CastleSiege.SetCrownAccessUserX(0);
					g_CastleSiege.SetCrownAccessUserY(0);
					g_CastleSiege.ResetCrownAccessTickCount();

					sLog->outBasic("[CastleSiege] [%s][%s] Register Castle Crown Canceled (GUILD:%s)",
						lpUserObj.AccountID, lpUserObj.Name, lpUserObj.m_PlayerData->GuildName);
				}
			}
			else
			{
				gGameProtocol.GCAnsCsAccessCrownState(lpUserObj.m_Index, 2);
				g_CastleSiege.ResetCrownUserIndex();
				g_CastleSiege.SetCrownAccessUserX(0);
				g_CastleSiege.SetCrownAccessUserY(0);
				g_CastleSiege.ResetCrownAccessTickCount();
			}
		}
		else
		{
			gGameProtocol.GCAnsCsAccessCrownState(lpUserObj.m_Index, 2);
			g_CastleSiege.ResetCrownUserIndex();
			g_CastleSiege.SetCrownAccessUserX(0);
			g_CastleSiege.SetCrownAccessUserY(0);
			g_CastleSiege.ResetCrownAccessTickCount();
		}
	}
	else
	{
		gGameProtocol.GCAnsCsAccessCrownState(lpUserObj.m_Index, 2);
		g_CastleSiege.ResetCrownUserIndex();
		g_CastleSiege.SetCrownAccessUserX(0);
		g_CastleSiege.SetCrownAccessUserY(0);
		g_CastleSiege.ResetCrownAccessTickCount();
	}
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

