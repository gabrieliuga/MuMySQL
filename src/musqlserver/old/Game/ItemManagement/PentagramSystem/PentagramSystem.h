#ifndef _MU_PENTAGRAMSYSTEM_H
#define _MU_PENTAGRAMSYSTEM_H

#include "StdAfx.h"
#include "ItemObject.h"
#include "CGameObject.h"
#include "TRandomPoolMgr.h"


struct TEST_ITEMSDROP;
struct SERVER_ATTRIBUTE_DEFINE;
struct PENTAGRAM_ITEM_OPEN_SOCKET_RATE;
struct MONSTER_DROP_ITEM_RATE;
struct JEWEL_OUT_RATE;
struct PENTAGRAM_SET_EFFECT;
struct PENTAGRAM_HAVE_SET_OPTION;
struct PENTAGRAM_ITEM_OPTION;
struct PENTAGRAM_ITEM_OPTION_ENABLE_NEED;
struct PENTAGRAM_SOCKET_RATE_BY_GRADE;


class CItemObject;
class TRandomPoolMgr;


class CPentagramSystem
{
public:
	CPentagramSystem(void);
	virtual ~CPentagramSystem(void);

	void Initialize_Drop();
	bool LoadDropScript(char* pchFileName);
	void Initialize_JewelOutRate();
	bool LoadJewelOutRate(char* pchFileName);
	void Initialize_SetOption();
	bool LoadPentagramSetOptionScript(char* pchFileName);
	void Initialize_PentagramItemOption();
	bool LoadPentagramOptionScript(char* pchFileName);

	bool IsPentagramItem(CItemObject &Item);
	bool IsPentagramItem(int ItemCode);

	bool IsPentagramMithril(int ItemIndex);
	bool IsPentagramJewel(CItemObject &Item);
	bool IsPentagramJewel(int ItemCode);

	void ClearPentagramItem(CGameObject &Obj);
	void CalcPentagramItem(CGameObject &Obj, CItemObject *lpItemData);

	bool IsEnableDropPentagramItemMap(int iMapIndex);
	int AttributeMonsterItemDrop(CGameObject &Obj);

	BYTE GetMakePentagramSlotCountNKind(BYTE *btEnableSlot, int iType);
	bool SetPentagramMainAttribute(CItemObject *lpItemData, BYTE btAttributeNumber);
	bool MakePentagramSocketSlot(CItemObject *lpItemData, BYTE btSocketSlot1, BYTE btSocketSlot2, BYTE btSocketSlot3, BYTE btSocketSlot4, BYTE btSocketSlot5);
	bool ClearPentagramSocketSlot(CGameObject &Obj, int iInventoryPos, CItemObject *lpTargetItem, BYTE btSocketSlotIndex);
	bool SetPentagramSocketSlot(CItemObject *lpTargetItem, BYTE bt1RankOptionNum, BYTE bt1RankLevel, BYTE bt2RankOptionNum, BYTE bt2RankLevel, BYTE bt3RankOptionNum, BYTE bt3RankLevel, BYTE bt4RankOptionNum, BYTE bt4RankLevel, BYTE bt5RankOptionNum, BYTE bt5RankLevel, BYTE curRank);

	bool SwitchPentagramJewel(CGameObject &Obj, CItemObject *lpSourceItem, int iSwitchType);
	bool AddPentagramJewelInfo(CGameObject &Obj, int iJewelPos, int iJewelIndex, int iItemType, int iItemIndex, int iMainAttribute, int iJewelLevel, BYTE btRank1OptionNum, BYTE btRank1Level, BYTE btRank2OptionNum, BYTE btRank2Level, BYTE btRank3OptionNum, BYTE btRank3Level, BYTE btRank4OptionNum, BYTE btRank4Level, BYTE btRank5OptionNum, BYTE btRank5Level);
	bool DelPentagramJewelInfo(CGameObject &Obj, CItemObject *lpItemData);
	bool DelPentagramJewelInfo(CGameObject &Obj, int iJewelPos, int iJewelIndex);

	void DBREQ_GetPentagramJewel(CGameObject lpObj, char *szAccountId, int iJewelPos);
	void DBANS_GetPentagramJewel(LPBYTE lpRecv);
	void GCPentagramJewelInfo(CGameObject &Obj, int iJewelPos);
	void DBREQ_SetPentagramJewel(CGameObject &Obj, int iJewelPos);
	void DBREQ_DelPentagramJewel(CGameObject &Obj, int iJewelPos, int iJewelIndex);
	void DBREQ_InsertPentagramJewel(CGameObject &Obj, int iJewelPos, int iJewelIndex, int iItemType, int iItemIndex, int iMainAttribute, int iJewelLevel, BYTE btRank1, BYTE btRank1Level, BYTE btRank2, BYTE btRank2Level, BYTE btRank3, BYTE btRank3Level, BYTE btRank4, BYTE btRank4Level, BYTE btRank5, BYTE btRank5Level);

	int PentagramJewel_IN(CGameObject &Obj, int iPentagramItemPos, int iJewelItemPos);
	int PentagramJewel_OUT(CGameObject &Obj, int iPentagramItemPos, BYTE btSocketIndex, BYTE *btJewelPos, BYTE *btJewelDBIndex);

	bool GCTransPentagramJewelViewInfo(CGameObject &Obj, CItemObject *lpItemData);
	bool GCPShopPentagramJewelViewInfo(CGameObject &Obj, int aSourceIndex);
	bool AddRadianceSlot(BYTE TargetSlot);
	int IsEnableToTradePentagramItem(CGameObject &Obj);
	int IsEnableTransPentagramJewelInfo(CGameObject &Obj, int targetIndex);

	int CheckOverlapMythrilPiece(CGameObject &Obj, int iItemType, int iMainAttribute);

	int AddTradeCount(CGameObject lpObj, int source, int target);

	// fix drop slot pentagrams
	void LoadOptionMaps(const char* File);
	void LoadOptionNews();
	BOOL ElementDrop(CGameObject &Obj, CGameObject lpTargetObj);
	TRandomPoolMgr m_SlotCountRate;

	TEST_ITEMSDROP* Penta_DropRate;

	int m_iSpiritMap_DropLevel;
	int m_iSpiritMap_DropRate;

	int m_iMithril_DropRate;

	int m_iMurenBook_DropLevel;
	int m_iMurenBook_DropRate;

	int m_iEtramuScroll_DropLevel;
	int m_iEtramuScroll_DropRate;
private:

	SERVER_ATTRIBUTE_DEFINE* m_ServerAttributeDefine;
	PENTAGRAM_ITEM_OPEN_SOCKET_RATE* m_PentagramItemOpenSocketRate;
	MONSTER_DROP_ITEM_RATE* m_MonsterDropItemRate;
	JEWEL_OUT_RATE* m_JewelOutRate;
	PENTAGRAM_SET_EFFECT* m_AttackSetEffect;
	PENTAGRAM_SET_EFFECT* m_RelationShipSetEffect;
	PENTAGRAM_HAVE_SET_OPTION* m_PentagramHaveSetOption;
	PENTAGRAM_ITEM_OPTION* m_PentagramItemOption;
	PENTAGRAM_ITEM_OPTION_ENABLE_NEED* m_PentagramItemOptionEnableNeed;
	PENTAGRAM_SOCKET_RATE_BY_GRADE* m_PentagramSocketRateByGrade;
};

extern CPentagramSystem g_PentagramSystem;

#endif

////////////////////////////////////////////////////////////////////////////////
//  vnDev.Games - MuServer S12EP2 IGC v12.0.1.0 - Trong.LIVE - DAO VAN TRONG  //
////////////////////////////////////////////////////////////////////////////////

