# Makefile for sqlite
#
# programmer : yee young han ( websearch@naver.com )
#            : http://blog.naver.com/websearch
# start date : 2017/03/22

include ../Makefile.mk

INC=

.SUFFIXES: .o .c
%.o : %.c
	gcc $(CFLAGS) -c $< $(INC)

all: libsqlite.a

LIB_OBJ=sqlite3.o

libsqlite.a: $(LIB_OBJ)
	ar crv $@ $(LIB_OBJ)

clean:
	rm -f *.a
	rm -f *.o
	rm -f core.*

