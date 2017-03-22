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

#ifndef _CPP_SQLITE_H_
#define _CPP_SQLITE_H_

#include "sqlite3.h"
#include <string>
#include "SipMutex.h"

/**
 * @ingroup CppSQLite
 * @brief C++ 기반 sqlite 클래스
 */
class CppSQLite
{
public:
	CppSQLite();
	~CppSQLite();

	bool Open( const char * pszFileName, bool bUseLock = false );
	bool Close( );
	bool IsOpen( );

	bool Execute( const char * pszSQL );
	bool ExecuteBind( const char * pszSQL, const char * pszArg );
	bool QueryOne( const char * pszSQL, int & iData );
	bool QueryOne( const char * pszSQL, uint32_t & iData );
	bool QueryOne( const char * pszSQL, uint64_t & iData );
	bool QueryOne( const char * pszSQL, std::string & strData );
	bool Query( const char * pszSQL, void * pclsData, bool (*FetchRow)( void *, sqlite3_stmt * psttStmt ) );

private:
	bool CreateTable();

	sqlite3	* m_psttDb;
	bool			m_bUseLock;

	static bool m_bInit;
	static CSipMutex m_clsMutex;
};

#endif
