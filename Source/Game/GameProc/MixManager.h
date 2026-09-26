#include "MixBase.h"
#include "ContentStruct.h"

struct MixManager
{
	static bool FindAndDecompose( struct StructPlayer* pPlayer, std::vector< std::pair< struct StructItem*, __int64 > >* vItemListToCreate, AR_HANDLE hItem, int nCount );
	static void RegisterEnhanceInfo( EnhanceInfo & info );
	static bool EnhanceItem( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool EnhanceSkillCard( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool EnhanceCreatureCard( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool UltimateEnhance( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool MixItemLevel( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool RecycleItem( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool RestoreEnhance( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool CreateItem( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool ChangeItemCode( const MixBase* pMixInfo, struct StructPlayer* pPlayer, struct StructItem* pMainMaterial, int nSubMaterialCount, struct StructItem** pSubMaterial, unsigned short* pCountList, std::vector< std::pair< StructItem*, __int64 > >* vCreatedItem = NULL);
	static bool ChangeAppearanceCode( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool ChangeItemUsablePeriod( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool SetElementalEffect( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool SetElementalEffectParameter( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool SetSocket( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool ReplaceSocketWith( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool SacrificeItemForEtherealDurability( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool SacrificeItemForEtherealStoneDurability( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool TransmitEtherealDurability( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool TransmitEtherealDurabilityFromEtherealStone( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool RecoverExhaustedEtherealDurability( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool InsertAwakenItem( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool DeleteAwakenOption( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL );
	static bool DeleteRandomOption(const MixBase* pMixInfo, struct StructPlayer* pPlayer, struct StructItem* pMainMaterial, int nSubMaterialCount, struct StructItem** pSubMaterial, unsigned short* pCountList, std::vector< std::pair< StructItem*, __int64 > >* vCreatedItem = NULL);
	static bool IdentifyItemForRandomOption(  const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem = NULL);

	static const MixBase * GetProperMixInfoAndArrangeSubMaterials( struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList );
	static void RegisterMixInfo( const MixBase & info );
	static void RegisterDecomposeInfo( const DecomposeBase & info );
	static bool CreateItemByDecompose( struct StructPlayer * pPlayer, DecomposeBase & decomposeInfo, std::vector< std::pair< struct StructItem*, __int64 > > *vCreatedItem, int nCount );

	static void procEnhanceFail( StructPlayer *pPlayer, StructItem *pItem, int nFailResult );
};
