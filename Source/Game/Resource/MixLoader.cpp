
#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>

#include "ContentLoader.h"
#include "MixBase.h"
#include "MixManager.h"
#include "GameDBUtil.h"
#include "DBPerformanceTracker.h"
#include "ADOConnection.h"

struct dbMix : public MixBase, public CADORecordBinding
{

BEGIN_ADO_BINDING(dbMix)

	ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, id, sizeof(id), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, type, sizeof(type), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, value[0], sizeof(value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, value[1], sizeof(value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, value[2], sizeof(value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, value[3], sizeof(value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, value[4], sizeof(value[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, value[5], sizeof(value[5]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4( 9, adInteger, sub_material_cnt, sizeof(sub_material_cnt), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(10, adInteger, main_material.type[0], sizeof(main_material.type[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(11, adInteger, main_material.value[0], sizeof(main_material.value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(12, adInteger, main_material.type[1], sizeof(main_material.type[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(13, adInteger, main_material.value[1], sizeof(main_material.value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(14, adInteger, main_material.type[2], sizeof(main_material.type[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(15, adInteger, main_material.value[2], sizeof(main_material.value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(16, adInteger, main_material.type[3], sizeof(main_material.type[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(17, adInteger, main_material.value[3], sizeof(main_material.value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(18, adInteger, main_material.type[4], sizeof(main_material.type[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(19, adInteger, main_material.value[4], sizeof(main_material.value[4]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(20, adInteger, sub_material[0].type[0], sizeof(sub_material[0].type[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(21, adInteger, sub_material[0].value[0], sizeof(sub_material[0].value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(22, adInteger, sub_material[0].type[1], sizeof(sub_material[0].type[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(23, adInteger, sub_material[0].value[1], sizeof(sub_material[0].value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(24, adInteger, sub_material[0].type[2], sizeof(sub_material[0].type[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(25, adInteger, sub_material[0].value[2], sizeof(sub_material[0].value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(26, adInteger, sub_material[0].type[3], sizeof(sub_material[0].type[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(27, adInteger, sub_material[0].value[3], sizeof(sub_material[0].value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(28, adInteger, sub_material[0].type[4], sizeof(sub_material[0].type[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(29, adInteger, sub_material[0].value[4], sizeof(sub_material[0].value[4]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(30, adInteger, sub_material[1].type[0], sizeof(sub_material[1].type[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(31, adInteger, sub_material[1].value[0], sizeof(sub_material[1].value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(32, adInteger, sub_material[1].type[1], sizeof(sub_material[1].type[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(33, adInteger, sub_material[1].value[1], sizeof(sub_material[1].value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(34, adInteger, sub_material[1].type[2], sizeof(sub_material[1].type[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(35, adInteger, sub_material[1].value[2], sizeof(sub_material[1].value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(36, adInteger, sub_material[1].type[3], sizeof(sub_material[1].type[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(37, adInteger, sub_material[1].value[3], sizeof(sub_material[1].value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(38, adInteger, sub_material[1].type[4], sizeof(sub_material[1].type[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(39, adInteger, sub_material[1].value[4], sizeof(sub_material[1].value[4]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(40, adInteger, sub_material[2].type[0], sizeof(sub_material[2].type[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(41, adInteger, sub_material[2].value[0], sizeof(sub_material[2].value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(42, adInteger, sub_material[2].type[1], sizeof(sub_material[2].type[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(43, adInteger, sub_material[2].value[1], sizeof(sub_material[2].value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(44, adInteger, sub_material[2].type[2], sizeof(sub_material[2].type[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(45, adInteger, sub_material[2].value[2], sizeof(sub_material[2].value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(46, adInteger, sub_material[2].type[3], sizeof(sub_material[2].type[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(47, adInteger, sub_material[2].value[3], sizeof(sub_material[2].value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(48, adInteger, sub_material[2].type[4], sizeof(sub_material[2].type[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(49, adInteger, sub_material[2].value[4], sizeof(sub_material[2].value[4]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(50, adInteger, sub_material[3].type[0], sizeof(sub_material[3].type[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(51, adInteger, sub_material[3].value[0], sizeof(sub_material[3].value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(52, adInteger, sub_material[3].type[1], sizeof(sub_material[3].type[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(53, adInteger, sub_material[3].value[1], sizeof(sub_material[3].value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(54, adInteger, sub_material[3].type[2], sizeof(sub_material[3].type[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(55, adInteger, sub_material[3].value[2], sizeof(sub_material[3].value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(56, adInteger, sub_material[3].type[3], sizeof(sub_material[3].type[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(57, adInteger, sub_material[3].value[3], sizeof(sub_material[3].value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(58, adInteger, sub_material[3].type[4], sizeof(sub_material[3].type[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(59, adInteger, sub_material[3].value[4], sizeof(sub_material[3].value[4]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(60, adInteger, sub_material[4].type[0], sizeof(sub_material[4].type[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(61, adInteger, sub_material[4].value[0], sizeof(sub_material[4].value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(62, adInteger, sub_material[4].type[1], sizeof(sub_material[4].type[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(63, adInteger, sub_material[4].value[1], sizeof(sub_material[4].value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(64, adInteger, sub_material[4].type[2], sizeof(sub_material[4].type[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(65, adInteger, sub_material[4].value[2], sizeof(sub_material[4].value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(66, adInteger, sub_material[4].type[3], sizeof(sub_material[4].type[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(67, adInteger, sub_material[4].value[3], sizeof(sub_material[4].value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(68, adInteger, sub_material[4].type[4], sizeof(sub_material[4].type[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(69, adInteger, sub_material[4].value[4], sizeof(sub_material[4].value[4]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(70, adInteger, sub_material[5].type[0], sizeof(sub_material[5].type[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(71, adInteger, sub_material[5].value[0], sizeof(sub_material[5].value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(72, adInteger, sub_material[5].type[1], sizeof(sub_material[5].type[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(73, adInteger, sub_material[5].value[1], sizeof(sub_material[5].value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(74, adInteger, sub_material[5].type[2], sizeof(sub_material[5].type[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(75, adInteger, sub_material[5].value[2], sizeof(sub_material[5].value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(76, adInteger, sub_material[5].type[3], sizeof(sub_material[5].type[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(77, adInteger, sub_material[5].value[3], sizeof(sub_material[5].value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(78, adInteger, sub_material[5].type[4], sizeof(sub_material[5].type[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(79, adInteger, sub_material[5].value[4], sizeof(sub_material[5].value[4]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(80, adInteger, sub_material[6].type[0], sizeof(sub_material[6].type[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(81, adInteger, sub_material[6].value[0], sizeof(sub_material[6].value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(82, adInteger, sub_material[6].type[1], sizeof(sub_material[6].type[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(83, adInteger, sub_material[6].value[1], sizeof(sub_material[6].value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(84, adInteger, sub_material[6].type[2], sizeof(sub_material[6].type[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(85, adInteger, sub_material[6].value[2], sizeof(sub_material[6].value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(86, adInteger, sub_material[6].type[3], sizeof(sub_material[6].type[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(87, adInteger, sub_material[6].value[3], sizeof(sub_material[6].value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(88, adInteger, sub_material[6].type[4], sizeof(sub_material[6].type[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(89, adInteger, sub_material[6].value[4], sizeof(sub_material[6].value[4]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(90, adInteger, sub_material[7].type[0], sizeof(sub_material[7].type[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(91, adInteger, sub_material[7].value[0], sizeof(sub_material[7].value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(92, adInteger, sub_material[7].type[1], sizeof(sub_material[7].type[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(93, adInteger, sub_material[7].value[1], sizeof(sub_material[7].value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(94, adInteger, sub_material[7].type[2], sizeof(sub_material[7].type[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(95, adInteger, sub_material[7].value[2], sizeof(sub_material[7].value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(96, adInteger, sub_material[7].type[3], sizeof(sub_material[7].type[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(97, adInteger, sub_material[7].value[3], sizeof(sub_material[7].value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(98, adInteger, sub_material[7].type[4], sizeof(sub_material[7].type[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(99, adInteger, sub_material[7].value[4], sizeof(sub_material[7].value[4]), FALSE)

	ADO_VARIABLE_LENGTH_ENTRY4(100, adInteger, sub_material[8].type[0], sizeof(sub_material[8].type[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(101, adInteger, sub_material[8].value[0], sizeof(sub_material[8].value[0]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(102, adInteger, sub_material[8].type[1], sizeof(sub_material[8].type[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(103, adInteger, sub_material[8].value[1], sizeof(sub_material[8].value[1]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(104, adInteger, sub_material[8].type[2], sizeof(sub_material[8].type[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(105, adInteger, sub_material[8].value[2], sizeof(sub_material[8].value[2]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(106, adInteger, sub_material[8].type[3], sizeof(sub_material[8].type[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(107, adInteger, sub_material[8].value[3], sizeof(sub_material[8].value[3]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(108, adInteger, sub_material[8].type[4], sizeof(sub_material[8].type[4]), FALSE)
	ADO_VARIABLE_LENGTH_ENTRY4(109, adInteger, sub_material[8].value[4], sizeof(sub_material[8].value[4]), FALSE)

END_ADO_BINDING()

};

struct dbDecompose : public DecomposeBase, public CADORecordBinding
{

	BEGIN_ADO_BINDING(dbDecompose)

		ADO_VARIABLE_LENGTH_ENTRY4( 1, adInteger, id, sizeof(id), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 2, adInteger, type, sizeof(type), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 3, adInteger, neededPrice, sizeof(neededPrice), FALSE)

		ADO_VARIABLE_LENGTH_ENTRY4( 4, adInteger, value[0], sizeof(value[0]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 5, adInteger, value[1], sizeof(value[1]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 6, adInteger, value[2], sizeof(value[2]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 7, adInteger, value[3], sizeof(value[3]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 8, adInteger, value[4], sizeof(value[4]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 9, adInteger, value[5], sizeof(value[5]), FALSE)

		ADO_VARIABLE_LENGTH_ENTRY4( 10, adInteger, material.type[0], sizeof(material.type[0]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 11, adInteger, material.value[0], sizeof(material.value[0]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 12, adInteger, material.type[1], sizeof(material.type[1]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 13, adInteger, material.value[1], sizeof(material.value[1]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 14, adInteger, material.type[2], sizeof(material.type[2]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 15, adInteger, material.value[2], sizeof(material.value[2]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 16, adInteger, material.type[3], sizeof(material.type[3]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 17, adInteger, material.value[3], sizeof(material.value[3]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 18, adInteger, material.type[4], sizeof(material.type[4]), FALSE)
		ADO_VARIABLE_LENGTH_ENTRY4( 19, adInteger, material.value[4], sizeof(material.value[4]), FALSE)

	END_ADO_BINDING()
};


void onMixInfo( dbMix * emprs )
{
	MixManager::RegisterMixInfo( *emprs );
}

void onDecomposeInfo( dbDecompose * emprs )
{
	MixManager::RegisterDecomposeInfo( *emprs );
}

bool MixLoader::onProcess( int nThreadNum )
{
	{
		// MixLoader에서 MixResource의 정보와 DecomposeResource의 정보 두가지를 로드한다.
		DBPerformanceTrackHelper helper;
		try
		{
			_ConnectionPtr		ConnPtr = NULL;

			InitContentDbConnection( ConnPtr );

			helper.start();
			size_t cnt = LoadDbResource< dbMix >( "MixResource", ConnPtr, onMixInfo );
			helper.end( "MixResource" );

			_cprint( "Total %d Mix info loaded...\n", cnt );
			FILELOG( "Total %d Mix info loaded...", cnt );
		}
		catch( _com_error &e )
		{
			helper.end( e.Error(), "MixResource" );
			LogDBError( e, "MixLoader", "MixResource" );

			std::string strError = "MIX RESOURCE DB ERROR : ";
			strError += e.Description();
			throw XException( strError );
		}
	}

	{
		DBPerformanceTrackHelper helper;
		try
		{		
			_ConnectionPtr		ConnPtr = NULL;

			InitContentDbConnection( ConnPtr );

			helper.start();
			size_t cnt = LoadDbResource< dbDecompose >( "DecomposeResource", ConnPtr, onDecomposeInfo );
			helper.end( "DecomposeResource" );

			_cprint( "Total %d Decompose info loaded...\n", cnt );
			FILELOG( "Total %d Decompose info loaded...", cnt );	
		}
		catch( _com_error &e )
		{
			helper.end( e.Error(), "DecomposeResource" );
			LogDBError( e, "MixLoader", "DecomposeResource" );

			std::string strError = "DECOMPOSE RESOURCE DB ERROR : ";
			strError += e.Description();
			throw XException( strError );
		}
	}

	return true;
}
