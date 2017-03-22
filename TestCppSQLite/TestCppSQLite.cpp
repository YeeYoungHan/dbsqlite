#include "CppSQLite.h"

int main( int argc, char * argv[] )
{
	if( argc != 2 )
	{
		printf( "[Usage] %s {db filename}\n", argv[0] );
		return 0;
	}

	const char * pszDBFile = argv[1];
	CppSQLite clsDB;

	if( clsDB.Open( pszDBFile ) == false )
	{
		printf( "clsDB.Open(%s) error\n", pszDBFile );
		return 0;
	}

	if( clsDB.Execute( "CREATE TABLE Test( name TEXT PRIMARY KEY, value TEXT )" ) == false )
	{
		printf( "Create table error\n" );
	}

	clsDB.Close();

	return 0;
}
