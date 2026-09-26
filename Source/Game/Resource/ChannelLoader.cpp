
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "ChannelManager.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbChannel : public CADORecordBinding
{

	int id;
	int left;
	int top;
	int right;
	int bottom;
	int channel_type;

BEGIN_ADO_BINDING(dbChannel)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, id, sizeof(id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, left, sizeof(left), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, top, sizeof(top), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(4, adInteger, right, sizeof(right), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(5, adInteger, bottom, sizeof(bottom), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(6, adInteger, channel_type, sizeof(channel_type), FALSE)
END_ADO_BINDING()
};

struct dbChannelUserLimit : public CADORecordBinding
{

	int id;
	int proper_user;
	int max_channel_num;

BEGIN_ADO_BINDING(dbChannelUserLimit)
	ADO_VARIABLE_LENGTH_ENTRY4(1, adInteger, id, sizeof(id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(2, adInteger, proper_user, sizeof(proper_user), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(3, adInteger, max_channel_num, sizeof(max_channel_num), FALSE)
END_ADO_BINDING()
};

void onChannelData( dbChannel * emprs )
{
	X2D::Box< AR_UNIT > box(
		static_cast< AR_UNIT >( emprs->left ),
		static_cast< AR_UNIT >( emprs->top ),
		static_cast< AR_UNIT >( emprs->right ),
		static_cast< AR_UNIT >( emprs->bottom ) );

	ChannelManager::RegisterChannelInfo( emprs->id, box, emprs->channel_type );
}

void onChannelUserData( dbChannelUserLimit * emprs )
{
	ChannelManager::RegisterUserLimitChannelInfo( emprs->id, emprs->proper_user, emprs->max_channel_num );
}

static bool LoadChannelResource()
{
	_ConnectionPtr		ConnPtr = NULL;

	InitContentDbConnection( ConnPtr );

	LoadDbResource< dbChannel >( "ChannelResource", ConnPtr, onChannelData );
	LoadDbResource< dbChannelUserLimit >( "ChannelUserLimitResource", ConnPtr, onChannelUserData );

	_cprint( "Channel info loading complete...\n" );
	FILELOG( "Channel info loading complete..." );

	return true;
}

bool ChannelLoader::onProcess( int nThreadNum )
{
	DBPerformanceTrackHelper helper;
	try
	{
		helper.start();
		LoadChannelResource();
		helper.end( "ChannelLoader" );
	}
	catch( _com_error &e )
	{
		helper.end( e.Error(), "ChannelLoader" );
		LogDBError( e, "ChannelLoader", "LoadDbResource()" );

		std::string strError = "CHANNEL RESOUCE DB ERROR : ";
		strError += e.Description();
		throw XException( strError );
	}

	return true;
}