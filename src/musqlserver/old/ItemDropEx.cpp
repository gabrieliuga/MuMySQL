////////////////////////////////////////////////////////////////////////////////
// ItemDropEx.cpp
#include "StdAfx.h"
#include "ItemDropEx.h"
#include "TLog.h"
#include "Readscript.h"
#include "DSProtocol.h"
#include "gObjMonster.h"
#include "GameMain.h"
#include "configread.h"
// -------------------------------------------------------------------------

DropEx	g_DropEx;
// -------------------------------------------------------------------------------

DropEx::DropEx()
{
	this->Init();
}
// -------------------------------------------------------------------------

DropEx::~DropEx()
{
	// ----
}
// -------------------------------------------------------------------------

void DropEx::Init()
{
	this->m_Data.clear();
#ifdef ITEM_BLOCK
	this->m_NonDrop.clear();
#endif
}
// -------------------------------------------------------------------------

void DropEx::Load()
{
	this->Init();
	this->ReadData(g_ConfigRead.GetPath("Custom\\ItemDropRate.ini"));
	this->ReadBlockData(g_ConfigRead.GetPath("Custom\\ItemDropBlocked.ini"));
}
// -------------------------------------------------------------------------

void DropEx::ReadData(char * File)
{
	__try
	{
		SMDFile = fopen(File, "r");
		// ----
		while (true)
		{
			SMDToken Token = GetToken();
			// ----
			if (Token == END)
			{
				break;
			}
			// ----
			DROPEX_DATA List;
			// ----
			List.ItemType = TokenNumber;
			// ----
			GetToken();
			List.ItemIndex = TokenNumber;
			// ----
			GetToken();
			List.ItemMinLevel = TokenNumber;
			// ----
			GetToken();
			List.ItemMaxLevel = TokenNumber;
			// ----
			GetToken();
			List.ItemMaxOption = TokenNumber;
#ifdef NEW_DROP
			GetToken();
			List.ItemOptRate = TokenNumber;
#endif
			// ----
			GetToken();
			List.ItemLuck = TokenNumber;
			// ----
#ifdef NEW_DROP
			GetToken();
			List.ItemLuckRate = TokenNumber;
#endif
			GetToken();
			List.ItemSkill = TokenNumber;
			// ----
#ifdef NEW_DROP
			GetToken();
			List.ItemSkillRate = TokenNumber;
#endif
			GetToken();
			List.ItemExcellent = TokenNumber;
			// ----
#ifdef NEW_DROP
			GetToken();
			List.ItemMaxExc = TokenNumber;

			GetToken();
			List.ItemExcRate = TokenNumber;
			// ----
			GetToken();
			List.ItemAncent = TokenNumber;
			// ----
			GetToken();
			List.ItemAncRate = TokenNumber;
#endif
			GetToken();
			List.ItemDur = TokenNumber;
			// ----
			GetToken();
			List.ItemDropRate = TokenNumber;
			// ----
			GetToken();
			List.MonsterMinLevel = TokenNumber;
			// ----
			GetToken();
			List.MonsterMaxLevel = TokenNumber;
			// ----
			GetToken();
			List.MonsterMap = TokenNumber;
			// ----
			this->m_Data.push_back(List);
		}
	}
	// ----
	__finally
	{
		if (SMDFile != NULL)
		{
			fclose(SMDFile);
		}
		// ----
		SMDFile = NULL;
	}
}
// -------------------------------------------------------------------------

#ifdef ITEM_BLOCK
void DropEx::ReadBlockData(char * File)
{
	__try
	{
		SMDFile = fopen(File, "r");
		// ----
		while (true)
		{
			SMDToken Token = GetToken();
			// ----
			if (Token == END)
			{
				break;
			}
			// ----
			DROPBLOCK_DATA List;
			// ----
			List.ItemType = TokenNumber;
			// ----
			GetToken();
			List.ItemIndex = TokenNumber;
			// ----
			this->m_NonDrop.push_back(List);
		}
	}
	// ----
	__finally
	{
		if (SMDFile != NULL)
		{
			fclose(SMDFile);
		}
		// ----
		SMDFile = NULL;
	}
}

// -------------------------------------------------------------------------

bool DropEx::IsBlockItem(int ItemID)
{
	for (int i = 0; i < this->m_NonDrop.size(); i++)
	{
		DROPBLOCK_DATA & Drop = this->m_NonDrop[i];
		// ----
		if (ItemID == ITEMGET(Drop.ItemType, Drop.ItemIndex))
		{
			return true;
		}
	}
	// ----
	return false;
}
#endif
// -------------------------------------------------------------------------

bool DropEx::DropItem(LPOBJ lpObj, LPOBJ lpTargetObj)
{
	switch (lpObj->Class)
	{
	case 78:	//-> Golden Goblin
	case 53:	//-> Golden Titan
	case 79:	//-> Golden Dragon
	case 80:	//-> Golden Lizard
	case 82:	//-> Golden Tantalos
	case 502:	//-> Golden Rabbit
	case 493:	//-> Golden Dark Knight
	case 494:	//-> Golden Devil
	case 495:	//-> ???
	case 496:	//-> Golden Crust
	case 497:	//-> Golden Satiros
	case 498:	//-> Golden Twintail
	case 499:	//-> Golden Iron Knight
	case 500:	//-> Golden Neipin
	case 501:	//-> Golden Great Dragon
	{
		return false;
	}
	break;
	}
	// ----
	for (int i = 0; i < this->m_Data.size(); i++)
	{
		DROPEX_DATA & Drop = this->m_Data[i];
		// ----
		if (lpObj->Level >= Drop.MonsterMinLevel && lpObj->Level <= Drop.MonsterMaxLevel)
		{
			if (lpObj->MapNumber == Drop.MonsterMap || Drop.MonsterMap == -1)
			{
				if (rand() % 10000 < Drop.ItemDropRate)
				{
					int iLuck = 0;
					int iSkill = 0;
					int iOption = 0;
					int iLevel = 0;

					// ----
					if (Drop.ItemMaxLevel == Drop.ItemMinLevel)
					{
						iLevel = Drop.ItemMinLevel;
					}
					else
					{
						iLevel = Drop.ItemMinLevel + (rand() % (Drop.ItemMaxLevel - Drop.ItemMinLevel + 1));
					}
#ifdef NEW_DROP
					if (Drop.ItemLuck == 1)
					{
						if (rand() % 100 < Drop.ItemLuckRate)
						{
							iLuck = 1;
						}
					}

					if (Drop.ItemSkill == 1)
					{
						if (rand() % 100 < Drop.ItemSkillRate)
						{
							iSkill = 1;
						}
					}

					if (Drop.ItemMaxOption != 0)
					{
						if (rand() % 100 < Drop.ItemOptRate)
						{
							iOption = rand() % Drop.ItemMaxOption + 1;
						}
					}

					int iExlOpt = 0;

					if (Drop.ItemExcellent)
					{
						if (rand() % 100 < Drop.ItemExcRate)
						{
							iExlOpt = this->GenExcOption(Drop.ItemMaxExc);
						}
					}

					int iAnc = 0;

					if (Drop.ItemAncent == 5 || Drop.ItemAncent == 10)
					{
						if (rand() % 100 < Drop.ItemAncRate)
						{
							iAnc = Drop.ItemAncent;
						}
					}

					ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(Drop.ItemType, Drop.ItemIndex), iLevel, Drop.ItemDur, iOption, iLuck, iSkill, gObjMonsterTopHitDamageUser(lpObj), iExlOpt, iAnc, 0, 0, 0);
#else
					// ----
					if (Drop.ItemLuck == 1)
					{
						if (rand() % 100 < 50)
						{
							iLuck = 1;
						}
					}
					// ----
					if (Drop.ItemSkill == 1)
					{
						if (rand() % 100 < 50)
						{
							iSkill = 1;
						}
					}
					// ----
					if (Drop.ItemMaxOption != 0)
					{
						if (rand() % 100 < 50)
						{
							iOption = rand() % Drop.ItemMaxOption + 1;
						}
					}

					ItemSerialCreateSend(lpObj->m_Index, lpObj->MapNumber, lpObj->X, lpObj->Y, ITEMGET(Drop.ItemType, Drop.ItemIndex), iLevel, Drop.ItemDur, iOption, iLuck, iSkill, gObjMonsterTopHitDamageUser(lpObj), Drop.ItemExcellent, 0, 0, 0, 0);
#endif

					return true;
				}
			}
		}
	}
	// ----
	return false;
}

int DropEx::GenExcOption(int amount)
{
	if (amount > 6) amount = 6;
	if (amount < 1) amount = 1;

	srand(time(NULL));

	amount = rand() % amount + 1;

	int opt_db[6] = { 1, 2, 4, 8, 16, 32 };
	int exc = 0;

	int opt_done[6] = { 0 };

	for (int n = 0; n < amount; n++)
	{
		int SearchExc = 0;

		srand(time(NULL));

		if (n == 0)
		{
			SearchExc = opt_db[rand() % 6];
			opt_done[n] = SearchExc;
		}
		else
		{
			while (true)
			{
				SearchExc = opt_db[rand() % 6];
				bool bOpt = true;

				for (int i = 0; i<amount; i++)
				{
					if (SearchExc == opt_done[i])
					{
						bOpt = false;
					}
				}

				if (bOpt == true)
				{
					opt_done[n] = SearchExc;
					break;
				}
			}
		}

		exc += SearchExc;
	}

	return exc;
}
// -------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

