
#include <toolkit/XEnv.h>
#include <network/IConnection.h>

#include "AgeLimitRule.h"
#include "StructPlayer.h"

struct EnvIniter
{

	EnvIniter()
		:perod_begin((time_t)-1),perod_end((time_t)-1)
	{

	}
	void Init()
	{
		age = ENV().GetInt("game.age_limit",0);

		if( ENV().IsExist("game.age_limit_period") )
		{
			char ct;
			int st_hour,st_min,ed_hour,ed_min;
			sscanf_s( ENV().GetString("game.age_limit_period").c_str(), "%d:%d%c%d:%d", &st_hour, &st_min, &ct, sizeof( ct ), &ed_hour, &ed_min );
			perod_begin = st_hour*60*60+st_min*60;
			perod_end = ed_hour*60*60+ed_min*60;
		}
	}

	bool IsValid()
	{
		return 0 != age && ((time_t)-1) != perod_begin && ((time_t)-1) != perod_end;
	}
	bool Check(time_t cur)
	{
		cur %= (24*60*60);
		return ( perod_begin > perod_end && (perod_begin <= cur || perod_end > cur) )
			|| ( perod_begin <= cur && perod_end > cur ) ;
	}

	time_t perod_begin;
	time_t perod_end;
	time_t age;
};


#define LIMIT_AGE_PERIOD_FLAG_1 1
#define LIMIT_AGE_PERIOD_FLAG_2 2


static struct EnvIniter age_limit;

void  InitAgeLimitPerod( )
{
	age_limit.Init();
}

bool CheckAgeLimitPerod( struct IStreamSocketConnection * pConnection )
{
	if( !age_limit.IsValid() )
	{
		return false;
	}

	_CONNECTION_TAG * pTag = static_cast< _CONNECTION_TAG * >( pConnection->GetTag() );

	// 로그인 이전의 세션
	if( pTag->szAccountName[ 0 ] == '\0' ) return false;
	if( !pTag->nAge ) return true;

	if( age_limit.age > pTag->nAge )
	{
		time_t cur = time(NULL);
		struct tm tt;
		_localtime64_s( &tt, &cur );

		cur = tt.tm_hour*60*60+tt.tm_min*60+tt.tm_sec;

		if( age_limit.Check(cur) )
		{
			pConnection->Close();
			return true;	
		}

		if( !pTag->pPlayer ) return false;

		if( LIMIT_AGE_PERIOD_FLAG_2 > pTag->nAgeLimitFlags && age_limit.Check(cur+30*60) )
		{
			pTag->nAgeLimitFlags = LIMIT_AGE_PERIOD_FLAG_2;			
			SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pTag->pPlayer, "@2933" ); // 만 16세 미만의 유저는 정부에서 시행하는 셧다운 제도에 따라 30분 후에 게임이 자동으로 종료되오니 안전한 장소에서 로그 아웃을 해 주시기 바랍니다.
			return false;
		}
		if( !pTag->nAgeLimitFlags && age_limit.Check(cur+60*60) )
		{
			pTag->nAgeLimitFlags = LIMIT_AGE_PERIOD_FLAG_1;
			SendChatMessage( false, CHAT_NOTICE, "@NOTICE", pTag->pPlayer, "@2934" ); // 만 16세 미만의 유저는 정부에서 시행하는 셧다운 제도에 따라 1시간 후에 게임이 자동으로 종료되오니 안전한 장소에서 로그 아웃을 해 주시기 바랍니다.
			return false;
		}			
	}

	return false;
}