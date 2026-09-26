
#include <mmo/ArcadiaServer.h>
#include <toolkit/ILock.h>
#include <toolkit/XEnv.h>
#include <toolkit/khash.h>
#include <logging/FileLog.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "StructItem.h"
#include "GameAllocator.h"
#include "Constant.h"
#include "GameContent.h"
#include "GameDBManager.h"
#include "StructPlayer.h"
#include "DB_Commands.h"
#include "GameRule.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "GameProc.h"


static volatile LONG											s_ItemCnt;
static std::vector< ItemBaseServer >							s_vItemBase;
static KHash< size_t, hashPr_mod_basic<ItemBase::ItemCode>  >	s_hsItemCode;
static KHash< size_t, hashPr_string_nocase >					s_hsItemName;
static std::vector< StructItem* >								s_vPendingItemForDelete;
static XCriticalSection											s_lockForItemDelete( "s_lockForItemDelete" );
static KHash< ItemBase::ItemCode, hashPr_mod_basic<ItemBase::ItemCode>  >		s_hsItemConvertingTable;

static StructItem *s_pDummyItem;

bool StructItem::RegisterItemBase( ItemBaseServer & base )
{
	// 이미 존재하면 수정
	if( s_hsItemCode.has( base.nCode ) )
	{
		size_t idx;
		s_hsItemCode.lookup( base.nCode, idx );
		s_vItemBase[idx] = base;
		
		return true;
	}

	if (base.nCode > 603100 && base.nCode < 603200)
		base.WearType = ItemBase::WEAR_EMBLEM;

	if (base.nCode == 603003)
		base.WearType = ItemBase::WEAR_BOOSTER;

	if (base.nCode == 804000)
	{
		base.WearType = ItemBase::WEAR_CHAOS_STONE;
		base.nOptType[0] = 27;
		base.fOptVar2[0] = 1000;
		base.nRank = 1;
	}

	//if (base.nClass == ItemBase::CLASS_CROSSBOW)
	//{
		//base.WearType = ItemBase::WEAR_WEAPON;
	//}

	if ((base.WearType != 27) && (base.nOptType[1] == 27)) {
		base.nOptType[1] = 0;
		base.fOptVar2[1] = 0;
	}

	s_vItemBase.push_back( base );

	s_hsItemCode.add( base.nCode, s_vItemBase.size()-1 );
	s_hsItemName.add( GameContent::GetString( base.nNameId ), s_vItemBase.size()-1 );

	return true;
}

bool StructItem::RegisterConvertingInfo( ItemBase::ItemCode _key, ItemBase::ItemCode _value )
{
	if( s_hsItemConvertingTable.add( _key, _value ) == NULL )
		return false;

	return true;
}

const char* StructItem::GetName() const
{
	return GameContent::GetString( GetItemBase().nNameId );
}

std::string	StructItem::GetNameInGame()
{
	std::string strName( GameContent::GetString( GetItemBase().nNameId ) );

	std::string strGameName;

	int cnt = 0;
	for( std::string::iterator it = strName.begin() ; it != strName.end() ; ++it )
	{
		if( (*it) == '\n' || (*it) == '\r' || (*it) == '\n' )
			continue;

		if( (*it) == '<' )
		{
			++cnt;
			continue;
		}

		if( (*it) == '>' )
		{
			--cnt;
			continue;
		}

		if( cnt <= 0 )
			strGameName += (*it);
	}

	return strGameName;
}

bool StructItem::IsValidItemCode( ItemBase::ItemCode code )
{
	size_t idx;
	if( !s_hsItemCode.lookup( code, idx ) )
	{
		return false;
	}
	return true;
}

ItemBaseServer & StructItem::GetItemBase( ItemBase::ItemCode c )
{
	static ItemBaseServer gold;
	if( !c )
	{
		return gold;
	}

	size_t idx;
	if( !s_hsItemCode.lookup( c, idx ) )
	{
		FILELOG( "invalid item code[%d]", c );
		return gold;
	}

	return s_vItemBase[ idx ];
}

ItemBase::ItemCode StructItem::GetItemCode( const char *szItemName )
{
	size_t idx;

	if( s_hsItemName.lookup( szItemName, idx ) ) return s_vItemBase[idx].nCode;

	return 0;
}

StructItem*	StructItem::AllocGold( const StructGold & gold,ItemInstance::GenerateCode gcode )
{
	return AllocItem( 0, 0, gold.GetRawData(), gcode, 0 );
}

StructItem* StructItem::FindItem( AR_HANDLE handle )
{
	return static_cast< StructItem* >( GameObject::raw_get( handle ) );
}

const int duraEffects[] = { 100932, 100932,  100965, 100954, 100976, 100987, 100998, 0, 101020, 100921, 100921, 101009, 100943,
				  400089, 400089,  400161, 400137, 400185, 400209, 400233, 0, 400041, 400065, 400065, 400017, 400113 };


const StructDuraConvert duraRank2[] = {
	{101, 4, 0, {101201,101202,101203,101204,0,0,0,0,0,0} },
	{102, 3, 12, {102201,102202,102203,0,0,0,0,0,0,0} },
	{103, 3, 0, {103201,103202,103203,0,0,0,0,0,0,0} },
	{104, 3, 12, {104201,104202,104203,0,0,0,0,0,0,0} },
	{105, 3, 12, {105201,105202,105203,0,0,0,0,0,0,0} },
	{106, 3, 0, {106201,106202,106203,0,0,0,0,0,0,0} },
	{107, 2, 12, {107201,107202,0,0,0,0,0,0,0,0} },
	{108, 2, 12, {108201,108202,0,0,0,0,0,0,0,0} },
	{110, 1, 12, {109201,0,0,0,0,0,0,0,0,0} },
	{111, 2, 0, {110201,110202,0,0,0,0,0,0,0,0} },
	{112, 3, 12, {111201,111202,111203,0,0,0,0,0,0,0} },
	{113, 1, 0, {112201,0,0,0,0,0,0,0,0,0} },
	{201, 4, 2, {221201,221202,221203,221204,0,0,0,0,0,0} },
	{202, 4, 2, {222201,222202,222203,222204,0,0,0,0,0,0} },
	{203, 3, 2, {223201,223202,223203,0,0,0,0,0,0,0} },
	{204, 5, 2, {224201,224202,224203,32042108,32042112,0,0,0,0,0} },
	{210, 2, 1, {301201,301202,111203,0,0,0,0,0,0,0} },
	{220, 1, 3, {302201,0,0,0,0,0,0,0,0,0} },
	{230, 3, 5, {304201,304202,304203,0,0,0,0,0,0,0} },
	{240, 3, 4, {303201,303202,303203,0,0,0,0,0,0,0} },
	{250, 1, 6, {305201,0,0,0,0,0,0,0,0,0} },
	{301, 7, 9, {432101,432102,432103,432104,432105,432106,432107,0,0,0} },
	{302, 4, 11, {422101,422102,422103,422104,0,0,0,0,0,0} },
	{303, 3, 8, {412100,412201,412202,0,0,0,0,0,0,0} },
};

const StructDuraConvert duraRank3[] = {
	{101, 3, 0, {101301,101302,101303,0,0,0,0,0,0,0} },
	{102, 2, 12, {102301,102302,0,0,0,0,0,0,0,0} },
	{103, 2, 0, {103301,103302,103203,0,0,0,0,0,0,0} },
	{104, 3, 12, {104301,104302,104303,0,0,0,0,0,0,0} },
	{105, 3, 12, {105301,105302,105303,0,0,0,0,0,0,0} },
	{106, 3, 0, {106301,106302,106303,0,0,0,0,0,0,0} },
	{107, 3, 12, {107301,107302,107303,0,0,0,0,0,0,0} },
	{108, 3, 12, {108301,108302,108303,0,0,0,0,0,0,0} },
	{110, 2, 12, {109301,109302,0,0,0,0,0,0,0} },
	{111, 2, 0, {110301,110302,0,0,0,0,0,0,0,0} },
	{112, 3, 12, {111301,111302,111303,0,0,0,0,0,0,0} },
	{113, 2, 0, {112301,112302,0,0,0,0,0,0,0,0} },

	{201, 7, 2, {221301,221302,231301,231302,241301,241302,261303,0,0,0} },
	{202, 7, 2, {222301,222302,232301,232302,242301,242302,262303,0,0,0} },
	{203, 7, 2, {223301,223302,233301,233302,243301,243302,263303,0,0,0} },
	{204, 7, 2, {224301,224302,234301,234302,244301,244302,264303,0,0,0} },
	{210, 3, 1, {301301,301302,301303,0,0,0,0,0,0,0} },
	{220, 2, 3, {302301,302301,0,0,0,0,0,0,0,0} },
	{230, 4, 5, {304301,304302,304303,304304,0,0,0,0,0,0} },
	{240, 4, 4, {303301,303302,303303,303304,0,0,0,0,0,0} },
	{250, 1, 6, {305301,0,0,0,0,0,0,0,0,0} },
	{301, 7, 9, {433101,433102,433103,433104,433105,433106,433107,0,0,0} },
	{302, 4, 11, {423101,423102,423103,423104,0,0,0,0,0,0} },
	{303, 3, 8, {413201,413202,413100,0,0,0,0,0,0,0} },

};

const StructDuraConvert duraRank4[] = {
	{101, 3, 0, {101401,101402,101403,0,0,0,0,0,0,0} },
	{102, 4, 12, {102401,102402,102403,102404,0,0,0,0,0,0} },
	{103, 3, 0, {103401,103402,103403,0,0,0,0,0,0,0} },
	{104, 3, 12, {104401,104402,104403,0,0,0,0,0,0,0} },
	{105, 2, 12, {105401,105402,0,0,0,0,0,0,0,0} },
	{106, 3, 0, {106401,106402,106403,0,0,0,0,0,0,0} },
	{107, 2, 12, {107401,107402,0,0,0,0,0,0,0,0} },
	{108, 2, 12, {108401,108402,0,0,0,0,0,0,0,0} },
	{110, 2, 12, {109401,109402,0,0,0,0,0,0,0} },
	{111, 3, 0, {110401,110402,110403,0,0,0,0,0,0,0} },
	{112, 2, 12, {111401,111402,0,0,0,0,0,0,0,0} },
	{113, 3, 0, {112401,112402,112403,0,0,0,0,0,0,0} },

	{201, 4, 2, {221401,231401,241401,261402,0,0,0,0,0,0} },
	{202, 4, 2, {222401,232401,242401,262402,0,0,0,0,0,0} },
	{203, 4, 2, {223401,233401,243401,263402,0,0,0,0,0,0} },
	{204, 4, 2, {224401,234401,244401,264402,0,0,0,0,0,0} },
	{210, 2, 1, {301401,301402,0,0,0,0,0,0,0,0} },
	{220, 2, 3, {302401,302402,0,0,0,0,0,0,0,0} },
	{230, 3, 5, {304401,304402,304403,0,0,0,0,0,0,0} },
	{240, 3, 4, {303401,303402,303403,0,0,0,0,0,0,0} },
	{250, 1, 6, {305401,0,0,0,0,0,0,0,0,0} },
	{301, 7, 9, {434101,434102,434103,434104,434105,434106,434107,0,0,0} },
	{302, 4, 11, {424101,424102,424103,0,0,0,0,0,0,0} },
	{303, 3, 8, {414201,414202,414100,0,0,0,0,0,0,0} },

};

const StructDuraConvert duraRank5[] = {
	{101, 3, 0, {101501,101502,101503,0,0,0,0,0,0,0} },
	{102, 3, 12, {102501,102502,102523,0,0,0,0,0,0,0} },
	{103, 2, 0, {103501,103502,0,0,0,0,0,0,0,0} },
	{104, 3, 12, {104501,104502,104503,0,0,0,0,0,0,0} },
	{105, 4, 12, {105523,105502,105501,105531,0,0,0,0,0,0} },
	{106, 3, 0, {106501,106522,106531,0,0,0,0,0,0,0} },
	{107, 3, 12, {107501,107502,107503,0,0,0,0,0,0,0} },
	{108, 3, 12, {108501,108523,1085012,0,0,0,0,0,0,0} },
	{110, 2, 12, {109501,109502,0,0,0,0,0,0,0} },
	{111, 3, 0, {110501,110502,110523,0,0,0,0,0,0,0} },
	{112, 2, 12, {111501,111523,0,0,0,0,0,0,0,0} },
	{113, 3, 0, {112501,112523,112502,0,0,0,0,0,0,0} },

	{201, 4, 2, {221501,231501,261501,261502,0,0,0,0,0,0} },
	{202, 4, 2, {222501,232501,262501,262502,0,0,0,0,0,0} },
	{203, 4, 2, {223501,233501,263501,263502,0,0,0,0,0,0} },
	{204, 4, 2, {224501,234501,264501,264502,0,0,0,0,0,0} },
	{210, 2, 1, {301501,301503,0,0,0,0,0,0,0,0} },
	{220, 1, 3, {302501,0,0,0,0,0,0,0,0,0} },
	{230, 1, 5, {304501,0,0,0,0,0,0,0,0,0} },
	{240, 1, 4, {303501,0,0,0,0,0,0,0,0,0} },
	{250, 1, 6, {305501,0,0,0,0,0,0,0,0,0} },
	{301, 7, 9, {435001,435002,435003,435004,435005,435006,435007,0,0,0} },
	{302, 4, 11, {425001,425002,425003,425004,0,0,0,0,0,0} },
	{303, 3, 8, {415101,415102,415100,0,0,0,0,0,0,0} },

};


const StructDuraConvert duraRank6[] = {
	{101, 4, 0, {101601,101624,101602,101603,0,0,0,0,0,0} },
	{102, 3, 12, {102601,102623,102602,0,0,0,0,0,0,0} },
	{103, 3, 0, {103601,103623,103602,0,0,0,0,0,0,0} },
	{104, 3, 12, {104601,104623,104602,0,0,0,0,0,0,0} },
	{105, 3, 12, {105601,105623,105602,0,0,0,0,0,0,0} },
	{106, 3, 0, {106601,106623,106602,0,0,0,0,0,0,0} },
	{107, 3, 12, {107601,107623,107602,0,0,0,0,0,0,0} },
	{108, 3, 12, {108601,108623,108602,0,0,0,0,0,0,0} },
	{110, 3, 12, {109601,109623,109602,0,0,0,0,0,0} },
	{111, 3, 0, {110601,110623,110602,0,0,0,0,0,0,0} },
	{112, 3, 12, {111601,111624,111602,0,0,0,0,0,0,0} },
	{113, 3, 0, {112601,112623,112602,0,0,0,0,0,0,0} },

	{201, 1, 2, {261601,0,0,0,0,0,0,0,0,0} },
	{202, 1, 2, {262601,0,0,0,0,0,0,0,0,0} },
	{203, 1, 2, {263601,0,0,0,0,0,0,0,0,0} },
	{204, 1, 2, {264601,0,0,0,0,0,0,0,0,0} },
	{210, 2, 1, {301601,301603,0,0,0,0,0,0,0,0} },
	{220, 1, 3, {302601,0,0,0,0,0,0,0,0,0} },
	{230, 1, 5, {304601,0,0,0,0,0,0,0,0,0} },
	{240, 1, 4, {303601,0,0,0,0,0,0,0,0,0} },
	{250, 1, 6, {305601,0,0,0,0,0,0,0,0,0} },
	{301, 7, 9, {436001,436002,436003,436004,436005,436006,436007,0,0,0} },
	{302, 4, 11, {426001,426002,426003,426004,0,0,0,0,0,0} },
	{303, 3, 8, {416100,416101,416102,0,0,0,0,0,0,0} },

};

const StructDuraConvert duraRank7[] = {
	{101, 5, 0, {101701,101702,101703,101724,101705,0,0,0,0,0} },
	{102, 5, 12, {102701,102702,102703,102724,102705,0,0,0,0,0} },
	{103, 5, 0, {103701,103702,103703,103724,103705,0,0,0,0,0} },
	{104, 5, 12, {104701,104702,104703,104724,104705,0,0,0,0,0} },
	{105, 5, 12, {105701,105702,105703,105724,105705,0,0,0,0,0} },
	{106, 5, 0, {106701,106702,106703,106724,106705,0,0,0,0,0} },
	{107, 5, 12, {107701,107702,107703,107724,107705,0,0,0,0,0} },
	{108, 5, 12, {108701,108702,108703,108724,108705,0,0,0,0,0} },
	{110, 5, 12, {109701,109702,109703,109724,109705,0,0,0,0} },
	{111, 5, 0, {110701,110702,110703,110724,110705,0,0,0,0,0} },
	{112, 5, 12, {111701,111702,111703,111725,111706,0,0,0,0,0} },
	{113, 5, 0, {112701,112702,112703,112724,112705,0,0,0,0,0} },

	{201, 4, 2, {271701,271702,261701,271703,0,0,0,0,0,0} },
	{202, 4, 2, {272701,272702,262701,272703,0,0,0,0,0,0} },
	{203, 4, 2, {273701,273702,263701,273703,0,0,0,0,0,0} },
	{204, 4, 2, {274701,274702,264701,274703,0,0,0,0,0,0} },
	{210, 3, 1, {301702,301703,301704,0,0,0,0,0,0,0} },
	{220, 3, 3, {302749,32207202,32207103,0,0,0,0,0,0,0} },
	{230, 4, 5, {304701,304702,304703,304704,0,0,0,0,0,0} },
	{240, 4, 4, {303701,303702,303703,303704,0,0,0,0,0,0} },
	{250, 8, 6, {305701,305702,305703,305701,305702,305703,305740,305740} },
	{301, 7, 9, {437001,437001,437003,437004,437005,437006,437007,0,0,0} },
	{302, 4, 11, {427001,427002,427003,427004,0,0,0,0,0,0} },
	{303, 3, 8, {33037101,33037102,33037103,0,0,0,0,0,0,0} },

};

bool StructItem::SwapDura(ItemBase::ItemCode& code, int &effect_id)
{
	int c = code;
	int orgCode = code;
	int nClass = code / 1000000;
	c -= nClass * 1000000;

	int nDuraRank = c / 100000;
	c -= nDuraRank * 100000; 

	int nGrade = c / 10000; 
	c -= nGrade * 10000;
	int nItemIdx = c / 100;
	c -= nItemIdx * 100;
	int nEffectIdx = c;


	bool res = false;

	if (nDuraRank == 2)
	{
		for (int i = 0; i < _countof(duraRank2); i++)
		{
			const StructDuraConvert* dr = &duraRank2[i];
			if (dr->nClass == nClass)
			{
				int idx = nItemIdx < dr->nCnt ? nItemIdx : XRandom(0, dr->nCnt - 1);
				code = dr->anCodes[idx];
				int effIdx = nEffectIdx < 12 ? nEffectIdx : XRandom(0, 11);
				effect_id = duraEffects[dr->nWear] + effIdx;
				res = true;
				break;
			}
		}
	}
	else if (nDuraRank == 3)
	{
		for (int i = 0; i < _countof(duraRank3); i++)
		{
			const StructDuraConvert* dr = &duraRank3[i];
			if (dr->nClass == nClass)
			{
				int idx = nItemIdx < dr->nCnt ? nItemIdx : XRandom(0, dr->nCnt - 1);
				code = dr->anCodes[idx];
				int effIdx = nEffectIdx < 12 ? nEffectIdx : XRandom(0, 11);
				effect_id = duraEffects[dr->nWear] + effIdx;
				res = true;
				break;
			}
		}
	}
	else if (nDuraRank == 4)
	{
		for (int i = 0; i < _countof(duraRank4); i++)
		{
			const StructDuraConvert* dr = &duraRank4[i];
			if (dr->nClass == nClass)
			{
				int idx = nItemIdx < dr->nCnt ? nItemIdx : XRandom(0, dr->nCnt - 1);
				code = dr->anCodes[idx];
				int effIdx = nEffectIdx < 12 ? nEffectIdx : XRandom(0, 11);
				effect_id = duraEffects[dr->nWear] + effIdx;
				res = true;
				break;
			}
		}
	}
	else if (nDuraRank == 5)
	{
		for (int i = 0; i < _countof(duraRank5); i++)
		{
			const StructDuraConvert* dr = &duraRank5[i];
			if (dr->nClass == nClass)
			{
				int idx = nItemIdx < dr->nCnt ? nItemIdx : XRandom(0, dr->nCnt - 1);
				code = dr->anCodes[idx];
				int effIdx = nEffectIdx < 12 ? nEffectIdx : XRandom(0, 11);
				effect_id = duraEffects[dr->nWear] + effIdx;
				res = true;
				break;
			}
		}
	}
	else if (nDuraRank == 6)
	{
		for (int i = 0; i < _countof(duraRank6); i++)
		{
			const StructDuraConvert* dr = &duraRank6[i];
			if (dr->nClass == nClass)
			{
				int idx = nItemIdx < dr->nCnt ? nItemIdx : XRandom(0, dr->nCnt - 1);
				code = dr->anCodes[idx];
				int effIdx = nEffectIdx < 12 ? nEffectIdx : XRandom(0, 11);
				effect_id = duraEffects[dr->nWear] + effIdx;
				res = true;
				break;
			}
		}
	}
	else if (nDuraRank == 7)
	{
		for (int i = 0; i < _countof(duraRank7); i++)
		{
			const StructDuraConvert* dr = &duraRank7[i];
			if (dr->nClass == nClass)
			{
				int idx = nItemIdx < dr->nCnt ? nItemIdx : XRandom(0, dr->nCnt - 1);
				code = dr->anCodes[idx];
				int effIdx = nEffectIdx < 12 ? nEffectIdx : XRandom(0, 11);
				effect_id = duraEffects[dr->nWear] + effIdx;
				res = true;
				break;
			}
		}
	}


/*
	if (nDuraRank == 2)
	{
		switch (nClass)
		{


		}

	}
*/
	if(!res)
		_cprint("Dura Swap: %d (class %d, dura rank %d, grade %d, item idx %d, effect idx %d)\n", orgCode, nClass, nDuraRank, nGrade, nItemIdx, nEffectIdx);
	else
		_cprint("Dura Swap: %d (class %d, dura rank %d, grade %d, item idx %d, effect idx %d) - Code: %d, effect: %d\n", orgCode, nClass, nDuraRank, nGrade, nItemIdx, nEffectIdx,code,effect_id);


	return res;
}


StructItem*  StructItem::AllocItem(	ItemUID	uid,
									ItemBase::ItemCode		code,
									const __int64 &			cnt,									
									ItemInstance::GenerateCode info,
									int						level,
									int						enhance,
									int						flag,
									ItemBase::ItemCode		socket_0,
									ItemBase::ItemCode		socket_1,
									ItemBase::ItemCode		socket_2,
									ItemBase::ItemCode		socket_3,
									int						awaken_sid,
									int						identified_sid,
									int						remain_time,
									const unsigned char &	elemental_effect_type,
									const time_t &			elemental_effect_expire_time,
									const int				elemental_effect_attack_point,
									const int				elemental_effect_magic_point,
									const ItemBase::ItemCode		appearance_code,
									int						summon_code,
									int						effect_id)
{
	// 돈 객체를 생성하는 경우 (아이템 코드가 0) cnt가 0일 수 있다.
	assert( cnt > 0 || code == 0 );

	StructItem* p;

	InterlockedIncrement( &s_ItemCnt );

	struct _myIntializer : GameAllocateFunctor
	{
		virtual void operator()( void * pObj, AR_HANDLE handle )
		{
			new (pObj) StructItem( handle );
		}
	};

	AR_HANDLE handle = allocItemStruct( &p, _myIntializer() );
	// _oprint( "ALLOC ITEM : %08X\n", p );

	if( handle == 0 )
	{
		s_pDummyItem = p;

		handle = allocItemStruct( &p, _myIntializer() );
	}
	//new (p) StructItem( handle );

	assert( code >= 0 );

	bool bUpdateCode = false;

	if (code > 100000000 && code < 305000000)
	{
		bUpdateCode = SwapDura(code, effect_id);
	}

	p->m_pItemBase = &GetItemBase( code );
	
	p->m_Instance.UID		= uid;
	p->m_Instance.Code		= code;
	p->m_Instance.PreviousUID = 0;
	p->m_Instance.nCount	= cnt;
	p->m_Instance.nLevel	= ( level == -1 ) ? p->m_pItemBase->nLevel : level ;		// 설정되지 않았으면 ItemResource에 있는대로 설정
	if( p->m_Instance.nLevel <= 0 )
	{
		// 돈은 level이 0일 수 있음 - 실질적인 아이템이 아니므로 이래도 저래도 관계 없음
		assert( p->m_pItemBase->nCode == 0 );
		p->m_Instance.nLevel = 1;
	}
	p->m_Instance.nEnhance	= ( enhance == -1 ) ? p->m_pItemBase->nEnhance : enhance ;	// 설정되지 않았으면 ItemResource에 있는대로 설정
	if( p->m_Instance.nEnhance < 0 )
	{
		assert( 0 );
		p->m_Instance.nEnhance = 0;
	}
	if( p->m_pItemBase->nGroup == ItemBase::GROUP_SKILLCARD && !p->m_Instance.nEnhance ) p->m_Instance.nEnhance = 1;	// 스킬카드는 기본이 1 
	p->m_Instance.GenerateInfo = info;
	p->m_Instance.nWearInfo = static_cast< ItemBase::ItemWearType >( -1 );
	
	p->m_Instance.OwnerHandle = 0;
	p->m_Instance.nOwnerUID = 0;

	p->m_Instance.Socket[0] = socket_0;
	p->m_Instance.Socket[1] = socket_1;
	p->m_Instance.Socket[2] = socket_2;
	p->m_Instance.Socket[3] = socket_3;
	p->m_Instance.RandomOption[ ItemInstance::AWAKEN ].nSID = awaken_sid;
	p->m_Instance.RandomOption[ ItemInstance::IDENTIFIED ].nSID = identified_sid;
	p->m_Instance.cElementalEffectType = elemental_effect_type;
	p->m_Instance.tElementalEffectExpire = elemental_effect_expire_time;
	p->m_Instance.nElementalEffectAttackPoint = elemental_effect_attack_point;
	p->m_Instance.nElementalEffectMagicPoint = elemental_effect_magic_point;
	p->m_Instance.nAppearanceCode = appearance_code;
	p->m_Instance.nSummonCode = summon_code;
	p->m_Instance.nEffectID = effect_id;

	p->m_Instance.nCurrentEtherealDurability = ( p->IsEtherealStone() ) ? 0 : p->GetItemBase().nEtherealDurability;
	p->m_Instance.nCurrentEndurance = p->GetItemBase().nEndurance;

	p->m_hBindedTarget		= 0;

	p->GetInstanceFlag().CopyFrom( ( flag == -1 ) ? &p->m_pItemBase->nInstanceFlag : &flag );

	if (bUpdateCode)
	{
		p->DBQuery(new DB_UpdateItemCode(p));
		//p->DBQuery(new DB_UpdateItem(p));
	}

	if( remain_time == -1 )
	{
		if( p->IsExpireItem() )
		{
			p->m_Instance.tExpire = time( NULL ) + p->GetItemBase().available_time;
		}
		else
		{
			p->m_Instance.tExpire = 0;
		}
	}
	else
	{
		if( p->GetItemBase().decrease_type == ItemBase::DECREASE_ALWAYS )
		{
			p->m_Instance.tExpire = remain_time;
		}
		else
		{
			p->m_Instance.tExpire = time( NULL ) + remain_time;
		}
	}

	if( !p->IsJoinable() && !p->IsGold() && p->m_Instance.nCount > 1 )
		p->m_Instance.nCount = 1;

	// 아이템 변환 테이블. 옛날 코드를 지정된 코드로 변환시켜준다.
	if( s_hsItemConvertingTable.has( code ) )
	{
		ItemBase::ItemCode nCode = -1;
		s_hsItemConvertingTable.lookup( code, nCode );
		p->m_Instance.Code = nCode;
		p->m_pItemBase = &GetItemBase( nCode );
		
		if( uid )
		{
			// uid가 있을 경우에는 DB안의 값도 바꿔야 하므로 로그를 남겨준다.
			// AllocItem 안에서는 주인 정보가 없어서 n1, n2 컬럼(아이템의 주인정보)을 기록하지 못하는 단점이..
			LOG::Log11N4S( LM_ITEM_CONVERT, 0, 
				0, p->GetItemEnhance() * 100 + p->GetItemLevel(), 
				code, nCode,
				p->GetCount(), 0,
				0, 0,
				0, p->GetItemUID(),
				"", 0, 
				"", 0,
				"", 0,
				"ITEM_CONVERT_BY_ALLOC_ITEM", LOG::STR_NTS );
			p->DBQuery( new DB_UpdateItemCode( p ) );
		}
	}

	return p;
}

void StructItem::PendFreeItem_( StructItem* p, const char* function, const int line )
{
	static int cnt;
	static AR_TIME prev_proc_time = GetArTime();

	if( function != NULL )
	{
		p->m_deleteFunction = function;
		p->m_deleteLine = line;
	}

	// 소환수/펫 아이템이 삭제될 때 함께 삭제되도록 ArcadiaServer::Instance().DeleteObject 를 호출하려면 s_lockForItemDelete가 풀린 상태여야 함
	StructCreature * pSummonPet = NULL;
	{
		THREAD_SYNCRONIZE( &s_lockForItemDelete );

		//_oprint( "PEND FREE ITEM : %08X\n", p );

		if( std::find( s_vPendingItemForDelete.begin(), s_vPendingItemForDelete.end(), p ) != s_vPendingItemForDelete.end() )
		{
			assert( 0 );

			return;
		}

		s_vPendingItemForDelete.push_back( p );
		p->bIsDeleteRequested = true;

		// 소환수 카드이고 테이밍 되어 있던 카드일 경우 소환수 제거
		if( p->IsSummonCard() && p->GetSummonStruct() )
		{
			// 역소환 처리가 된 후에 아이템을 삭제해야 하지만, 어디선가 소환된 소환수의 카드를 삭제 시도한 경우
			if( p->GetSummonStruct()->IsInWorld() )
			{
				assert( 0 );

				// 급한 대로 월드에서 제거
				RemoveSummonFromWorld( p->GetSummonStruct() );
			}

			pSummonPet = p->GetSummonStruct();
		}
		// 펫 카드일 경우 소속되어 있던 펫 제거
		else if( p->IsPetCage() && p->GetPetStruct() )
		{
			// 역소환 처리가 된 후에 아이템을 삭제해야 하지만, 어디선가 소환된 소환수의 카드를 삭제 시도한 경우
			if( p->GetPetStruct()->IsInWorld() )
			{
				assert( 0 );

				// 급한 대로 월드에서 제거
				RemovePetFromWorld( p->GetPetStruct() );
			}

			pSummonPet = p->GetPetStruct();
		}

#ifdef _DEBUG
		// 임시
		deletePendingItem();
#endif // _DEBUG

		if( ++cnt > 10 )
		{
			cnt = 0;
			AR_TIME t = GetArTime();

			if( prev_proc_time + 100 < t )
			{
				prev_proc_time = t;
				deletePendingItem();
			}
		}
	}

	if( pSummonPet )
	{
		ArcadiaServer::Instance().DeleteObject( pSummonPet );
	}
}

void StructItem::deletePendingItem()
{	
	std::vector< StructItem* >	vTmp;

	std::vector< StructItem* >::iterator it;

	for( it = s_vPendingItemForDelete.begin(); it != s_vPendingItemForDelete.end(); ++it )
	{
		if( !(*it)->IsDeleteable() )
		{
			vTmp.push_back( *it );
			continue;
		}

		//_oprint( "DELETE ITEM : %08X\n", (*it) );
		freeItem( *it );		
	}

	s_vPendingItemForDelete.swap( vTmp );
}

void StructItem::InitItemSystem()
{	
	ENV().Bind( "game.item_count", (int*)&s_ItemCnt );
}

void StructItem::DeInitItemSystem()
{
	if( s_pDummyItem )
		freeItemStruct( s_pDummyItem );

	THREAD_SYNCRONIZE( &s_lockForItemDelete );

	while( !s_vPendingItemForDelete.empty() ) deletePendingItem();
}

void StructItem::freeItem( StructItem* p )
{
	// _oprint( "DEL ITEM : %08X\n", p );

	InterlockedDecrement( &s_ItemCnt );

	prepareFreeItemStruct( p );

	p->StructItem::~StructItem();	

	//memset( p, 0xaa, sizeof(StructItem) );	// 임시, 디버깅용

	freeItemStruct( p );
}

bool StructItem::SetItemUID( ItemUID uid )
{
	if( m_Instance.UID ) return false;
	m_Instance.UID = uid;
	return true;
}

bool StructItem::ChangeItemCode( ItemBase::ItemCode code )
{
	// 잘못 된 아이템 코드일 경우 안 바꿔줌
	ItemBaseServer *pBase = &GetItemBase( code );
	if( pBase->nCode == 0 )
		return false;

	m_pItemBase = pBase;
	m_Instance.Code = code;
	
	// 업데이트 플래그는 건들지않는다. (위 단 핸들러에서 판단)
	DBQuery( new DB_UpdateItemCode( this ) );
	return true;
}

StructItem::StructItem( AR_HANDLE handle )
: m_bQueryLock( "StructItem::m_bQueryLock" )
{	
	m_nArObjectType = ArObject::STATIC_OBJECT;

	m_bIsVirtualItem = false;
	m_bIsEventDrop = false;
	m_Instance.nLevel = 1;
	m_Instance.nEnhance = 1;

	m_hHandle = handle;

	m_nAccountID = 0;
	//m_nOwnSummonUID	= -1;

	m_unInventoryIndex = 0;	// 리스트에서의 위치

	m_nDBUpdateFlag = 0;	

	m_nDropTime			= 0;	

	m_pSummon			= NULL;
	m_pPet				= NULL;

	m_lQueryList.clear();

	memset( &m_ItemPickupOrder, 0, sizeof( m_ItemPickupOrder ) );

#ifdef _MEM_USAGE_DEBUG
	XSEH::IncreaseAllocCount( "StructItem" );
#endif

	m_deleteFunction = NULL;
	m_deleteLine = 0xFFFFFFFF;
}

StructItem::~StructItem()
{
#ifdef _MEM_USAGE_DEBUG
	XSEH::DecreaseAllocCount( "StructItem" );
#endif
}

bool StructItem::ProcDelete()
{
	if( bIsDeleted ) assert( 0 );

	StructItem::freeItem( this );

	return true;
}

bool StructItem::IsWearable() const
{
	// 장착 불가 아이템
	if( GetWearType() == ItemBase::WEAR_CANTWEAR )
		return false;

	// 강화 실패작
	if( GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FAILED ) )
		return false;

	// 에테리얼 내구도 소모로 인한 파괴
	if( GetMaxEtherealDurability() > 0 && GetCurrentEtherealDurability() <= 0 )
		return false;

	// 랜덤 옵션 아이템은 오픈하지 않았다면 장착 불가
	if( IsRandomizable() && !IsIdentified() )
	{
		return false;
	}

	return true;
}

int StructItem::GetMaxSocketCount() const
{
	int nSocketCount = GetItemBase().nSocketCount;

	if( IsRandomizable() )
	{
		if( IsIdentified() )
		{
			// 밸트 소켓과 랜덤옵션의 소켓과는 다르다.
			if( IsBelt() == false )
			{
				for( int i = 0 ; i < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++i )
				{
					if( GetIdentifiedOptionType( i ) == ITEM_EFFECT_PASSIVE::INC_SOCKET_COUNT_A || GetIdentifiedOptionType( i ) == ITEM_EFFECT_PASSIVE::INC_SOCKET_COUNT_B )
					{
						nSocketCount += GetIdentifiedOptionValue1( i );
					}
				}
				if( nSocketCount > ItemBase::MAX_SOCKET_NUMBER )
					nSocketCount = ItemBase::MAX_SOCKET_NUMBER;
			}
		}
		else
		{
			// 미확인 랜덤 옵션
			nSocketCount = 0;
		}
	}

	return nSocketCount;
}

int StructItem::GetUsingSocketCount() const
{
	int nUsableSocketNumber = GetMaxSocketCount();

	if( !nUsableSocketNumber )
		return 0;

	int nCount = 0;
	for( int i = 0 ; i < nUsableSocketNumber ; ++i )
	{
		ItemBase::ItemCode nSocketCode = GetSocketCode( i );

		if( nSocketCode )
			++nCount;
	}

	return nCount;
}

// Instance에 박힌 SummonCode 우선 (베이스, 인스턴스 서몬 코드 둘다 있으면 인스턴스 정보로 줌)
int	StructItem::GetSummonCode() const
{
	if( m_Instance.nSummonCode )
		return m_Instance.nSummonCode;
	
	return m_pItemBase->nSummonId;
}


void StructItem::SetItemEnhance( int enhance )
{
	if( m_Instance.nEnhance != enhance )
	{
		m_Instance.nEnhance = enhance;
		GameObject* object = GameObject::raw_get( GetOwnerHandle() );
		if( object && object->IsPlayer() )
		{
			static_cast< StructPlayer* >( object )->UpdateQuestStatusByItemEnhance( GetItemCode() );
		}
		TurnOnUpdateFlag();
	}
}

const int StructItem::ProcEtherealDurabilityConsumption( const int nBaseConsumption, const c_fixed10 & fConsumeRate, const c_fixed10 & fEnvironmentalConsumeRate )
{
	// 상급 아이템이 아니거나 에테리얼 내구도가 처음부터 0이거나 남은 에테리얼 내구도가 0이면 불가
	// 혹은 벨트 장착용 보스 카드
	if( !( GetItemGrade() && GetMaxEtherealDurability() && GetCurrentEtherealDurability() ) && !IsEquipmentOnBelt() )
		return 0;

	// 소모율 계산 마무리 + 기본 소모량 적용
	c_fixed10 fConsumption( fConsumeRate + GameRule::GetEtherealDurabilityConsumeRateByItem( GetItemRank(), GetItemGrade() ) );
	fConsumption *= nBaseConsumption;

	// 상황에 따른 소모율 적용
	fConsumption *= fEnvironmentalConsumeRate;

	// 소모될 에테리얼 내구도가 0 이하면 패스
	if( static_cast< int >( fConsumption ) <= 0 )
		return 0;

	int nPrevEtherealDurability = GetCurrentEtherealDurability();

	AddCurrentEtherealDurability( -1 * fConsumption );

	return nPrevEtherealDurability - GetCurrentEtherealDurability();
}

const int StructItem::GetMaxEtherealDurability() const
{
	// 크리처 카드는 강화 정도에 따라 내구도가 다르다.
	if( IsSummonCard() )
		return GameContent::GetCreatureEnhanceInfo( GetItemEnhance() )->card_durability;

	return GetItemBase().nEtherealDurability; 
}

void	StructItem::SetCurrentEndurance( int n )
{
	int nPrevEndurance = m_Instance.nCurrentEndurance;
	m_Instance.nCurrentEndurance = std::max( std::min( n, GetMaxEndurance() ), 0 );
	if( nPrevEndurance != m_Instance.nCurrentEndurance )
	{
		TurnOnUpdateFlag();
	}
}


int StructItem::GetMaxEndurance() const
{
	int nUsableSocketNumber = GetMaxSocketCount();

	if( !nUsableSocketNumber )
		return GetItemBase().nEndurance;

	int nMaxEndurance = 0;
	for( int i = 0 ; i < nUsableSocketNumber ; ++i )
	{
		ItemBase::ItemCode nSocketCode = GetSocketCode( i );

		if( !nSocketCode )
			continue;

		nMaxEndurance += GetItemBase( nSocketCode ).nEndurance;
	}

	return ( nMaxEndurance ) ? nMaxEndurance : GetItemBase().nEndurance;
}

int StructItem::GetLevelLimit() const
{
	return std::max( GameRule::GetItemLevelLimitByRank( GetItemRank() ), GetItemBase().nMinLevel );
}

int StructItem::GetRecommendLevel() const
{
	return GameRule::GetItemRecommendLevel( GetItemRank(), GetItemLevel(), GetItemBase().nMinLevel );
}

void StructItem::SetInstanceFlagOn( int idx )			
{
	if( m_Instance.Flag.IsOn( idx ) )
		return;
	TurnOnUpdateFlag();
	m_Instance.Flag.On( idx ); 
}

void StructItem::SetInstanceFlagOff( int idx )			
{ 
	if( !m_Instance.Flag.IsOn( idx ) )
		return;
	TurnOnUpdateFlag();
	m_Instance.Flag.Off( idx ); 
}

void StructItem::SetBindTarget( struct StructCreature *pTarget, const bool bSkipDBUpdate )
{
	m_Instance.Socket[0] = pTarget ? ( pTarget->IsPlayer() ? pTarget->GetSID() : 0 ) : 0;
	m_Instance.Socket[1] = pTarget ? ( pTarget->IsSummon() ? pTarget->GetSID() : 0 ) : 0;
	m_hBindedTarget = pTarget ? pTarget->GetHandle() : 0;

	if( !bSkipDBUpdate )
		TurnOnUpdateFlag();
}

// DB 에  아이템의 소유자를 변경해야 하는 쿼리가 미결되었는데 덜썩 지워버리면 낭패이므로
// 쿼리가 다 끝나기 전에는 삭제되지 않도록 한다.
bool StructItem::IsDeleteable()
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	if( !GameObject::IsDeleteable() ) return false;

	if( m_lQueryList.empty() ) return true;

	return false;
}

void StructItem::DBQuery( GameDBManager::DBProc *pWork )
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	if( m_lQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_lQueryList.push_back( pWork );
}

void StructItem::onEndQuery()
{
	THREAD_SYNCRONIZE( m_bQueryLock );

	m_lQueryList.pop_front();

	if( !m_lQueryList.empty() )
	{
		DB().Push( m_lQueryList.front() );
	}
}

void StructItem::CopyFrom( StructItem* pFrom )
{
	AR_HANDLE hOldOwnerHandle = m_Instance.OwnerHandle;
	int nOldOwner = m_Instance.UID;
	
	mv = pFrom->mv;
	layer = pFrom->layer;
	m_Instance = pFrom->m_Instance;

	m_Instance.UID = 0;

	m_Instance.nOwnerUID = nOldOwner;	
	m_Instance.OwnerHandle = hOldOwnerHandle;
}

const bool StructItem::IsReplaceable( const ItemBase::ItemCode code, const bool reset_remain_time )
{
	// 동종의 아이템은 무조건 실패(시간 제한 재설정은 SetRemainTime 함수를 이용해야 함)
	if( code == m_Instance.Code )
		return false;

	ItemBaseServer *pNewItemBase = &GetItemBase( code );

	// 기존에 시간 제한이 있던 아이템이었을 경우 영구템으로 변경하는데 남은 시간을 초기화하지 않아야하는 경우라면 불가능
	if( IsExpireItem() && !reset_remain_time && pNewItemBase->decrease_type == ItemBase::PERMANENT )
	{
		return false;
	}

	// 중첩 가능 아이템을 중첩 불가 아이템으로 바꾸는데 기존 수량이 1개가 아니면 불가능
	if( !pNewItemBase->Flag.IsOn( ItemBase::FLAG_JOIN ) && GetCount() != 1 )
		return false;

	// 아이템 생성 당시 기본 InstanceFlag 가 다른 아이템으로는 변경 불가능(아이템 생성 이후 어떻게 바뀌었는지가 날아감 -_ -;)
	if( pNewItemBase->nInstanceFlag != GetItemBase().nInstanceFlag )
		return false;

	return true;
}

bool StructItem::IsExpireItem() const
{
	if( GetItemBase().decrease_type == ItemBase::DECREASE_ON_GAME || GetItemBase().decrease_type == ItemBase::DECREASE_ALWAYS )
		return true;

	return false;
}

const unsigned short StructItem::SetElementalEffect( const unsigned char & cType, const time_t & tExpire )
{
	if( !IsWeapon() )
	{
		return RESULT_NOT_ACTABLE;
	}

	m_Instance.cElementalEffectType = cType;
	m_Instance.tElementalEffectExpire = tExpire;

	// 속성 이펙트 적용 시 기존에 있던 추가 성능은 제거됨
	m_Instance.nElementalEffectAttackPoint = 0;
	m_Instance.nElementalEffectMagicPoint = 0;

	return RESULT_SUCCESS;
}

const unsigned short StructItem::SetElementalEffectAttackPoint( const int nAttackPoint )
{
	if( !IsWeapon() || !GetElementalEffectType() )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 속성 이펙트 추가 공격력 설정
	m_Instance.nElementalEffectAttackPoint = nAttackPoint;

	return RESULT_SUCCESS;
}

const unsigned short StructItem::SetElementalEffectMagicPoint( const int nMagicPoint )
{
	if( !IsWeapon() || !GetElementalEffectType() )
	{
		return RESULT_NOT_ACTABLE;
	}

	// 속성 이펙트 추가 마력 설정
	m_Instance.nElementalEffectMagicPoint = nMagicPoint;

	return RESULT_SUCCESS;
}

void StructItem::ClearElementalEffect()
{
	// 속성 이펙트 초기화
	m_Instance.cElementalEffectType = 0;
	m_Instance.tElementalEffectExpire = 0;

	// 추가 공격력/마력 성능 초기화
	m_Instance.nElementalEffectAttackPoint = 0;
	m_Instance.nElementalEffectMagicPoint = 0;
}

bool StructItem::SetRandomOption( ItemInstance::RANDOM_TYPE eType, ItemInstance::RANDOM_OPTION & RandomOption )
{
	SetRandomSID( eType, RandomOption.nSID );

	m_Instance.RandomOption[ eType ].nRandomType = eType;

	for( int nCnt = 0 ; nCnt < ItemInstance::MAX_RANDOM_OPTION_NUMBER; ++nCnt )
	{
		m_Instance.RandomOption[ eType ].OptionInfo[ nCnt ].nType	= RandomOption.OptionInfo[ nCnt ].nType;
		m_Instance.RandomOption[ eType ].OptionInfo[ nCnt ].fValue1	= RandomOption.OptionInfo[ nCnt ].fValue1;
		m_Instance.RandomOption[ eType ].OptionInfo[ nCnt ].fValue2	= RandomOption.OptionInfo[ nCnt ].fValue2;
	}

	return true;
}

ItemInstance::RANDOM_OPTION* StructItem::GetRandomOption( ItemInstance::RANDOM_TYPE eType )
{
	if( eType < 0 || eType >= ItemInstance::MAX )
		return NULL;

	return &( m_Instance.RandomOption[ eType ] );
}
