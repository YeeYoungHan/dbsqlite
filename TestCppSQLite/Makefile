# Makefile for TestCppSQLite
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2017/03/22

include ../Makefile.mk

INC=-I../CppSQLite -I../SipPlatform -I../sqlite
LIB=-I../CppSQLite -lcppsqlite \
	-I../SipPlatform -lsipplatform \
	-I../sqlite -lsqlite \
	-lpthread

.SUFFIXES: .o .cpp
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< $(INC)

all: TestCppSQLite.exe

SRV_OBJ=TestCppSQLite.o

TestCppSQLite.exe: $(SRV_OBJ)
	$(CC) $(CFLAGS) -o $@ $(SRV_OBJ) $(INC) $(LIB)

clean:
	rm -f *.exe
	rm -f *.o
	rm -f core.*

