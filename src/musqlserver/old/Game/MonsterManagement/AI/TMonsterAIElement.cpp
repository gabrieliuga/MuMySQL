// TMonsterAIElement.cpp: implementation of the TMonsterAIElement class.
//	GS-N	1.00.18	JPN	0x0055D120	-	Completed
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "TMonsterAIElement.h"
#include "TMonsterAIGroup.h"
#include "TMonsterSkillManager.h"
#include "TMonsterSkillUnit.h"
#include "TMonsterAIUtil.h"
#include "CrywolfUtil.h"
#include "KanturuUtil.h"
#include "KanturuMonsterMng.h"
#include "Logging/Log.h"
#include "Gamemain.h"
#include "util.h"
#include "BuffEffectSlot.h"
#include "configread.h"

static CKanturuUtil KANTURU_UTIL;

bool TMonsterAIElement::s_bDataLoad = FALSE;
TMonsterAIElement TMonsterAIElement::s_MonsterAIElementArray[MAX_AI_ELEMENT];
TMonsterAIMovePath TMonsterAIElement::s_MonsterAIMovePath[MAX_NUMBER_MAP];

static CCrywolfUtil UTIL;
static TMonsterAIUtil MONSTER_UTIL;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TMonsterAIElement::TMonsterAIElement()
{
	this->Reset();
}

TMonsterAIElement::~TMonsterAIElement()
{

}

void TMonsterAIElement::Reset()
{
	this->m_iElementClass = -1;
	this->m_iElementType = -1;
	this->m_iElementNumber = -1;
	this->m_iSuccessRate = -1;
	this->m_iDelayTime = -1;
	this->m_iTargetType = -1;
	this->m_iX = -1;
	this->m_iY = -1;
	memset(this->m_szElementName, 0, sizeof(this->m_szElementName));
}



BOOL TMonsterAIElement::LoadData(LPSTR lpszFileName)
{
	TMonsterAIElement::s_bDataLoad = FALSE;

	if ( lpszFileName == NULL || strcmp(lpszFileName, "") == 0 )
	{
		sLog->outError("[Monster AI Element] - File load error : File Name Error");
		return FALSE;
	}

	try
	{
		pugi::xml_document file;
		pugi::xml_parse_result res = file.load_file(lpszFileName);

		if ( res.status != pugi::status_ok )
		{
			sLog->outError("[Monster AI Element] - Can't Load %s (%s)", lpszFileName, res.description());
			return FALSE;
		}

		TMonsterAIElement::DelAllAIElement();

		pugi::xml_node mainXML = file.child("MonsterAI");

		for (pugi::xml_node element = mainXML.child("Element"); element; element = element.next_sibling())
		{
			char szElementName[50]={0};
			std::memcpy(szElementName, element.attribute("Name").as_string(), sizeof(szElementName));

			int iElementNumber = element.attribute("Number").as_int();
			int iElementClass = element.attribute("Class").as_int();
			int iElementType = element.attribute("Type").as_int();
			int iSuccessRate = element.attribute("SuccessRate").as_int();
			int iDelayTime = element.attribute("DelayTime").as_int();
			int iTargetType = element.attribute("TargetType").as_int();
			int iX = element.attribute("X").as_int();
			int iY = element.attribute("Y").as_int();

			if ( iElementNumber < 0 || iElementNumber >= MAX_AI_ELEMENT )
			{
				sLog->outError("[Monster AI Element] - ElementNumber(%d) Error (%s) File. ", iElementNumber, lpszFileName);
				continue;
			}

			if ( iElementClass < 0 || iElementClass >= MAX_AI_ELEMENT_CLASS )
			{
				sLog->outError("[Monster AI Element] - ElementClass(%d) Error (%s) File. ", iElementClass, lpszFileName);
				continue;
			}

			TMonsterAIElement::s_MonsterAIElementArray[iElementNumber].m_iElementNumber = iElementNumber;
			TMonsterAIElement::s_MonsterAIElementArray[iElementNumber].m_iElementClass = iElementClass;
			TMonsterAIElement::s_MonsterAIElementArray[iElementNumber].m_iElementType = iElementType;
			TMonsterAIElement::s_MonsterAIElementArray[iElementNumber].m_iSuccessRate = iSuccessRate;
			TMonsterAIElement::s_MonsterAIElementArray[iElementNumber].m_iDelayTime = iDelayTime;
			TMonsterAIElement::s_MonsterAIElementArray[iElementNumber].m_iTargetType = iTargetType;
			TMonsterAIElement::s_MonsterAIElementArray[iElementNumber].m_iX = iX;
			TMonsterAIElement::s_MonsterAIElementArray[iElementNumber].m_iY = iY;
			std::memcpy(TMonsterAIElement::s_MonsterAIElementArray[iElementNumber].m_szElementName, szElementName, sizeof(szElementName));
		}

		TMonsterAIElement::s_bDataLoad = TRUE;
	}

	catch (DWORD)
	{
		sLog->outError("[Monster AI Element] - Loading Exception Error (%s) File. ", lpszFileName);
	}

	TMonsterAIElement::s_MonsterAIMovePath[MAP_INDEX_CRYWOLF_FIRSTZONE].LoadData(g_ConfigRead.GetPath("\\Monsters\\AI\\IGC_Monster_AI_MovePath.xml"), "CryWolf");
	return FALSE;
}



BOOL TMonsterAIElement::DelAllAIElement()
{
	for ( int i=0;i<MAX_AI_ELEMENT;i++)
	{
		TMonsterAIElement::s_MonsterAIElementArray[i].Reset();
	}

	for ( int j=0;j<MAX_NUMBER_MAP;j++)
	{
		TMonsterAIElement::s_MonsterAIMovePath[j].DelAllAIMonsterMovePath();
	}

	return FALSE;
}



TMonsterAIElement * TMonsterAIElement::FindAIElement(int iElementNumber)
{
	if ( (iElementNumber < 0 && iElementNumber != -1) || iElementNumber >= MAX_AI_ELEMENT )
	{
		return NULL;
	}

	if ( TMonsterAIElement::s_MonsterAIElementArray[iElementNumber].m_iElementNumber == iElementNumber )
	{
		return &TMonsterAIElement::s_MonsterAIElementArray[iElementNumber];
	}
	return NULL;
}



BOOL TMonsterAIElement::ForceAIElement(CGameObject &Obj, int iTargetIndex, TMonsterAIState *pAIState)
{
	CGameObject* lpObj = Obj;

	if ( (rand()%100) > this->m_iSuccessRate )
		return FALSE;

	switch ( this->m_iElementType )
	{
		case MAE_TYPE_COMMON_NORMAL:
			this->ApplyElementCommon(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_MOVE_NORMAL:
			this->ApplyElementMove(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_MOVE_TARGET:
			this->ApplyElementMoveTarget(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_GROUP_MOVE:
			this->ApplyElementGroupMove(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_ATTACK_NORMAL:
			this->ApplyElementAttack(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_ATTACK_AREA:
			this->ApplyElementAttackArea(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_ATTACK_PENETRATION:
			this->ApplyElementAttackPenetration(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_HEAL_SELF:
			this->ApplyElementHealSelf(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_HEAL_GROUP:
			this->ApplyElementHealGroup(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_AVOID_NORMAL:
			this->ApplyElementAvoid(iIndex, iTargetIndex, pAIState);
			break;
		/*case MAE_TYPE_HELP_HP:
			this->ApplyElement(iIndex, iTargetIndex, pAIState);
			break;*/
		/*case MAE_TYPE_HELP_BUFF:
			this->ApplyElement(iIndex, iTargetIndex, pAIState);
			break;*/
		/*case MAE_TYPE_HELP_TARGET:
			this->ApplyElement(iIndex, iTargetIndex, pAIState);
			break;*/
		/*case MAE_TYPE_SPECIAL:
			this->ApplyElement(iIndex, iTargetIndex, pAIState);
			break;*/
		case MAE_TYPE_SPECIAL_SOMMON:
			this->ApplyElementSpecialSommon(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_SPECIAL_NIGHTMARE_SUMMON:
			this->ApplyElementNightmareSummon(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_SPECIAL_WARP:
			this->ApplyElementNightmareWarp(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_SPECIAL_SKILLATTACK:
			this->ApplyElementSkillAttack(iIndex, iTargetIndex, pAIState);
			break;
		case MAE_TYPE_SPECIAL_CHANGEAI:
			this->ApplyElementAIChange(iIndex, iTargetIndex, pAIState);
			break;
		/*case MAE_TYPE_EVENT:
			this->ApplyElement(iIndex, iTargetIndex, pAIState);
			break;*/
		case MAE_TYPE_SPECIAL_IMMUNE:
			this->ApplyElementSpecialImmune(iIndex, iTargetIndex, pAIState);
			break;
	}
	return TRUE;
}



BOOL TMonsterAIElement::ApplyElementCommon(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	return FALSE;
}

BOOL TMonsterAIElement::ApplyElementMove(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	UTIL.SendCrywolfChattingMsg(iIndex, "Element-이동");

	if ( Obj.PathStartEnd )
		return FALSE;

	BOOL bFindXY = FALSE;

	if ( pAIState->m_iTransitionType == 2 )
		bFindXY = MONSTER_UTIL.GetXYToChase(lpObj);
	else
		bFindXY = MONSTER_UTIL.GetXYToPatrol(lpObj);

	if ( bFindXY )
		MONSTER_UTIL.FindPathToMoveMonster(lpObj, Obj.MTX, Obj.MTY, 5, 1);

	return FALSE;
}

BOOL TMonsterAIElement::ApplyElementMoveTarget(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	UTIL.SendCrywolfChattingMsg(iIndex, "Element-타겟이동");

	if ( Obj.PathStartEnd )
		return FALSE;

	if ( Obj.X == this->m_iX &&
		 Obj.Y == this->m_iY )
	{
		this->ApplyElementMove(iIndex, iTargetIndex, pAIState);
		return FALSE;
	}

	BOOL bFindXY = TRUE;
	int iTargetX = this->m_iX;
	int iTargetY = this->m_iY;
	int iTargetDistance = sqrt( static_cast<float>( ((Obj.X - iTargetX)*(Obj.X - iTargetX))+ ((Obj.Y - iTargetY)*(Obj.Y - iTargetY)) ));
	
	if ( TMonsterAIElement::s_MonsterAIMovePath[Obj.MapNumber].m_bDataLoad )
	{
		if ( iTargetDistance > 10 )
		{
			int iMinCost = 1000000;
			int iMidX = -1;
			int iMidY = -1;
			int iSpotNum = -1;

			for ( int i=0;i<MAX_MONSTER_AI_MOVE_PATH;i++)
			{
				TMonsterAIMovePathInfo & PathInfo = TMonsterAIElement::s_MonsterAIMovePath[MAP_INDEX_CRYWOLF_FIRSTZONE].m_MovePathInfo[i];
				float fDistX = Obj.X - PathInfo.m_iPathX;
				float fDistY = Obj.Y - PathInfo.m_iPathY;
				int iPathSpotDist =  sqrt( (fDistX*fDistX) + (fDistY*fDistY) );

				if ( iPathSpotDist < 20 )
				{
					fDistX = iTargetX - PathInfo.m_iPathX;
					fDistY = iTargetY - PathInfo.m_iPathY;
					int iMidDist = sqrt( (fDistX*fDistX) + (fDistY*fDistY) );

					if ( iMinCost > iMidDist )
					{
						if ( iMidDist )
						{
							iMinCost = iMidDist;
							iMidX = PathInfo.m_iPathX;
							iMidY = PathInfo.m_iPathY;
							iSpotNum = i;
						}
					}
				}
			}

			if ( iMinCost != 1000000 )
			{
				iTargetX = iMidX;
				iTargetY = iMidY;
			}
		}
	}

	if ( bFindXY )
	{
		if ( MONSTER_UTIL.FindPathToMoveMonster(lpObj, iTargetX, iTargetY, 7, FALSE) )
			Obj.PathStartEnd = 1;
		else
			Obj.PathStartEnd = 0;
	}

	return FALSE;
}


BOOL TMonsterAIElement::ApplyElementGroupMove(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	UTIL.SendCrywolfChattingMsg(iIndex, "Element-그룹이동");

	if ( Obj.PathStartEnd )
		return FALSE;

	BOOL bFindXY = FALSE;
	BOOL bFoundLeader = TRUE;
	int iLeaderIndex = -1;
	iLeaderIndex = TMonsterAIGroup::FindGroupLeader(Obj.m_iGroupNumber);

	if ( iLeaderIndex == -1 || getGameObject(iLeaderIndex)->Live == FALSE )
		bFoundLeader = FALSE;

	if ( bFoundLeader && gObjCalDistance(lpObj, getGameObject(iLeaderIndex)) > 6 )
	{
		Obj.TargetNumber = iLeaderIndex;
		bFindXY = MONSTER_UTIL.GetXYToChase(lpObj);
	}
	else if ( pAIState->m_iTransitionType == 2 )
	{
		bFindXY = MONSTER_UTIL.GetXYToChase(lpObj);
	}
	else
	{
		bFindXY = MONSTER_UTIL.GetXYToPatrol(lpObj);
	}

	if ( bFindXY )
	{
		if ( MONSTER_UTIL.FindPathToMoveMonster(lpObj, Obj.MTX, Obj.MTY, 5, TRUE) )
		{

		}
		else
		{
			MONSTER_UTIL.GetXYToPatrol(lpObj);
			MONSTER_UTIL.FindPathToMoveMonster(lpObj, Obj.MTX, Obj.MTY, 5, TRUE);
		}
	}

	return FALSE;
}

BOOL TMonsterAIElement::ApplyElementAttack(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	UTIL.SendCrywolfChattingMsg(iIndex, "Element-공격");

	if ( TMonsterSkillManager::CheckMonsterSkill(Obj.Class) && Obj.Class != 519 ) // Medic has only BUFF in MONSTERSKILL, so disable check for him
	{
		BOOL bEnableAttack = TRUE;
		int iTargetNumber = Obj.TargetNumber;

		if ( iTargetNumber < 0 )
			bEnableAttack = FALSE;

		else if ( !getGameObject(iTargetNumber)->Live || getGameObject(iTargetNumber)->Teleport )
			bEnableAttack = FALSE;

		else if ( getGameObject(iTargetNumber)->Connected <= PLAYER_LOGGED ||
			 getGameObject(iTargetNumber)->CloseCount != -1 )
		{
			bEnableAttack = FALSE;
		}

		if ( !bEnableAttack )
		{
			Obj.TargetNumber = -1;
			Obj.m_ActState.Emotion = 0;
			Obj.m_ActState.Attack = 0;
			Obj.m_ActState.Move = 0;
			Obj.NextActionTime = 1000;

			return FALSE;
		}

		CGameObject lpTargetObj = getGameObject(iTargetNumber);
		Obj.Dir = GetPathPacketDirPos(lpTargetObj.X - Obj.X, lpTargetObj.Y - Obj.Y);

		if ( (rand()%4) == 0 )
		{
			PMSG_ATTACK pAttackMsg;

			pAttackMsg.AttackAction = 0x78;
			pAttackMsg.DirDis = Obj.Dir;
			pAttackMsg.NumberH = SET_NUMBERH(iTargetNumber);
			pAttackMsg.NumberL = SET_NUMBERL(iTargetNumber);

			gGameProtocol.GCActionSend(lpObj, 0x78, Obj.m_Index, iTargetNumber);
			gObjAttack(lpObj, getGameObject(iTargetNumber), 0, 0, 0, 0, 0, 0, 0);
		}
		else
		{
			TMonsterSkillManager::UseMonsterSkill(Obj.m_Index, iTargetNumber, 0, -1, NULL);
		}

		Obj.m_ActState.Attack = 0;
		return FALSE;
	}
	else
	{
		int iTargetNumber = Obj.TargetNumber;

		if (!ObjectMaxRange(iTargetNumber))
		{
			return FALSE;
		}

		CGameObject lpTargetObj = getGameObject(iTargetNumber);
		Obj.Dir = GetPathPacketDirPos(lpTargetObj.X - Obj.X, lpTargetObj.Y - Obj.Y);

		PMSG_ATTACK pAttackMsg;

		pAttackMsg.AttackAction = 0x78;
		pAttackMsg.DirDis = Obj.Dir;
		pAttackMsg.NumberH = SET_NUMBERH(iTargetNumber);
		pAttackMsg.NumberL = SET_NUMBERL(iTargetNumber);

		gGameProtocol.CGAttack((PMSG_ATTACK *)&pAttackMsg, Obj.m_Index);
		gGameProtocol.GCActionSend(lpObj, 0x78, Obj.m_Index, lpTargetObj.m_Index);
		gObjAttack(lpObj, getGameObject(iTargetNumber), 0, 0, 0, 0, 0, 0, 0);

		return FALSE;
	}
}

struct PMSG_NOTIFY_REGION_MONSTER_ATTACK
{
	PBMSG_HEAD2 h;	// C1:BD:0C
	BYTE btObjClassH;	// 4
	BYTE btObjClassL;	// 5
	BYTE btSourceX;	// 6
	BYTE btSourceY;	// 7
	BYTE btPointX;	// 8
	BYTE btPointY;	// 9
};


BOOL TMonsterAIElement::ApplyElementAttackArea(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	UTIL.SendCrywolfChattingMsg(iIndex, "Element-영역공격");

	int iTargetX = this->m_iX + (rand()%5) * ((rand()%2==0)?1:-1 ) ;
	int iTargetY = this->m_iY + (rand()%5) * ((rand()%2==0)?1:-1 ) ;

	for (int i= g_ConfigRead.server.GetObjectStartUserIndex();i<g_ConfigRead.server.GetObjectMax();i++)
	{
		if ( !gObjIsConnected(i))
			continue;

		CGameObject *lpTargetObj = getGameObject(i);

		if ( !Obj.Live )
			continue;

		if ( Obj.MapNumber != lpTargetObj.MapNumber )
			continue;

		int iTargetDistance = sqrt( static_cast<float> ( ((lpTargetObj.X - iTargetX)*(lpTargetObj.X - iTargetX)) + ((lpTargetObj.Y - iTargetY)*(lpTargetObj.Y - iTargetY)) ) );

		if ( iTargetDistance < 10 )
		{
			PMSG_NOTIFY_REGION_MONSTER_ATTACK pMsg;

			PHeadSubSetB((BYTE*)&pMsg, 0xBD, 0x0C, sizeof(pMsg));
			pMsg.btObjClassH = SET_NUMBERH(Obj.Class);
			pMsg.btObjClassL = SET_NUMBERL(Obj.Class);
			pMsg.btSourceX = Obj.X;
			pMsg.btSourceY = Obj.Y;
			pMsg.btPointX = iTargetX;
			pMsg.btPointY = iTargetY;

			IOCP.DataSend(lpTargetObj.m_Index, (BYTE*)&pMsg, sizeof(pMsg));
		}

		if ( iTargetDistance < 6 )
		{
			gObjAttack(lpObj, lpTargetObj, 0, 0, 0, 0, 0, 0, 0);
		}
	}

	return FALSE;
}

BOOL TMonsterAIElement::ApplyElementAttackPenetration(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	iTargetIndex = Obj.TargetNumber;

	if ( iTargetIndex == -1 )
		return FALSE;

	if ( getGameObject(iTargetIndex)->Live == 0 )
		return FALSE;

	TMonsterSkillManager::UseMonsterSkill(iIndex, iTargetIndex, 2, -1, NULL);
	return FALSE;
}


BOOL TMonsterAIElement::ApplyElementAvoid(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	UTIL.SendCrywolfChattingMsg(iIndex, "Element-회피");

	BOOL bFindXY = MONSTER_UTIL.GetXYToEascape(lpObj);

	if ( bFindXY )
	{
		MONSTER_UTIL.FindPathToMoveMonster(lpObj, Obj.MTX, Obj.MTY, 5, 1);
	}

	return FALSE;
}


BOOL TMonsterAIElement::ApplyElementHealSelf(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	UTIL.SendCrywolfChattingMsg(iIndex, "Element-셀프치료");

	Obj.Life += Obj.Life * 20.0f / 100.0f;
	UTIL.SendCrywolfChattingMsg(iIndex, "HP : %d", (int)Obj.Life);
	//Obj.m_ViewSkillState |= 8;
	//GCStateInfoSend(lpObj, 1, Obj.m_ViewSkillState);

	return FALSE;
}


BOOL TMonsterAIElement::ApplyElementHealGroup(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	UTIL.SendCrywolfChattingMsg(iIndex, "Element-그룹치료");
	TMonsterAIGroupMember * pMemb = TMonsterAIGroup::FindGroupMemberToHeal(Obj.m_Index, Obj.m_iGroupNumber, Obj.m_iGroupMemberGuid, 6);

	if ( pMemb )
	{
		CGameObject lpTargetObj = getGameObject(pMemb->m_iObjIndex);

		if ( lpTargetObj.Live == 0 )
			return FALSE;

		TMonsterSkillUnit * lpSkillUnit = NULL;

		if(Obj.Class == 519)
		{
			TMonsterSkillManager::UseMonsterSkill(iIndex, pMemb->m_iObjIndex, 44, -1, NULL);
		}
		else
		{
			lpSkillUnit = TMonsterSkillManager::FindMonsterSkillUnit(Obj.m_Index, 21);
		}

		if ( lpSkillUnit )
		{
			lpSkillUnit->RunSkill(iIndex, lpTargetObj.m_Index);
		}

		UTIL.SendCrywolfChattingMsg(iIndex, "그룹치료 HP : %d", (int)lpTargetObj.Life);
		UTIL.SendCrywolfChattingMsg(lpTargetObj.m_Index, "HP : %d", (int)lpTargetObj.Life);
	}

	return FALSE;
}


BOOL TMonsterAIElement::ApplyElementSpecialSommon(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	UTIL.SendCrywolfChattingMsg(iIndex, "Element-특수소환");
	TMonsterAIGroupMember * pMemb = TMonsterAIGroup::FindGroupMemberToSommon(Obj.m_Index, Obj.m_iGroupNumber, Obj.m_iGroupMemberGuid);

	if ( pMemb )
	{
		CGameObject lpTargetObj = getGameObject(pMemb->m_iObjIndex);

		if ( lpTargetObj.Live != 0 )
			return FALSE;

		TMonsterSkillUnit * lpSkillUnit = TMonsterSkillManager::FindMonsterSkillUnit(Obj.m_Index, 30);

		if ( lpSkillUnit )
		{
			lpSkillUnit->RunSkill(iIndex, lpTargetObj.m_Index);
		}
	}

	return FALSE;
}

BOOL TMonsterAIElement::ApplyElementSpecialImmune(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;
	
	this->m_iX = 10;
	this->m_iY = 10;

	gObjAddBuffEffect(lpObj, BUFFTYPE_MONSTER_MAGIC_IMMUNE, 0, 0, 0, 0, 10);
	gObjAddBuffEffect(lpObj, BUFFTYPE_MONSTER_MELEE_IMMUNE, 0, 0, 0, 0, 10);

	return FALSE;
}

BOOL TMonsterAIElement::ApplyElementNightmareSummon(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;

	if ( Obj.TargetNumber == -1 )
		iTargetIndex = Obj.m_Index;

	iTargetIndex = Obj.TargetNumber;

	if (iTargetIndex == -1)
	{
		return FALSE;
	}

	TMonsterSkillUnit * lpSkillUnit = TMonsterSkillManager::FindMonsterSkillUnit(Obj.m_Index, 30); 

	if (lpSkillUnit)
	{
		gGameProtocol.GCUseMonsterSkillSend(Obj, getGameObject(iTargetIndex), lpSkillUnit->m_iUnitNumber);

		if (g_ConfigRead.server.GetServerType() != SERVER_CASTLE)
		{
			int iRegenMonster = g_KanturuMonsterMng.SetKanturuMonster(6);
		}
	}

	return FALSE;
}

BOOL TMonsterAIElement::ApplyElementNightmareWarp(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;

	BYTE x = this->m_iX;
	BYTE y = this->m_iY;

	PMSG_MAGICATTACK_RESULT pAttack;

	PHeadSetBE((BYTE *)&pAttack,0x19,sizeof(pAttack));

	pAttack.MagicNumberH = HIBYTE(AT_SKILL_TELEPORT);
	pAttack.MagicNumberL = LOBYTE(AT_SKILL_TELEPORT);
	pAttack.SourceNumberH = SET_NUMBERH(Obj.m_Index);
	pAttack.SourceNumberL = SET_NUMBERL(Obj.m_Index);
	pAttack.TargetNumberH = SET_NUMBERH(Obj.m_Index);
	pAttack.TargetNumberL = SET_NUMBERL(Obj.m_Index);

	if ( Obj.Type == OBJ_USER )
		IOCP.DataSend(Obj.m_Index,(BYTE *)&pAttack,pAttack.h.size);

	gGameProtocol.MsgSendV2(lpObj,(BYTE*)&pAttack,pAttack.h.size);

	gObjTeleportMagicUse(iIndex,x,y);
	Obj.TargetNumber = -1;

	return FALSE;
}



BOOL TMonsterAIElement::ApplyElementSkillAttack(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	CGameObject* lpObj = Obj;

	if ( TMonsterSkillManager::CheckMonsterSkill(Obj.Class) )
	{
		BOOL bEnableAttack = TRUE;
		int iTargetNumber = Obj.TargetNumber;

		if (iTargetNumber < 0 )
			bEnableAttack = FALSE;

		else if ( !getGameObject(iTargetNumber)->Live || getGameObject(iTargetNumber)->Teleport )
			bEnableAttack = FALSE;

		else if ( getGameObject(iTargetNumber)->Connected <= PLAYER_LOGGED ||
			 getGameObject(iTargetNumber)->CloseCount != -1 )
		{
			bEnableAttack = FALSE;
		}

		if ( !bEnableAttack )
		{
			Obj.TargetNumber = -1;
			Obj.m_ActState.Emotion = 0;
			Obj.m_ActState.Attack = 0;
			Obj.m_ActState.Move = 0;
			Obj.NextActionTime = 1000;

			return FALSE;
		}

		CGameObject lpTargetObj = getGameObject(iTargetNumber);
		Obj.Dir = GetPathPacketDirPos(lpTargetObj.X - Obj.X, lpTargetObj.Y - Obj.Y);
		int iRate1 = this->m_iTargetType;
		int iRate2 = this->m_iX;
		int iRate3 = this->m_iY;
		int iRandom = rand() % 100;

		if ( iRandom < iRate1 )
			TMonsterSkillManager::UseMonsterSkill(Obj.m_Index, iTargetNumber, 0, -1, NULL);
		else if ( iRandom < (iRate1+iRate2) )
			TMonsterSkillManager::UseMonsterSkill(Obj.m_Index, iTargetNumber, 1, -1, NULL);
		else if ( iRandom < (iRate1+iRate2+iRate3) )
			TMonsterSkillManager::UseMonsterSkill(Obj.m_Index, iTargetNumber, 2, -1, NULL);

		Obj.m_ActState.Attack = 0;
		return FALSE;
	}

	return FALSE;
}
		

BOOL TMonsterAIElement::ApplyElementAIChange(CGameObject &Obj, int iTargetIndex, TMonsterAIState * pAIState)
{
	TMonsterAIGroup::ChangeAIOrder(this->m_iTargetType, this->m_iX);
	sLog->outError(  "[AI Change] Group %d AI Order %d",
		this->m_iTargetType, this->m_iX);

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

