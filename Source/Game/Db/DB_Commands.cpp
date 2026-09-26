
#include "DB_Commands.h"


void	DateToString( DATE date, char* buffer, size_t size )
{
	COleDateTime dtDate( date );
	s_sprintf( buffer, size, "%04d-%02d-%02d %02d:%02d:%02d",
		dtDate.GetYear(), dtDate.GetMonth(), dtDate.GetDay(),
		dtDate.GetHour(), dtDate.GetMinute(), dtDate.GetSecond() );
}
