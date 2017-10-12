cmake_minimum_required(VERSION 3.8)
project(lp)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        ex01/testdir/data.h
        ex01/testdir/data01.h
        ex01/testdir/data02.h
        ex01/testdir/data03.h
        ex01/testdir/data04.h
        ex01/testdir/data05.h
        ex01/testdir/data06.h
        ex01/testdir/driver.cc
        ex01/const.h
        ex01/error.cc
        ex01/error.h
        ex01/scanner-skel.cc
        ex01/scanner.cc
        ex01/scanner.h
        ex01/y.tab.h)

add_executable(lp ${SOURCE_FILES})