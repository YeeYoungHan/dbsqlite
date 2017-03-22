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
 * @brief DB open �Ѵ�.
 * @param pszFileName DB ���� full path
 * @param bUseLock		DB ���� lock �� ����ϸ� true �� �Է��ϰ� �׷��� ������ false �� �Է��Ѵ�.
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief DB close �Ѵ�.
 * @returns true �� �����Ѵ�.
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
 * @brief DB �� open �Ǿ� �ִ��� �˻��Ѵ�.
 * @returns DB �� open �Ǿ� ������ true �� �����ϰ� �׷��� ������ false �� �����Ѵ�.
 */
bool CppSQLite::IsOpen( )
{
	if( m_psttDb ) return true;

	return false;
}

/**
 * @ingroup CppSQLite
 * @brief INSERT, UPDATE, DELETE SQL ���� �����Ѵ�.
 * @param pszSQL SQL ���ڿ�
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief 1���� ���� ���ڸ� ������ ���� SQL ���� �����Ѵ�.
 * @param pszSQL SQL ���ڿ�
 * @param pszArg ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief SQL ������ 1���� �÷��� �˻��Ѵ�.
 * @param pszSQL	SQL ���ڿ�
 * @param iData		[out] �˻��� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief SQL ������ 1���� �÷��� �˻��Ѵ�.
 * @param pszSQL	SQL ���ڿ�
 * @param iData		[out] �˻��� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief SQL ������ 1���� �÷��� �˻��Ѵ�.
 * @param pszSQL	SQL ���ڿ�
 * @param iData		[out] �˻��� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief SQL ������ 1���� �÷��� �˻��Ѵ�.
 * @param pszSQL	SQL ���ڿ�
 * @param strData [out] �˻��� ��� ���� ����
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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
 * @brief SELECT SQL ���� �����Ѵ�.
 * @param pszSQL		SQL ���ڿ�
 * @param pclsData	�˻� ��� ���� ����
 * @param FetchRow	�˻� ��� ���� �Լ�
 * @returns �����ϸ� true �� �����ϰ� �����ϸ� false �� �����Ѵ�.
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

	// 1���� Row �� ��� �������� ó���Ѵ�.

	sqlite3_finalize( psttStmt );

	return true;
}

