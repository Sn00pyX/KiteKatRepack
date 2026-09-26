
#include <vector>

#include <network/XIOCP.h>
#include <toolkit/myDC.h>
#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>
#include <framework/ArcadiaFramework.h>
#include <dump/XException.h>
#include <toolkit/XEnv.h>

#include "StructPlayer.h"
#include "StructNPC.h"
#include "StructSummon.h"
#include "StructItem.h"
#include "Constant.h"

myDC				g_DC;

extern XCriticalSection				g_Lock;
extern std::vector< StructPlayer* >	g_vPlayer;
extern std::vector< StructItem* >		g_vItem;
//extern std::vector< StructMonster* >	g_vNPC;
extern std::vector< StructSummon* >		g_vSummon;

extern XIOCP			*				g_pIOCP;

extern ArcadiaServer *			g_pArcadia;

extern int								g_nDrawMode;

int										g_nDrawScale = 1;

inline int								_S( int x ) { return x / g_nDrawScale; }

void DrawConsole( XConsole & console, int nStartPos = 0 )
{	
	for( int y = 0; y < console.GetHeight(); y++ )
	{
		g_DC.Printf( 5, (nStartPos + y )*14+5, console.GetLine( y ) );
	}
}

void DrawRegion()
{
	if( ENV().IsExist( "no_region" ) ) return;

	g_DC.SetPenColor( RGB( 230, 230, 230 ) );
	for( int x = 0; x < 1280*g_nDrawScale; x += g_nRegionSize )
	{
		g_DC.Line( 0, _S(x), 1280, _S(x ));
		g_DC.Line( _S(x), 0, _S(x), 1024 );
	}
	g_DC.SetPenColor( 0 );
}

void DrawPlayer()
{
	if( ENV().IsExist( "no_player" ) ) return;

	for( unsigned i = 0; i < g_vPlayer.size(); i++ )
	{	
		g_DC.SetPenColor( RGB( 100,200,100 ) );
		AR_UNIT fx = g_vPlayer[i]->GetX();
		AR_UNIT fy = g_vPlayer[i]->GetY();

		if( g_vPlayer[i]->IsMoving() ) g_DC.DrawArrow( _S(fx), _S(fy), _S(g_vPlayer[i]->GetTX()), _S(g_vPlayer[i]->GetTY()) );

		g_DC.SetPenColor( 0 );

		COLORREF c = RGB( 200, 0, 0 );
		if( g_vPlayer[i]->IsDead() ) c = RGB( 100, 100, 200 );
		g_DC.Box( _S( g_vPlayer[i]->GetX() ) - 1, _S( g_vPlayer[i]->GetY() ) - 1, _S( g_vPlayer[i]->GetX() ) + 1, _S( g_vPlayer[i]->GetY() ) + 1, c, FILL_SOLID );
	}
}



void DrawSummon()
{
	if( ENV().IsExist( "no_summon" ) ) return;

	for( unsigned i = 0; i < g_vSummon.size(); i++ )
	{	
		g_DC.SetPenColor( RGB( 100,200,100 ) );
		AR_UNIT fx = g_vSummon[i]->GetX();
		AR_UNIT fy = g_vSummon[i]->GetY();

		if( g_vSummon[i]->IsMoving() ) g_DC.DrawArrow( _S( fx ), _S( fy ), _S( g_vSummon[i]->GetTX() ), _S( g_vSummon[i]->GetTY() ) );

		g_DC.SetPenColor( 0 );

		COLORREF c = RGB( 200, 0, 200 );
		if( g_vSummon[i]->IsDead() ) c = RGB( 100, 100, 200 );
		g_DC.Box( _S( g_vSummon[i]->GetX() ) - 1, _S( g_vSummon[i]->GetY() ) - 1, _S( g_vSummon[i]->GetX() ) + 1, _S( g_vSummon[i]->GetY() ) + 1, c, FILL_SOLID );
	}
}

void DrawItem()
{
	if( ENV().IsExist( "no_item" ) ) return;

	for( unsigned i = 0; i < g_vItem.size(); i++ )
	{	
		//g_DC.SetPenColor( RGB( 200,200,0 ) );
		AR_UNIT fx = g_vItem[i]->GetX();
		AR_UNIT fy = g_vItem[i]->GetY();

		g_DC.Box( _S( g_vItem[i]->GetX() ) - 1, _S( g_vItem[i]->GetY() ) - 1, _S( g_vItem[i]->GetX() ) + 1, _S( g_vItem[i]->GetY() ) + 1, RGB(200,200,0), FILL_SOLID );
	}
}

void DrawNPC()
{
	if( ENV().IsExist( "no_npc" ) ) return;
/*
	for( unsigned i = 0; i < g_vNPC.size(); i++ )
	{	
		g_DC.SetPenColor( RGB( 200,200,200 ) );
		AR_UNIT fx = g_vNPC[i]->GetX();
		AR_UNIT fy = g_vNPC[i]->GetY();

		if( _S( fx ) > 1600 ) continue;
		if( _S( fy ) > 1600 ) continue;

		if( g_vNPC[i]->IsMoving() ) g_DC.DrawArrow( _S( fx ), _S( fy ), _S( g_vNPC[i]->GetTX() ), _S( g_vNPC[i]->GetTY() ) );

		g_DC.SetPenColor( 0 );

		COLORREF c = RGB( 200, 0, 0 );
		if( !g_vNPC[i]->bIsNearClient ) c = RGB( 240, 200, 200 );
		if( g_vNPC[i]->IsDead() ) c = RGB( 240, 240, 0 );
		g_DC.Box( _S( g_vNPC[i]->GetX() ) - 1, _S( g_vNPC[i]->GetY() ) - 1, _S( g_vNPC[i]->GetX() ) + 1, _S( g_vNPC[i]->GetY() ) + 1, c, FILL_SOLID );

		if( g_vNPC[i]->GetPriority() != ArObject::UPDATE_PRIORITY_IDLE )
		{
			g_DC.Box( _S( g_vNPC[i]->GetX() ) - 3, _S( g_vNPC[i]->GetY() ) - 3, _S( g_vNPC[i]->GetX() ) + 3, _S( g_vNPC[i]->GetY() ) + 3, RGB(240, 200, 200) );
		}
	}
*/
}

void DrawStat()
{
	g_DC.Clear();
	g_DC.Begin();

	static XConsole statConsole(120, 10 );
	static DWORD dwPrevTime = timeGetTime();
	static long nIPS;
	static int nActiveCnt, nCnt;
	static float fUsage;

	if( g_pIOCP )
	{
		nActiveCnt += g_pIOCP->GetActiveThreadCount();
		nCnt++;
	}

	if( dwPrevTime + 1000 < timeGetTime() )
	{
		dwPrevTime = timeGetTime();
		if( g_pIOCP )
		{
			static long nPrevIPS = 0;
			nIPS = g_pIOCP->GetInstructionCount() - nPrevIPS;
			nPrevIPS += nIPS;
		}

		fUsage = (float)nActiveCnt*100/(nCnt*g_pIOCP->GetTotalThreadCount()) ;
		nCnt = 0;
		nActiveCnt = 0;
	}

	statConsole.Clear();

	statConsole.Printf( "Overview         : ArTime(%8d)        CPU Usage(%4.1f%%%%)\n", GetArTime(), fUsage );
	statConsole.Printf( "Object Count     : Client(%4d)  NPC(%3d)  Item(%3d)\n", g_vPlayer.size(), NPC_COUNT, g_vItem.size() );
	if( g_pIOCP && g_pArcadia )
	{
		statConsole.Printf( "Thread Condition : Active/Total(%2d/%2d)     Work/Wait(%d/%2d)\n", g_pIOCP->GetActiveThreadCount(), g_pIOCP->GetTotalThreadCount(), g_pArcadia->GetLockedThreadCount(), g_pArcadia->GetWaitingThreadCount() );
		statConsole.Printf( "Instruction Proc : Total(%9d)        IPS(%-5d)\n", g_pIOCP->GetInstructionCount(), nIPS );
	}

	statConsole.Printf( "\n\n\nF1 : display server stat   F2 : display world objects\n" );

	DrawConsole( statConsole );

	HDC dc = GetDC( ArcadiaFrameworkIntf::GetViewWindow() );
	BitBlt( dc, 0, 0, ArcadiaFrameworkIntf::GetViewRect().right, ArcadiaFrameworkIntf::GetViewRect().bottom, g_DC.GetDC(), 0, 0, SRCCOPY );
	ReleaseDC( ArcadiaFrameworkIntf::GetViewWindow(), dc );
}

void DrawWorld()
{
	THREAD_SYNCHRONIZE( g_Lock );

	g_DC.Clear();
	g_DC.Begin();

	DrawRegion();

	DrawPlayer();

	DrawSummon();

	DrawItem();

	DrawNPC();
	
	HDC dc = GetDC( ArcadiaFrameworkIntf::GetViewWindow() );
	BitBlt( dc, 0, 0, ArcadiaFrameworkIntf::GetViewRect().right, ArcadiaFrameworkIntf::GetViewRect().bottom, g_DC.GetDC(), 0, 0, SRCCOPY );
	ReleaseDC( ArcadiaFrameworkIntf::GetViewWindow(), dc );
}

void Draw()
{
	/*
	if( g_nDrawMode == MODE_WORLD && GetKeyState( VK_SCROLL ) )
	{
		return;
	}

	if( g_nDrawMode == MODE_STAT )
	{
		DrawStat();
		return;
	}*/

	DrawWorld();
}
