// 생성자에서 GameContent를 사용해야 해서 파일로 뺌 -_ -;;

#include "GameContent.h"
#include "MonsterBase.h"


MonsterBase::MonsterBase()
{
	memset( this, 0, sizeof( *this ) );

	trigger_list = GameContent::GetMonsterTriggerInfo( 0 );
	item_drop_list = GameContent::GetMonsterDropTable( 0 );
	skill_info_list = GameContent::GetMonsterSkillInfo( 0 );
}
