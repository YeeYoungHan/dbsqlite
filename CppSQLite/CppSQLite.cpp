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
#include "StringUtility.h"
#include "Log.h"
#include "MemoryDebug.h"

bool CppSQLite::m_bInit = false;
CSipMutex CppSQLite::m_clsMutex;

CppSQLite::CppSQLite() : m_psttDb(NULL), m_bUseLock(false)
{
	if( m_bInit == false )
	{
		sqlite3_initialize( );
		m_bInit = true;
	}
}

CppSQLite::~CppSQLite()
{
	Close();
}

/**
 * @ingroup CppSQLite
 * @brief DB open 한다.
 * @param pszFileName DB 파일 full path
 * @param bUseLock		DB 전역 lock 을 사용하면 true 를 입력하고 그렇지 않으면 false 를 입력한다.
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CppSQLite::Open( const char * pszFileName, bool bUseLock )
{
	if( m_psttDb ) return false;

	int n;

	m_bUseLock = bUseLock;

	if( m_bUseLock ) m_clsMutex.acquire();

	n = sqlite3_open( pszFileName, &m_psttDb );
	if( n != SQLITE_OK )
	{
		CLog::Print( LOG_ERROR, "DB open(%s) error(%d)", pszFileName, n );
		Close();
		return false;
	}

	return true;
}

/**
 * @ingroup CppSQLite
 * @brief DB close 한다.
 * @returns true 를 리턴한다.
 */
bool CppSQLite::Close( )
{
	if( m_psttDb )
	{
		sqlite3_close( m_psttDb );
		m_psttDb = NULL;

		if( m_bUseLock ) m_clsMutex.release();
	}

	return true;
}

/**
 * @ingroup CppSQLite
 * @brief DB 가 open 되어 있는지 검사한다.
 * @returns DB 가 open 되어 있으면 true 를 리턴하고 그렇지 않으면 false 를 리턴한다.
 */
bool CppSQLite::IsOpen( )
{
	if( m_psttDb ) return true;

	return false;
}

/**
 * @ingroup CppSQLite
 * @brief INSERT, UPDATE, DELETE SQL 문을 실행한다.
 * @param pszSQL SQL 문자열
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CppSQLite::Execute( const char * pszSQL )
{
	if( m_psttDb == NULL ) return false;

	sqlite3_stmt * psttStmt = NULL;
	int n;
	bool bRes = false;

	n = sqlite3_prepare( m_psttDb, pszSQL, -1, &psttStmt, NULL );
	if( n != SQLITE_OK )
	{
		CLog::Print( LOG_ERROR, "DB prepare(%s) error(%d) - %s", pszSQL, n, sqlite3_errmsg(m_psttDb) );
		return false;
	}

	n = sqlite3_step( psttStmt );
	if( n != SQLITE_DONE )
	{
		CLog::Print( LOG_ERROR, "DB step(%s) error(%d) - %s", pszSQL, n, sqlite3_errmsg(m_psttDb) );
	}
	else
	{
		bRes = true;
	}

	sqlite3_finalize( psttStmt );

	return bRes;
}

/**
 * @ingroup CppSQLite
 * @brief 1개의 동적 인자를 포함한 동적 SQL 문을 실행한다.
 * @param pszSQL SQL 문자열
 * @param pszArg 동적 인자
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CppSQLite::ExecuteBind( const char * pszSQL, const char * pszArg )
{
	if( m_psttDb == NULL ) return false;

	sqlite3_stmt * psttStmt = NULL;
	int n;
	bool bRes = false;

	n = sqlite3_prepare( m_psttDb, pszSQL, -1, &psttStmt, NULL );
	if( n != SQLITE_OK )
	{
		CLog::Print( LOG_ERROR, "DB prepare(%s) error(%d) - %s", pszSQL, n, sqlite3_errmsg(m_psttDb) );
		return false;
	}

	n = sqlite3_bind_text( psttStmt, 1, pszArg, -1, NULL );

	n = sqlite3_step( psttStmt );
	if( n != SQLITE_DONE )
	{
		CLog::Print( LOG_ERROR, "DB step(%s) error(%d) - %s", pszSQL, n, sqlite3_errmsg(m_psttDb) );
	}
	else
	{
		bRes = true;
	}

	sqlite3_finalize( psttStmt );

	return bRes;
}

/**
 * @ingroup CppSQLite
 * @brief SQL 문으로 1개의 컬럼을 검색한다.
 * @param pszSQL	SQL 문자열
 * @param iData		[out] 검색된 결과 저장 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CppSQLite::QueryOne( const char * pszSQL, int & iData )
{
	if( m_psttDb == NULL ) return false;

	sqlite3_stmt * psttStmt = NULL;
	int n;
	bool bRes = false;

	n = sqlite3_prepare( m_psttDb, pszSQL, -1, &psttStmt, NULL );
	if( n != SQLITE_OK )
	{
		CLog::Print( LOG_ERROR, "DB prepare(%s) error(%d)", pszSQL, n );
		return false;
	}

	if( sqlite3_step( psttStmt ) == SQLITE_ROW )
	{
		iData = sqlite3_column_int( psttStmt, 0 );
		bRes = true;
	}

	sqlite3_finalize( psttStmt );

	return bRes;
}

/**
 * @ingroup CppSQLite
 * @brief SQL 문으로 1개의 컬럼을 검색한다.
 * @param pszSQL	SQL 문자열
 * @param iData		[out] 검색된 결과 저장 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CppSQLite::QueryOne( const char * pszSQL, uint32_t & iData )
{
	std::string strData;

	if( QueryOne( pszSQL, strData ) == false ) return false;

	iData = GetUInt32( strData.c_str() );

	return true;
}

/**
 * @ingroup CppSQLite
 * @brief SQL 문으로 1개의 컬럼을 검색한다.
 * @param pszSQL	SQL 문자열
 * @param iData		[out] 검색된 결과 저장 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CppSQLite::QueryOne( const char * pszSQL, uint64_t & iData )
{
	std::string strData;

	if( QueryOne( pszSQL, strData ) == false ) return false;

	iData = GetUInt64( strData.c_str() );

	return true;
}

/**
 * @ingroup CppSQLite
 * @brief SQL 문으로 1개의 컬럼을 검색한다.
 * @param pszSQL	SQL 문자열
 * @param strData [out] 검색된 결과 저장 변수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CppSQLite::QueryOne( const char * pszSQL, std::string & strData )
{
	if( m_psttDb == NULL ) return false;

	sqlite3_stmt * psttStmt = NULL;
	int n;
	bool bRes = false;

	n = sqlite3_prepare( m_psttDb, pszSQL, -1, &psttStmt, NULL );
	if( n != SQLITE_OK )
	{
		CLog::Print( LOG_ERROR, "DB prepare(%s) error(%d)", pszSQL, n );
		return false;
	}

	if( sqlite3_step( psttStmt ) == SQLITE_ROW )
	{
		char * pszText = (char *)sqlite3_column_text( psttStmt, 0 );
		if( pszText )
		{
			strData = pszText;
			bRes = true;
		}
	}

	sqlite3_finalize( psttStmt );

	return bRes;
}

/**
 * @ingroup CppSQLite
 * @brief SELECT SQL 문을 실행한다.
 * @param pszSQL		SQL 문자열
 * @param pclsData	검색 결과 저장 변수
 * @param FetchRow	검색 결과 저장 함수
 * @returns 성공하면 true 를 리턴하고 실패하면 false 를 리턴한다.
 */
bool CppSQLite::Query( const char * pszSQL, void * pclsData, bool (*FetchRow)( void *, sqlite3_stmt * psttStmt ) )
{
	if( m_psttDb == NULL ) return false;

	sqlite3_stmt * psttStmt = NULL;
	int n;

	n = sqlite3_prepare( m_psttDb, pszSQL, -1, &psttStmt, NULL );
	if( n != SQLITE_OK )
	{
		CLog::Print( LOG_ERROR, "DB prepare(%s) error(%d)", pszSQL, n );
		return false;
	}

	while( sqlite3_step( psttStmt ) == SQLITE_ROW )
	{
		if( FetchRow( pclsData, psttStmt ) == false ) break;
	}

	// 1개의 Row 도 없어도 성공으로 처리한다.

	sqlite3_finalize( psttStmt );

	return true;
}

