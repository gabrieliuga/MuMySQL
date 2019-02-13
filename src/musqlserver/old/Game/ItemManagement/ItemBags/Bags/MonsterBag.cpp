////////////////////////////////////////////////////////////////////////////////
// MonsterBag.cpp
#include "StdAfx.h"
#include "MonsterBag.h"
#include "Logging/Log.h"
#include "User/CUserData.h"
#include "LuaBag.h"
#include "MapClass.h"
#include "GameMain.h"

CMonsterBag::CMonsterBag()
{
}

CMonsterBag::~CMonsterBag()
{
}

void CMonsterBag::SetBagInfo(int iParam1, int MonsterClass)
{
	this->m_BagMonsterClass = MonsterClass;
}

bool CMonsterBag::CheckCondition(CGameObject &Obj, int MonsterClass, int iParam2)
{
	if (rand() % 10000 >= this->m_BagData.dwBagUseRate)
	{
		return false;
	}

	if (this->m_BagMonsterClass == MonsterClass)
	{
		return true;
	}

	return false;
}

bool CMonsterBag::IsBag(CGameObject &Obj, int MonsterClass, int iParam2)
{
	if (this->m_BagMonsterClass == MonsterClass)
	{
		return true;
	}

	return false;
}

bool CMonsterBag::UseBag(CGameObject &Obj, int iMonsterIndex)
{
	if (lpObj.Type != OBJ_USER)
	{
		return false;
	}

	
	CGameObject lpMonsterObj = &gGameObjects[iMonsterIndex];

	if (rand() % 10000 >= this->m_BagData.dwItemDropRate)
	{
		MapC[lpObj.MapNumber].MoneyItemDrop(this->m_BagData.dwDropMoney, lpObj.X, lpObj.Y);

		return true;
	}

	if (rand() % 10000 < this->m_BagData.dwGainRuudRate)
	{
		int iRuudValue = this->GetValueMinMax(this->m_BagData.dwMinGainRuud, this->m_BagData.dwMaxGainRuud);
		lpObj.m_PlayerData->Ruud += iRuudValue;

		gGameProtocol.GCSendRuud(aIndex, lpObj.m_PlayerData->Ruud, iRuudValue, true);
		return true;
	}

	BAG_ITEM m_Item;
	BAG_SECTION_ITEMS m_ItemSection;
	BAG_SECTION_DROP m_DropSection;

	int iResult = this->GetDropSection(aIndex, m_DropSection);

	if (iResult == FALSE)
	{
		return false;
	}

	iResult = this->GetItemsSection(m_DropSection, m_ItemSection);

	if (iResult == FALSE)
	{
		return false;
	}

	if (m_ItemSection.btItemDropCount <= 0)
	{
		return false;
	}

	if (m_ItemSection.btItemDropCount == 1)
	{
		if (rand()%10000 < this->m_BagData.dwRandomSetItemDropRate)
		{
			MakeRewardSetItem(aIndex, lpMonsterObj.X, lpMonsterObj.Y, 1, lpObj.MapNumber);
			return true;
		}

		if (this->GetItem(m_ItemSection, m_Item) == FALSE)
		{
			return false;
		}

		bool bResult = gLuaBag.DropCommonBag(aIndex, lpMonsterObj.MapNumber, lpMonsterObj.X, lpMonsterObj.Y, &m_Item);

		if (bResult == false)
		{
			return false;
		}

		return true;
	}

	for (int i = 0; i < m_ItemSection.btItemDropCount; i++)
	{
 		BYTE cDropX = lpMonsterObj.X;
		BYTE cDropY = lpMonsterObj.Y;

		if (!gObjGetRandomItemDropLocation(lpMonsterObj.MapNumber, cDropX, cDropY, 4, 4, 10))
		{
			cDropX = lpMonsterObj.X;
			cDropY = lpMonsterObj.Y;
		}

		if (rand()%10000 < this->m_BagData.dwRandomSetItemDropRate)
		{
			MakeRewardSetItem(aIndex, cDropX, cDropY, 1, lpObj.MapNumber);
			continue;
		}	

		if (this->GetItem(m_ItemSection, m_Item) == FALSE)
		{
			return false;
		}

		bool bResult = gLuaBag.DropMonsterBag(aIndex, iMonsterIndex, lpMonsterObj.MapNumber, cDropX, cDropY, &m_Item);

		if (bResult == false)
		{
			return false;
		}

	}
	return true;
}

bool CMonsterBag::UseBag_GremoryCase(CGameObject &Obj, int iMonsterIndex, BYTE btStorageType, BYTE btRewardSource, int iExpireDays)
{
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

