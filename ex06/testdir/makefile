.cc.o:

	g++ -c $(CFLAGS) $<

VPATH = ${SOURCEDIR}:${TESTDIR}
CC = g++
YACC = bison

YFLAGS = -d -v -y
DFLAG = 
CFLAGS = -I. -I${INCDIR} -I${TESTDIR} ${EXINCFLAG}
INCDIR = ${SOURCEDIR}
PSOURCEDIR = ..
SOURCEDIR = ..
TESTDIR = .
YFILES = ${SOURCEDIR}/strlist.y
OFILES = ${TESTDIR}/error.o
I86OBJFILES = ${TESTDIR}/scanner.o

MACHINE = $(shell uname -i)


all:	strlist

strlist : y.tab.o
ifneq (,$(findstring i86,${MACHINE}))
	${CC} ${CFLAGS} -o $@ ${I86OBJFILES} ${PSOURCEDIR}/error.cc $^
endif

y.tab.c y.tab.h: ${YFILES}
	${YACC} ${YFLAGS} ${DFLAG} ${YFILES}

clean:
	rm -f strlist y.tab.* y.output ${OFILES} \
              test[0-9].in.result test[0-9][0-9].in.result

