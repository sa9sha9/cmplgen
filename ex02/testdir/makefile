.cc.o:

	g++ -c $(CFLAGS) $<


CC = g++
SOURCEDIR = ..
PSOURCEDIR = ..
TESTDIR = .
CFLAGS = -I. -I${INCDIR} ${EXINCFLAG}
OBJFILES = error.o
INCDIR = ${PSOURCEDIR}
OFILES = scanner.o error.o
DRIVER = ${TESTDIR}/driver.cc

all:	test01 test02 test03 test04 test05 test06 test07 test08 test09 \
	test10 test11 test12 test13 test14 test15 test16 test17 test18 \
	test19 test20 test21 test22 test23 test24 test25 test26 test27 \
	test28 test29 test30 test31 test32 test33

test01 : ${OFILES}
	${CC} ${CFLAGS} -DTEST1 -o $@ ${DRIVER} ${OFILES}

test02 : ${OFILES}
	${CC} ${CFLAGS} -DTEST2 -o $@ ${DRIVER} ${OFILES}

test03 : ${OFILES}
	${CC} ${CFLAGS} -DTEST3 -o $@ ${DRIVER} ${OFILES}

test04 : ${OFILES}
	${CC} ${CFLAGS} -DTEST4 -o $@ ${DRIVER} ${OFILES}

test05 : ${OFILES}
	${CC} ${CFLAGS} -DTEST5 -o $@ ${DRIVER} ${OFILES}

test06 : ${OFILES}
	${CC} ${CFLAGS} -DTEST6 -o $@ ${DRIVER} ${OFILES}

test07 : ${OFILES}
	${CC} ${CFLAGS} -DTEST7 -o $@ ${DRIVER} ${OFILES}

test08 : ${OFILES}
	${CC} ${CFLAGS} -DTEST8 -o $@ ${DRIVER} ${OFILES}

test09 : ${OFILES}
	${CC} ${CFLAGS} -DTEST9 -o $@ ${DRIVER} ${OFILES}

test10 : ${OFILES}
	${CC} ${CFLAGS} -DTEST10 -o $@ ${DRIVER} ${OFILES}

test11 : ${OFILES}
	${CC} ${CFLAGS} -DTEST11 -o $@ ${DRIVER} ${OFILES}

test12 : ${OFILES}
	${CC} ${CFLAGS} -DTEST12 -o $@ ${DRIVER} ${OFILES}

test13 : ${OFILES}
	${CC} ${CFLAGS} -DTEST13 -o $@ ${DRIVER} ${OFILES}

test14 : ${OFILES}
	${CC} ${CFLAGS} -DTEST14 -o $@ ${DRIVER} ${OFILES}

test15 : ${OFILES}
	${CC} ${CFLAGS} -DTEST15 -o $@ ${DRIVER} ${OFILES}

test16 : ${OFILES}
	${CC} ${CFLAGS} -DTEST16 -o $@ ${DRIVER} ${OFILES}

test17 : ${OFILES}
	${CC} ${CFLAGS} -DTEST17 -o $@ ${DRIVER} ${OFILES}

test18 : ${OFILES}
	${CC} ${CFLAGS} -DTEST18 -o $@ ${DRIVER} ${OFILES}

test19 : ${OFILES}
	${CC} ${CFLAGS} -DTEST19 -o $@ ${DRIVER} ${OFILES}

test20 : ${OFILES}
	${CC} ${CFLAGS} -DTEST20 -o $@ ${DRIVER} ${OFILES}

test21 : ${OFILES}
	${CC} ${CFLAGS} -DTEST21 -o $@ ${DRIVER} ${OFILES}

test22 : ${OFILES}
	${CC} ${CFLAGS} -DTEST22 -o $@ ${DRIVER} ${OFILES}

test23 : ${OFILES}
	${CC} ${CFLAGS} -DTEST23 -o $@ ${DRIVER} ${OFILES}

test24 : ${OFILES}
	${CC} ${CFLAGS} -DTEST24 -o $@ ${DRIVER} ${OFILES}

test25 : ${OFILES}
	${CC} ${CFLAGS} -DTEST25 -o $@ ${DRIVER} ${OFILES}

test26 : ${OFILES}
	${CC} ${CFLAGS} -DTEST26 -o $@ ${DRIVER} ${OFILES}

test27 : ${OFILES}
	${CC} ${CFLAGS} -DTEST27 -o $@ ${DRIVER} ${OFILES}

test28 : ${OFILES}
	${CC} ${CFLAGS} -DTEST28 -o $@ ${DRIVER} ${OFILES}

test29 : ${OFILES}
	${CC} ${CFLAGS} -DTEST29 -o $@ ${DRIVER} ${OFILES}

test30 : ${OFILES}
	${CC} ${CFLAGS} -DTEST30 -o $@ ${DRIVER} ${OFILES}

test31 : ${OFILES}
	${CC} ${CFLAGS} -DTEST31 -o $@ ${DRIVER} ${OFILES}

test32 : ${OFILES}
	${CC} ${CFLAGS} -DTEST32 -o $@ ${DRIVER} ${OFILES}

test33 : ${OFILES}
	${CC} ${CFLAGS} -DTEST33 -o $@ ${DRIVER} ${OFILES}

scanner.o : ${SOURCEDIR}/scanner.cc
	${CC} -c ${CFLAGS} -o $@ $^

error.o: ${PSOURCEDIR}/error.cc
	${CC} -c -o $@ ${CFLAGS} $^

clean:
	rm -f *.o test[0-9][0-9] test[0-9][0-9].result tmp.in

