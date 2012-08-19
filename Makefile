CPPFLAGS = -std=c++11 -Wall -g -DDEBUG -Iinc/
CPP = g++
VPATH = src/:inc/:lib/

configtest: testsrc/configtest.cc src/config.cc inc/config.hh
	${CPP} ${CPPFLAGS} -o bin/configtest testsrc/configtest.cc src/config.cc -lconfuse
	
