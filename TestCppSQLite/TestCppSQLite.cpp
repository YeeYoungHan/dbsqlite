#include "CppSQLite.h"
#include <list>

class CNameValue
{
public:
	std::string m_strName;
	std::string m_strValue;
};

typedef std::list< CNameValue > NAME_VALUE_LIST;

void DbSetData( sqlite3_stmt * psttStmt, int & iCol, std::string & strData )
{
	char * pszText = (char *)sqlite3_column_text( psttStmt, iCol );
	if( pszText )
	{
		strData = pszText;
	}

	++iCol;
}

/**
 * @ingroup TestCppSQLite
 * @brief N 개의 row 및 colmun 에 대한 쿼리 fetch 를 위한 함수
 * @param pclsData 응용에서 입력한 포인터
 * @param psttStmt sqlite3_stmt 포인터
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool FetchNameValue( void * pclsData, sqlite3_stmt * psttStmt )
{
	NAME_VALUE_LIST * pclsList = (NAME_VALUE_LIST *)pclsData;
	CNameValue clsRow;
	int iCol = 0;

	DbSetData( psttStmt, iCol, clsRow.m_strName );
	DbSetData( psttStmt, iCol, clsRow.m_strValue );

	pclsList->push_back( clsRow );

	return true;
}

int main( int argc, char * argv[] )
{
	if( argc != 2 )
	{
		printf( "[Usage] %s {db filename}\n", argv[0] );
		return 0;
	}

	const char * pszDBFile = argv[1];
	CppSQLite clsDB;

	// SQLite DB 를 open 한다.
	if( clsDB.Open( pszDBFile ) == false )
	{
		printf( "clsDB.Open(%s) error\n", pszDBFile );
		return 0;
	}

	clsDB.Execute( "DROP TABLE Test" );

	// 테이블을 생성한다.
	if( clsDB.Execute( "CREATE TABLE Test( name TEXT PRIMARY KEY, value TEXT )" ) == false )
	{
		printf( "Create table error\n" );
	}
	else
	{
		// 테이블에 INSERT 한다.
		clsDB.Execute( "INSERT INTO Test( name, value ) VALUES( 'n1', 'v1' )" );
		clsDB.Execute( "INSERT INTO Test( name, value ) VALUES( 'n2', 'v2' )" );

		// 한 개의 row 및 column 에 대한 쿼리를 실행한다.
		int iCount = 0;
		clsDB.QueryOne( "SELECT count(*) FROM Test", iCount );
		printf( "count = %d\n", iCount );

		// N 개의 row 및 colmun 에 대한 쿼리를 실행한다.
		NAME_VALUE_LIST clsList;
		NAME_VALUE_LIST::iterator itList;

		clsDB.Query( "SELECT name, value FROM Test", &clsList, FetchNameValue );
		for( itList = clsList.begin(); itList != clsList.end(); ++itList )
		{
			printf( "name(%s) value(%s)\n", itList->m_strName.c_str(), itList->m_strValue.c_str() );
		}
	}

	clsDB.Close();

	return 0;
}
