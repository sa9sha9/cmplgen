.cc.o:

	g++ -c $(CFLAGS) $<

CC = g++
YACC = bison

YFLAGS = -d -v -y
DFLAG = 


MACHINE = $(shell uname -i)


loop1 : YFILE=loop1.y
loop1 : yclean y.tab.c y.tab.h ${OFILES}
ifneq (,$(findstring i86,${MACHINE}))
	${CC} ${CFLAGS} -o $@ y.tab.c loop-scanner.o
endif

loop2 : YFILE=loop2.y
loop2 : yclean y.tab.c y.tab.h ${OFILES}
ifneq (,$(findstring i86,${MACHINE}))
	${CC} ${CFLAGS} -o $@ y.tab.c loop-scanner.o
endif

loop3 : YFILE=loop3.y
loop3 : yclean y.tab.c y.tab.h ${OFILES}
ifneq (,$(findstring i86,${MACHINE}))
	${CC} ${CFLAGS} -o $@ y.tab.c loop-scanner.o
endif

loop4 : YFILE=loop4.y
loop4 : yclean y.tab.c y.tab.h ${OFILES}
ifneq (,$(findstring i86,${MACHINE}))
	${CC} ${CFLAGS} -o $@ y.tab.c loop-scanner.o
endif

y.tab.c y.tab.h: ${YFILE}
	${YACC} ${YFLAGS} ${DFLAG} ${YFILE}

.PHONY: clean
clean: yclean
	rm -f loop1 loop2 loop3 loop4 

.PHONY: yclean
yclean:
	rm -f y.tab.* y.output

