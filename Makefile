# Makefile for all of C++ sqlite
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2017/03/22

all:
	cd SipPlatform && make
	cd sqlite && make
	cd CppSQLite && make
	cd TestCppSQLite && make

clean:
	cd SipPlatform && make clean
	cd sqlite && make clean
	cd CppSQLite && make clean
	cd TestCppSQLite && make clean

install:

