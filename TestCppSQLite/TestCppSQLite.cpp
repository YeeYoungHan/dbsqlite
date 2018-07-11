/* 
 * Copyright (C) 2012 Yee Young Han <websearch@naver.com> (http://blog.naver.com/websearch)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

#include "CppSQLite.h"
#include <list>

// 테이블 row 1개를 저장할 자료구조
class CNameValue
{
public:
	std::string m_strName;
	std::string m_strValue;
};

// 테이블 row N개를 저장할 자료구조
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
	clsDB.Execute( "DROP TABLE TestKey" );

	// 테이블을 생성한다.
	if( clsDB.Execute( "CREATE TABLE Test( name TEXT PRIMARY KEY, value TEXT )" ) == false )
	{
		printf( "Create table error\n" );
	}
	else
	{
		// 테이블에 INSERT 한다.
		clsDB.Execute( "INSERT INTO Test( name, value ) VALUES( 'n1', 'v1' )" );

		// 동적 SQL 실행 예제
		clsDB.Execute( "INSERT INTO Test( name, value ) VALUES( ?, ? )", 2, "n2", "v2" );

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

	// AUTOINCREMENT KEY 를 포함한 테이블을 생성한다.
	if( clsDB.Execute( "CREATE TABLE TestKey( idk INTEGER PRIMARY KEY AUTOINCREMENT, value TEXT )" ) == false )
	{
		printf( "Create table error\n" );
	}
	else
	{
		int64_t iKey;

		// 테이블에 INSERT 한다.
		clsDB.Insert( "INSERT INTO TestKey( value ) VALUES( 'v1' )", &iKey );
		printf( "key(" LONG_LONG_FORMAT ")\n", iKey );
		clsDB.Insert( "INSERT INTO TestKey( value ) VALUES( 'v2' )", &iKey );
		printf( "key(" LONG_LONG_FORMAT ")\n", iKey );
	}

	clsDB.Close();

	return 0;
}
