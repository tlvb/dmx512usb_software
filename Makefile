CPPFLAGS = -std=c++11 -Wall -g -DDEBUG -Iinc/
CPP = g++

bin/serialtest: configtest_successful_run testsrc/serialtest.cc serial.o config.o
	${CPP} ${CPPFLAGS} -o bin/serialtest testsrc/serialtest.cc serial.o config.o -lconfuse

.PHONY: configtest_successful_run
configtest_successful_run: bin/configtest
	./bin/configtest

bin/configtest: testsrc/configtest.cc config.o
	${CPP} ${CPPFLAGS} -o bin/configtest testsrc/configtest.cc config.o -lconfuse

serial.o: src/serial.cc inc/serial.hh
	${CPP} ${CPPFLAGS} -c src/serial.cc

config.o: src/config.cc inc/config.hh
	${CPP} ${CPPFLAGS} -c src/config.cc -lconfuse
	
